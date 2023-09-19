/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#include <curses.h>
#include <string.h>
#include <sys/syscall.h>

#include "xlinux/xpthread.h"
#include "xlinux/xepoll.h"
#include "xlinux/xunistd.h"

#include "ui_thread.h"
#include "xcurses.h"
#include "params.h"
#include "explorer.h"
#include "display.h"
#include "client_thread.h"
#include "threads.h"

static void ui_thread(void)
{
  int r;
  ssize_t bytes;
  int key;
  SCREEN *scr;
  FILE *fdtermout;
  FILE *fdtermin;
  char keys[64];
  struct epoll_event events[1];
  int epfd = -1;
  sigset_t sigs;

  fdtermin = fdopen(TERMINAL_IN, "r+");
  fdtermout = fdopen(TERMINAL_OUT, "r+");

  epfd = xepoll_create();
  memset(&events[0], 0, sizeof(events[0]));
  events[0].events = EPOLLIN | EPOLLPRI | EPOLLERR;
  xepoll_ctl(epfd, EPOLL_CTL_ADD, TERMINAL_IN, &events[0]);
  sigemptyset(&sigs);

  // curses initialization - before signal handlers setup
  xinitscr(0);
  xnewterm(&scr, 0, fdtermout, fdtermin);
  start_color();
  use_default_colors(); // somehow, this makes transparency work
  noecho();          // dont display user input
  nonl();            /* Disable conversion and detect newlines from input. */
  // in cbreak mode, ctrl+c generates SIGINT
  cbreak();
  keypad(stdscr, TRUE);
  curs_set(0);            /* cursor visibility state */
  leaveok(stdscr, false); /* dont update cursor */
  clear();
  refresh();         /* mark stdscr as up-to-date so the implicit refresh of getch is suppressed */
  timeout(-1);

  // setup colors
  init_pair(1, COLOR_WHITE, -1);
  init_pair(2, COLOR_RED, -1);
  init_pair(3, COLOR_WHITE, -1);

  // starts out in the explorer
  explorer_display_switch();
  client_thread_redrive();

  while(!g_params.shutdown)
  {
    g_display->redraw();

    // receive epoll events or signal
    r = uxepoll_pwait(epfd, events, 1, -1, &sigs);
    if(r < 1) {
      continue;
    }

    bytes = xread(TERMINAL_IN, keys, sizeof(keys));

    if(bytes > 2 && keys[0] == 0x1b && keys[1] == 0x4f && keys[2] == 0x41) {
      key = KEY_UP;
    } else if(bytes > 2 && keys[0] == 0x1b && keys[1] == 0x4f && keys[2] == 0x42) {
      key = KEY_DOWN;
    } else if(bytes > 2 && keys[0] == 0x1b && keys[1] == 0x5b && keys[2] == 0x5a) {
      key = KEY_BTAB;
    } else if(bytes == 1 && keys[0] == '\r') {
      key = KEY_ENTER;
    } else if(bytes == 1 && keys[0] == '\n') {
      key = KEY_ENTER;
    } else if(bytes == 1 && keys[0] == '\t') {
      key = '\t';
    } else if(bytes == 1 && keys[0] >= 0x1 && keys[0] <= 0x1a) {
      key = 0x7000 | keys[0];
    } else {
      key = keys[0];
    }

    if((r = g_display->keypress(key)) == REDRIVE)
    {
      client_thread_redrive();
    }
    else if(r == EXIT)
    {
      break;
    }
  }

  /* restore the terminal */
  curs_set(1);
  endwin();
}

static void * ui_thread_jump(void * arg)
{
  tid = g_params.thread_ui = gettid();

  ui_thread();

  __atomic_fetch_sub(&g_params.thread_count, 1, __ATOMIC_RELAXED);
  syscall(SYS_tgkill, g_params.pid, g_params.thread_main, SIGUSR1);
  return 0;
}

//
// public
//

void ui_thread_launch()
{
  pthread_t pthread_id;
  pthread_attr_t attr;

  xpthread_attr_init(&attr);
  xpthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  g_params.thread_count++;
  xpthread_create(&pthread_id, &attr, ui_thread_jump, 0);

  pthread_attr_destroy(&attr);
}
