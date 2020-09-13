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
#include <sys/syscall.h>
#include <sys/time.h>

#include "xlinux/xpthread.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xfutex.h"
#include "xlinux/xsignal.h"
#include "xapi/trace.h"

#include "logger.h"
#include "fab/client.h"
#include "fab/ipc.h"
#include "fab/sigutil.h"
#include "fab/FAB.errtab.h"

#include "client_thread.h"
#include "logging.h"
#include "params.h"
#include "display.h"

static bool redrive;

static xapi client_thread()
{
  enter;

  fab_client * client = 0;
  fabipc_message *msg;
  int err;
  struct timespec interval;
  uint16_t pulse;
  int iter;
  sigset_t sigs;
  siginfo_t info;
  int channel_shmid;

  g_params.thread_client = gettid();

  logs(L_ERROR, "CLIENT THREAD");

  sigfillset(&sigs);
  sigdelset(&sigs, SIGUSR1);
  fatal(xpthread_sigmask, SIG_SETMASK, &sigs, 0);

  interval.tv_sec = 0;
  interval.tv_nsec = 500000000;   // 500 millis

  fatal(fab_client_create, &client, ".", XQUOTE(FABIPCDIR));
  fatal(fab_client_prepare, client);
  fatal(fab_client_solicit, client);

  /* wait for the acknowledgement signal carrying the channel id */
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGRTMIN);

  while(1)
  {
    fatal(sigutil_timedwait, &err, &sigs, &info, &interval);
    if(err == 0) {
      break;
    } else if(err == EAGAIN || g_params.shutdown) {
      goto XAPI_FINALLY; // timeout expired
    } else if(err == EINTR) {
      continue;
    }

    tfail(perrtab_KERNEL, err);
  }

  channel_shmid = info.si_value.sival_int;
printf("attach %d\n", channel_shmid);
  fatal(fab_client_attach, client, channel_shmid);

  interval.tv_nsec = 125000000;   // 125 millis
  iter = 1;
  pulse = client->shm->server_pulse;
  while(!g_params.shutdown)
  {
    client->shm->client_pulse++;
    if(redrive) {
      fatal(g_display->redrive, client);
      redrive = false;
      continue;
    }

    if(!(msg = fab_client_acquire(client))) {
      if(client->shm->server_exit) {
printf("BREAK ; server_exit\n");
        break;
      }

      client->shm->server_ring.waiters = 1;
      smp_wmb();
      fatal(uxfutex, &err, &client->shm->server_ring.waiters, FUTEX_WAIT, 1, &interval, 0, 0);
      if(err == EINTR || err == EAGAIN) {
        continue;
      }

      if(((iter++) % 5) == 0) {
        if(pulse == client->shm->server_pulse) {
printf("BREAK ; server_pulse\n");
          break;
        }
        pulse = client->shm->server_pulse;
      }

      continue;
    }

    if(msg->type == FABIPC_MSG_STDOUT) {
      printf("server/stdout: %.*s\n", msg->size, msg->text);
    } else if(msg->type == FABIPC_MSG_STDERR) {
      printf("server/stderr: %.*s\n", msg->size, msg->text);
    } else if(msg->type == FABIPC_MSG_RESULT) {
      // consume the message
      g_display->rebind(msg);
      // re-draw
      fatal(xtgkill, g_params.pid, g_params.thread_ui, SIGUSR1);
    }

    fab_client_consume(client);
  }

finally:
#if DEBUG || DEVEL
  logs(L_IPC, "terminating");
#endif

  fatal(fab_client_xfree, client);
coda;
}

static void * client_thread_jump(void * arg)
{
  enter;

  xapi R;
  fatal(client_thread);

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL || XAPI
    xapi_infos("name", "fabc/client");
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

xapi client_thread_launch()
{
  enter;

  pthread_t pthread_id;
  pthread_attr_t attr;
  int rv;

  fatal(xpthread_attr_init, &attr);
  fatal(xpthread_attr_setdetachstate, &attr, PTHREAD_CREATE_DETACHED);

  g_params.thread_count++;
  if((rv = pthread_create(&pthread_id, &attr, client_thread_jump, 0)) != 0)
  {
    g_params.thread_count--;
    tfail(perrtab_KERNEL, rv);
  }

finally:
  pthread_attr_destroy(&attr);
coda;
}

xapi client_thread_redrive(void)
{
  enter;

  redrive = true;
  fatal(xtgkill, g_params.pid, g_params.thread_client, SIGUSR1);

  finally : coda;
}
  //sigdelset(&sigs, SIGINT);
  //sigdelset(&sigs, SIGTERM);
  //sigdelset(&sigs, SIGQUIT);
#if 0
  if(err == EAGAIN) {        // timeout expired
    fail(FAB_NODAEMON);
  } else if(err == EINTR) {  // signal other than RTMIN was received
    goto XAPI_FINALLY;
  } else if(err) {
    fail(KERNEL_EINVAL);
  }
#endif
