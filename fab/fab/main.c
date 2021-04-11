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

#include "xapi.h"

#include "fab/load.h"
#include "logger/load.h"
#include "narrator/load.h"
#include "valyria/load.h"
#include "xlinux/load.h"
#include "value/load.h"

#include "xapi/SYS.errtab.h"
#include "xapi/calltree.h"
#include "xapi/trace.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xfutex.h"
#include "xlinux/xpthread.h"
#include "xlinux/xsignal.h"

#include "common/attrs.h"
#include "fab/FAB.errtab.h"
#include "fab/client.h"
#include "fab/events.h"
#include "fab/ipc.h"
#include "fab/sigutil.h"

#include "MAIN.errtab.h"
#include "args.h"
#include "build.h"
#include "command.h"
#include "kill.h"
#include "logging.h"
#include "params.h"

#include "barriers.h"

__thread int32_t tid;

static void client_acquired(fab_client * restrict client, fabipc_message * restrict msg)
{
#if DEBUG || DEVEL || 1
  uint32_t h;
  char buf[64];
  size_t z;

  if(g_args.verbose < 2) {
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

void client_post(struct fab_client * restrict client, fabipc_message * restrict msg)
{
  fab_client_post(client, msg);

#if DEBUG || DEVEL || 1
  uint32_t h;
  char buf[64];
  size_t z;

  if(g_args.verbose < 1) {
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

static xapi xmain_exit;
static xapi xmain()
{
  enter;

  sigset_t sigs;
  siginfo_t info;
  fab_client * client = 0;
  fabipc_message * msg = 0;
  struct timespec interval;
  int err;
  uint16_t pulse;
  int iter;
  int channel_shmid;
  int x;
  int r;

  // parse cmdline arguments
  fatal(args_parse);

  fatal(sigutil_install_handlers);
  sigfillset(&sigs);
  sigdelset(&sigs, SIGINT);
  sigdelset(&sigs, SIGTERM);
  sigdelset(&sigs, SIGQUIT);
  sigdelset(&sigs, SIGUSR1);
  fatal(xpthread_sigmask, SIG_SETMASK, &sigs, 0);

#if DEVEL
  static char space[512];
  snprintf(space, sizeof(space), "%s/../fabd/fabd.devel.xapi", g_params.exedir);
  g_fab_client_fabd_path = space;
  if(g_args.system_config_path.s) {
    g_fab_client_system_config_path = strndup(g_args.system_config_path.s, g_args.system_config_path.len);
  }
  if(g_args.user_config_path.s) {
    g_fab_client_user_config_path = strndup(g_args.user_config_path.s, g_args.user_config_path.len);
  }
  if(g_args.project_config_path.s) {
    g_fab_client_project_config_path = strndup(g_args.project_config_path.s, g_args.project_config_path.len);
  }
  if(g_args.default_filesystem_invalidate.s) {
    g_fab_client_default_filesystem_invalidate = strndup(g_args.default_filesystem_invalidate.s, g_args.default_filesystem_invalidate.len);
  }
  if(g_args.sweeper_period_nsec.s) {
    g_fab_client_sweeper_period_nsec = strndup(g_args.sweeper_period_nsec.s, g_args.sweeper_period_nsec.len);
  }
#endif

  interval.tv_sec = 0;
  interval.tv_nsec = 500000000;   // 500 millis

  fatal(fab_client_create, &client, ".", XQUOTE(FABIPCDIR));
  fatal(fab_client_prepare, client);

  /* kill an existing fabd - waiting at most 3s */
  if(g_args.kill && client->fabd_pid != 0)
  {
    fatal(fab_client_kill, client);

    for(x = 0; x < (5 * 3); x++)
    {
      fatal(uxkill, &r, client->fabd_pid, 0);
      if(r) {
        break;
      }
      usleep(200 * 1000);
    }
    client->fabd_pid = 0;
  }
#if DEVEL
  if(g_args.no_launch && client->fabd_pid == 0)
  {
    fails(SYS_INVALID, "invariant", "fabd not running");
  }
#endif

  if(g_cmd == &kill_command) {
    goto XAPI_FINALLY;
  }

  fatal(fab_client_solicit, client);

  /* wait for the acknowledgement signal carrying the channel id */
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGRTMIN);

  while(!g_sigterm && !g_params.shutdown)
  {
    fatal(sigutil_timedwait, &err, &sigs, &info, &interval);
    if(err == 0) {                  // signal received
      break;
    } else if(err == EAGAIN) {      // timeout expired
      fail(MAIN_CHANTIME);
      goto XAPI_FINALLY;
    } else if(err == EINTR) {       // interrupted by signal
      continue;
    }

    tfail(perrtab_KERNEL, err);
  }

  channel_shmid = info.si_value.sival_int;
  fatal(fab_client_attach, client, channel_shmid);

  /* always subscribe to stdout/stderr */
  msg = fab_client_produce(client);
  msg->type = FABIPC_MSG_EVENTSUB;
  msg->attrs = 0
    | FABIPC_EVENT_BAMD_STDERR
    | FABIPC_EVENT_BAMD_STDOUT
    ;
  client_post(client, msg);

  /* event subscription and send the request */
  fatal(g_cmd->connected, g_cmd, client);

  /* processing */
  interval.tv_nsec = 125 * 1000 * 1000;   // 125 millis
  iter = 1;
  pulse = client->shm->server_pulse;
  while(!g_sigterm && !g_params.shutdown)
  {
    client->shm->client_pulse++;
    if(!(msg = fab_client_acquire(client))) {
      if(client->shm->server_exit) {
        /* normally the daemon will send a response message with nonzero code on error */
        fail(FAB_NODAEMON);
        break;
      }

      client->shm->server_ring.waiters = 1;
      smp_wmb();
      fatal(uxfutex, &err, &client->shm->server_ring.waiters, FUTEX_WAIT, 1, &interval, 0, 0);
      if(err == EINTR || err == EAGAIN) {
        continue;
      }

      /* 5 seconds */
      if(((iter++) % 25) == 0) {
        if(pulse == client->shm->server_pulse) {
          /* watchdog timeout - unresponsive daemon */
          fail(FAB_NODAEMON);
          break;
        }
        pulse = client->shm->server_pulse;
      }

      continue;
    }

    RUNTIME_ASSERT(msg->type);
    client_acquired(client, msg);
    if(msg->type == FABIPC_MSG_EVENTS && msg->evtype == FABIPC_EVENT_BAMD_STDOUT)
    {
      dprintf(1, "[out] ");
      write(1, msg->text, msg->size);
    }
    else if(msg->type == FABIPC_MSG_EVENTS && msg->evtype == FABIPC_EVENT_BAMD_STDERR)
    {
      dprintf(2, "[err] ");
      write(2, msg->text, msg->size);
    }
    else if(msg->type == FABIPC_MSG_RESULT && msg->code != 0)
    {
      write(2, msg->text, msg->size);
    }
    else
    {
      fatal(g_cmd->process, g_cmd, client, msg);
    }

    fab_client_consume(client, msg);
  }

finally:
  if(XAPI_UNWINDING)
  {
    if(XAPI_ERRVAL == FAB_NODAEMON)
    {
#if DEBUG || DEVEL
      // fabd exited - check for a coredump
#endif
    }

    if(client) {
      xapi_infos("hash", client->hash);
    }
  }

  if(client) {
    fab_client_disconnect(client);
  }
  fatal(fab_client_xfree, client);
coda;
}

static xapi xmain_jump()
{
  enter;

  fatal(xmain);

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL
    xapi_infof("pid", "%ld", (long)getpid());
    xapi_infof("tid", "%ld", (long)gettid());
    xapi_fulltrace(2, XAPI_TRACE_COLORIZE);
#else
    xapi_pithytrace(2, XAPI_TRACE_COLORIZE);
#endif

    xmain_exit = XAPI_ERRVAL;
    xapi_calltree_unwind();
  }
coda;
}

static xapi xmain_load(char ** envp)
{
  enter;

  // load libraries
  fatal(fab_load);
  fatal(logger_load);
  fatal(logger_load);
  fatal(narrator_load);
  fatal(valyria_load);
  fatal(xlinux_load);
  fatal(value_load);

  // load modules
  fatal(logging_setup, envp);
  fatal(params_setup);
  fatal(build_command_setup);

  fatal(xmain_jump);

finally:
  // modules
  params_teardown();
  fatal(build_command_cleanup);

  // libraries
  fatal(fab_unload);
  fatal(logger_unload);
  fatal(narrator_unload);
  fatal(valyria_unload);
  fatal(xlinux_unload);
  fatal(value_unload);
coda;
}

int main(int argc, char ** argv, char ** envp)
{
  enter;

  tid = syscall(SYS_gettid);

  xapi R = 0;
  fatal(xmain_load, envp);

finally:
  if(XAPI_UNWINDING)
  {
    // write failures before liblogger to stderr
    xapi_backtrace(2, XAPI_TRACE_COLORIZE);
  }

conclude(&R);
  xapi_teardown();

  R |= xmain_exit;

  return !!R;
}
