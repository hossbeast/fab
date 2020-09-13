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
#include <termios.h>

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/time.h>

#include "xlinux/xpthread.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xfutex.h"
#include "xlinux/xepoll.h"
#include "xlinux/xunistd.h"
#include "xapi/trace.h"

#include "logger.h"
#include "fab/client.h"
#include "fab/ipc.h"
#include "fab/FAB.errtab.h"

#include "ui_thread.h"
#include "xcurses.h"
#include "logging.h"
#include "params.h"
#include "explorer.h"
#include "display.h"
#include "client_thread.h"

static xapi ui_thread()
{
  enter;

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

  g_params.thread_ui = gettid();

  logs(L_ERROR, "UI THREAD");

  fdtermin = fdopen(100, "r+");
  fdtermout = fdopen(101, "r+");

  fatal(xepoll_create, &epfd);
  memset(&events[0], 0, sizeof(events[0]));
  events[0].events = EPOLLIN | EPOLLPRI | EPOLLERR;
  fatal(xepoll_ctl, epfd, EPOLL_CTL_ADD, 100, &events[0]);
  sigemptyset(&sigs);

  // curses initialization - before signal handlers setup
  fatal(xinitscr, 0);
  fatal(xnewterm, &scr, 0, fdtermout, fdtermin);
  fatal(start_color);
  use_default_colors(); // somehow, this makes transparency work
  fatal(noecho);          // dont display user input
  fatal(nonl);            /* Disable conversion and detect newlines from input. */
  // in cbreak mode, ctrl+c generates SIGINT
  fatal(cbreak);
  fatal(keypad, stdscr, TRUE);
  curs_set(0);            /* cursor visibility state */
  leaveok(stdscr, false); /* dont update cursor */
  fatal(clear);
  fatal(refresh);         /* mark stdscr as up-to-date so the implicit refresh of getch is suppressed */
  timeout(-1);

  // setup colors
  init_pair(1, COLOR_WHITE, -1);
  init_pair(2, COLOR_RED, -1);
  init_pair(3, COLOR_WHITE, -1);

  // starts out in the explorer
  fatal(explorer_display_switch);
  client_thread_redrive();

  while(!g_params.shutdown)
  {
logs(L_ERROR, "ui thread drawing");
    fatal(g_display->draw);

    // receive epoll events or signal
logs(L_ERROR, "ui thread read start");
    fatal(uxepoll_pwait, &r, epfd, events, 1, -1, &sigs);
logs(L_ERROR, "ui thread read done");

    if(r < 1) {
      continue;
    }

    fatal(xread, 100, keys, sizeof(keys), (void*)&bytes);

int x;
for(x = 0; x < bytes; x++) {
  printf(" 0x%02hhx", keys[x]);
}
printf("\n");

    if(bytes > 2 && keys[0] == 0x1b && keys[1] == 0x4f && keys[2] == 0x41) {
      key = KEY_UP;
    } else if(bytes > 2 && keys[0] == 0x1b && keys[1] == 0x4f && keys[2] == 0x42) {
      key = KEY_DOWN;
    } else if(bytes > 2 && keys[0] == 0x1b && keys[1] == 0x5b && keys[2] == 0x5a) {
      key = KEY_BTAB;
    } else {
      key = keys[0];
    }

logf(L_ERROR, "ui thread getch -> %d", key);
    if(key == '\r' || key == '\n') {
      key = KEY_ENTER;
    }

    if((r = g_display->keypress(key)) == REBIND)
    {
      client_thread_redrive();
    }
    else if(r == EXIT)
    {
      break;
    }
  }

finally:
#if DEBUG || DEVEL
  logs(L_IPC, "terminating");
#endif

  curs_set(1);
  endwin();
coda;
}

static void * ui_thread_jump(void * arg)
{
  enter;

  xapi R;
  fatal(ui_thread);

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL || XAPI
    xapi_infos("name", "fabc/ui");
    xapi_infof("pgid", "%ld", (long)getpgid(0));
    xapi_infof("pid", "%ld", (long)getpid());
    xapi_infof("tid", "%ld", (long)gettid());
    fatal(logger_xtrace_full, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#else
    fatal(logger_xtrace_pithy, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#endif
  }
conclude(&R);

  __atomic_fetch_sub(&g_params.thread_count, 1, __ATOMIC_RELAXED);
  syscall(SYS_tgkill, g_params.pid, g_params.thread_main, SIGUSR1);
  return 0;
}

//
// public
//

xapi ui_thread_launch()
{
  enter;

  pthread_t pthread_id;
  pthread_attr_t attr;
  int rv;

  fatal(xpthread_attr_init, &attr);
  fatal(xpthread_attr_setdetachstate, &attr, PTHREAD_CREATE_DETACHED);

  g_params.thread_count++;
  if((rv = pthread_create(&pthread_id, &attr, ui_thread_jump, 0)) != 0)
  {
    g_params.thread_count--;
    tfail(perrtab_KERNEL, rv);
  }

finally:
  pthread_attr_destroy(&attr);
coda;
}
//    key = getch();
//    if(key == ERR && errno == EINTR) {
//      continue;
//    } else if(key == ERR) {
//      fail(SYS_ABORT);
//    }

//    if(r == -1 && errno == EINTR) {
//      continue;
//    } else if(r == -1) {
//      fail(SYS_ABORT);
//    }
//printf("read : %d", r);
//if(r == -1) {
//  printf(" err %d %s", errno, strerror(errno));
//}

//  struct termios termios;
//  tcgetattr(100, &termios);
//  termios.c_lflag &= ~ICANON;
//  termios.c_cc[VTIME] = 1;
//  tcsetattr(100, TCSANOW, &termios);
