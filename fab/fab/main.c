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
#include "xapi/calltree.h"
#include "fab/load.h"
#include "logger/load.h"
#include "narrator/load.h"
#include "valyria/load.h"
#include "xlinux/load.h"
#include "value/load.h"

#include "xapi/trace.h"
#include "xlinux/xunistd.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xstat.h"
#include "xlinux/xshm.h"
#include "xlinux/xpthread.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xfutex.h"
#include "xlinux/xsignal.h"
#include "narrator.h"
#include "logger.h"

#include "fab/sigutil.h"
#include "fab/client.h"
#include "fab/ipc.h"
#include "fab/FAB.errtab.h"

#include "args.h"
#include "command.h"
#include "params.h"
#include "logging.h"
#include "build.h"
#include "MAIN.errtab.h"
#include "common/attrs.h"
#include "config.h"

#include "macros.h"
#include "barriers.h"

static xapi xmain_exit;
static xapi xmain()
{
  enter;

#if DEVEL
  char space[512];
#endif
  command * cmd = 0;
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

  // parse configuration files
  fatal(config_reconfigure);

  // parse cmdline arguments
  fatal(args_parse, &cmd);

  fatal(sigutil_install_handlers);
  sigfillset(&sigs);
  sigdelset(&sigs, SIGINT);
  sigdelset(&sigs, SIGTERM);
  sigdelset(&sigs, SIGQUIT);
  sigdelset(&sigs, SIGUSR1);
  fatal(xpthread_sigmask, SIG_SETMASK, &sigs, 0);

#if DEVEL
  snprintf(space, sizeof(space), "%s/../fabd/fabd.devel.xapi", g_params.exedir);
  g_fab_client_fabd_path = space;
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
  }

  fatal(fab_client_solicit, client);

  /* wait for the acknowledgement signal carrying the channel id */
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGRTMIN);

//interval.tv_sec = 1;
//interval.tv_nsec = 0;
  while(1)
  {
    fatal(sigutil_timedwait, &err, &sigs, &info, &interval);
    if(err == 0) {
      break;
    } else if(err == EAGAIN) {
      fail(MAIN_CHANTIME);
      goto XAPI_FINALLY; // timeout expired
    } else if(err == EINTR) {
      continue;
    }

printf("2 TFAIL\n");
    tfail(perrtab_KERNEL, err);
  }

  channel_shmid = info.si_value.sival_int;
  fatal(fab_client_attach, client, channel_shmid);

  /* event subscription and send the request */
  fatal(cmd->connected, cmd, client);

  /* processing */
  interval.tv_nsec = 125 * 1000 * 1000;   // 125 millis
  iter = 1;
  pulse = client->shm->server_pulse;
  while(!g_sigterm && !g_params.shutdown)
  {
    client->shm->client_pulse++;
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

//printf("%8d %d %s", msg->id, msg->type, attrs32_name_byvalue(fabipc_msg_type_attrs, msg->type));
//printf(" attrs 0x%08x", msg->attrs);
//if(msg->type == FABIPC_MSG_EVENTS)
//{
//  printf(" %s", attrs32_name_byvalue(fabipc_event_type_attrs, msg->evtype));
//}
//printf(" size %hu\n", msg->size);

    fatal(cmd->process, cmd, client, msg);

    fab_client_consume(client);
  }

printf("client: server exit %d server pulse %hu\n", client->shm->server_exit, client->shm->server_pulse);

finally:
  if(XAPI_UNWINDING)
  {
    if(XAPI_ERRVAL == FAB_NODAEMON)
    {
#if DEBUG || DEVEL
      // fabd exited - check for a coredump
#endif
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
#if DEBUG || DEVEL || XAPI
    xapi_infos("name", "fab");
    xapi_infof("pgid", "%ld", (long)getpgid(0));
    xapi_infof("pid", "%ld", (long)getpid());
    xapi_infof("tid", "%ld", (long)gettid());

    xapi_fulltrace(2, XAPI_TRACE_COLORIZE);
    //fatal(logger_trace_full, L_ERROR, XAPI_TRACE_COLORIZE);
#else
    xapi_pithytrace(2, XAPI_TRACE_COLORIZE);
    //fatal(logger_trace_pithy, L_ERROR, XAPI_TRACE_COLORIZE);
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
  fatal(config_setup);
  fatal(build_command_setup);

  fatal(xmain_jump);

finally:
  // modules
  params_teardown();
  fatal(config_cleanup);
  fatal(build_command_cleanup);
  fatal(args_teardown);

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
