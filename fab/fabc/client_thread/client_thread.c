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

#include "xlinux/xpthread.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xfutex.h"
#include "xlinux/xsignal.h"
#include "xapi/trace.h"
#include "common/attrs.h"

#include "fab/client.h"
#include "fab/ipc.h"
#include "fab/events.h"
#include "fab/sigutil.h"

#include "client_thread.h"
#include "display.h"
#include "logging.h"
#include "params.h"
#include "threads.h"
#include "args.h"

static bool redrive;

static void client_acquired(fab_client * restrict client, fabipc_message * restrict msg)
{
#if DEVEL
  uint32_t h;
  char buf[64];
  size_t z;

  if(g_args.verbose == 0) {
    return;
  }

  h = client->shm->server_ring.head;
  z = znload_attrs32(buf, sizeof(buf), fabipc_msg_type_attrs, msg->type);
  printf("rx head %5u id %8d code %8d type %.*s", h, msg->id, msg->code, (int)z, buf);
  if(msg->type == FABIPC_MSG_EVENTS)
  {
    z = znload_attrs32(buf, sizeof(buf), fabipc_event_type_attrs, msg->evtype);
    printf(" attrs %.*s", (int)z, buf);
  }
  else if(msg->attrs)
  {
    printf(" attrs 0x%08x", msg->attrs);
  }
  printf(" size %hu\n", msg->size);
#endif
}

static void client_posted(fab_client * restrict client, fabipc_message * restrict msg)
{
#if DEVEL
  uint32_t h;
  char buf[64];
  size_t z;

  if(g_args.verbose == 0) {
    return;
  }

  h = client->shm->client_ring.tail;
  z = znload_attrs32(buf, sizeof(buf), fabipc_msg_type_attrs, msg->type);
  printf("tx tail %5u id %8d code %8d type %.*s", h, msg->id, msg->code, (int)z, buf);
  if(msg->type == FABIPC_MSG_EVENTS)
  {
    z = znload_attrs32(buf, sizeof(buf), fabipc_event_type_attrs, msg->evtype);
    printf(" attrs %.*s", (int)z, buf);
  }
  else if(msg->attrs)
  {
    printf(" attrs 0x%08x", msg->attrs);
  }
  printf(" size %hu\n", msg->size);
  write(1, msg->text, msg->size);
  printf("\n");
#endif
}

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

  while(!g_params.shutdown)
  {
    fatal(sigutil_timedwait, &err, &sigs, &info, &interval);
    if(err == 0) {
      break;
    } else if(err == EAGAIN) {
      goto XAPI_FINALLY; // timeout expired
    } else if(err == EINTR) {
      continue;
    }

    tfail(perrtab_KERNEL, err);
  }

  channel_shmid = info.si_value.sival_int;
  fatal(fab_client_attach, client, channel_shmid);

  interval.tv_nsec = 125 * 1000 * 1000;   // 125 millis
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
        break;
      }

      client->shm->server_ring.waiters = 1;
      smp_wmb();
      fatal(uxfutex, &err, &client->shm->server_ring.waiters, FUTEX_WAIT, 1, &interval, 0, 0);
      if(err == EINTR || err == EAGAIN) {
        continue;
      }

      if(((iter++) % 25) == 0) {
        if(pulse == client->shm->server_pulse) {
          break;
        }
        pulse = client->shm->server_pulse;
      }

      continue;
    }

    client_acquired(client, msg);
    RUNTIME_ASSERT(msg->type);
    if(msg->type == FABIPC_MSG_RESULT || msg->type == FABIPC_MSG_RESPONSE) {
      fatal(g_display->rebind, client, msg);
    }
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
  tid = g_params.thread_client = gettid();
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

void client_thread_post(fab_client * restrict client, fabipc_message * restrict msg)
{
  fab_client_post(client, msg);
  client_posted(client, msg);
}
