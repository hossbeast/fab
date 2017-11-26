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

#include <sys/syscall.h>
#include <string.h>

#include "xapi.h"
#include "xapi/trace.h"
#include "xlinux/xpthread.h"
#include "xlinux/xsignal.h"
#include "moria/graph.h"
#include "moria/vertex.h"
#include "fab/ipc.h"
#include "fab/sigutil.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xunistd.h"
#include "xlinux/xinotify.h"
#include "xlinux/xdirent.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xstring.h"
#include "logger/config.h"
#include "valyria/map.h"
#include "narrator.h"

#include "notify_thread.h"
#include "params.h"
#include "logging.h"
#include "node.h"
#include "walker.h"
#include "sweeper_thread.h"
#include "path.h"
#include "inotify_mask.h"

#include "atomic.h"
#include "macros.h"

#define restrict __restrict

int in_fd;

static xapi notify_thread()
{
  enter;

  sigset_t sigs;
  char buffer[4096] = {};
  narrator * N;

  g_params.thread_notify = gettid();

  // signals handled on this thread
  sigfillset(&sigs);
  sigdelset(&sigs, FABIPC_SIGINTR);
  fatal(xpthread_sigmask, SIG_SETMASK, &sigs, 0);

#if DEBUG || DEVEL
  logs(L_IPC, "starting");
#endif

  while(!g_params.shutdown)
  {
    ssize_t r = read(in_fd, buffer, sizeof(buffer));
    if(r < 0 && errno == EINTR)
      continue;
    else if(r < 0)
      tfail(perrtab_KERNEL, errno);

    size_t o = 0;
    while(o < r)
    {
      struct inotify_event * ev = (void*)buffer + o;

      if(ev->mask & IN_IGNORED)
      {

      }
      else
      {
        if(log_would(L_FSEVENT))
        {
          fatal(log_start, L_FSEVENT, &N);
          node * n = map_get(g_nodes_by_wd, MM(ev->wd));
          char path[512];
          node_get_relative_path(n, path, sizeof(path));
          xsayf("%s/%s ", path, ev->name);
          fatal(inotify_mask_say, ev->mask, N);
          fatal(log_finish);
        }

        fatal(sweeper_thread_enqueue, ev->wd, ev->mask, ev->name, strlen(ev->name));
      }

      o += sizeof(*ev) + ev->len;
    }
  }

finally:
#if DEBUG || DEVEL
  logs(L_IPC, "terminating");
#endif

  fatal(ixclose, &in_fd);
coda;
}

static void * notify_thread_main(void * arg)
{
  enter;

  xapi R;
  logger_set_thread_name("notify");
  logger_set_thread_categories(L_NOTIFY);
  fatal(notify_thread, arg);

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL || XAPI
    xapi_infos("name", "fabd/notify");
    xapi_infof("pgid", "%ld", (long)getpgid(0));
    xapi_infof("pid", "%ld", (long)getpid());
    xapi_infof("tid", "%ld", (long)gettid());
    fatal(logger_xtrace_full, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#else
    fatal(logger_xtrace_pithy, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#endif
  }
conclude(&R);

  atomic_dec(&g_params.thread_count);
  syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, FABIPC_SIGSCH);
  return 0;
}

//
// public
//

xapi notify_thread_setup()
{
  xproxy(xinotify_init, &in_fd);
}

xapi notify_thread_cleanup()
{
  xproxy(ixclose, &in_fd);
}

xapi notify_thread_launch()
{
  enter;

  pthread_t pthread_id;
  pthread_attr_t attr;
  int rv;

  fatal(xpthread_attr_init, &attr);
  fatal(xpthread_attr_setdetachstate, &attr, PTHREAD_CREATE_DETACHED);

  atomic_inc(&g_params.thread_count);
  if((rv = pthread_create(&pthread_id, &attr, notify_thread_main, 0)) != 0)
  {
    atomic_dec(&g_params.thread_count);
    tfail(perrtab_KERNEL, rv);
  }

finally:
  pthread_attr_destroy(&attr);
coda;
}

xapi notify_thread_watch(node * n)
{
  enter;

  char space[512];

  uint32_t mask = 0;
  mask |= IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO;
  mask |= IN_ONLYDIR | IN_EXCL_UNLINK;

  // full path
  node_get_absolute_path(n, space, sizeof(space));

  fatal(xinotify_add_watch, &n->wd, in_fd, space, mask);
  fatal(map_set, g_nodes_by_wd, MM(n->wd), n);

  if(log_would(L_GRAPH))
  {
    narrator * N;
    fatal(log_start, L_GRAPH, &N);
    xsays(">> ");
    fatal(node_path_say, n, N);
    fatal(log_finish);
  }

  finally : coda;
}
