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

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include "fab/load.h"
#include "narrator/load.h"
#include "value/load.h"

#include "fab/client.h"
#include "fab/sigutil.h"
#include "xlinux/xsignal.h"
#include "xlinux/xunistd.h"

#include "args.h"
#include "client_thread.h"
#include "params.h"
#include "ui_thread.h"

__thread int32_t tid;

static void xmain()
{
  sigset_t sigs;
  siginfo_t info;
  int fd;

#if DEVEL
  char space[512];
  snprintf(space, sizeof(space), "%s/../fabd/fabd.devel.xapi", g_params.exedir);
  g_fab_client_fabd_path = space;
#endif

  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

  // save stdin/stdout - ui thread fetches them from here
  xdup2(0, TERMINAL_IN);
  xdup2(1, TERMINAL_OUT);

  // discard stdout/stderr
#if DEVEL
  fd = open("/tmp/fabc", O_CREAT | O_WRONLY);
#else
  fd = open("/dev/null", O_WRONLY);
#endif
  xdup2(fd, 1);
  xdup2(fd, 2);

  sigutil_install_handlers();
  sigfillset(&sigs);
  xsigprocmask(SIG_SETMASK, &sigs, 0);

  client_thread_launch();
  ui_thread_launch();

  while(g_params.thread_count)
  {
    sigutil_wait(&sigs, &info);
    g_params.shutdown = true;
    uxtgkill(g_params.pid, g_params.thread_client, SIGUSR1);
    uxtgkill(g_params.pid, g_params.thread_ui, SIGUSR1);
  }
}

int main (int argc, char ** argv)
{
  tid = g_params.thread_main = gettid();

  // load libraries
  fab_load();
  narrator_load();
  value_load();

  // load modules
  params_setup();
  args_parse(argc, argv);

  xmain();

  // modules
  params_teardown();

  // libraries
  fab_unload();
  narrator_unload();
  value_unload();

  return 0;
}
