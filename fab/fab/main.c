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

#include <stdio.h>
#include <inttypes.h>
#include <errno.h>

#include "fab/load.h"
#include "narrator/load.h"
#include "value/load.h"

#include "xlinux/xfutex.h"
#include "xlinux/xpthread.h"
#include "xlinux/xsignal.h"

#include "common/attrs.h"
#include "fab/client.h"
#include "fab/events.h"
#include "fab/ipc.h"
#include "fab/sigutil.h"

#include "args.h"
#include "build.h"
#include "command.h"
#include "kill.h"
#include "params.h"

#include "barriers.h"

__thread int32_t tid;
static uint64_t eventsubid;
static bool waserror;

static void client_acquired(fab_client * restrict client, fabipc_message * restrict msg)
{
#if DEBUG || DEVEL || 1
  uint32_t h;
  char buf[64];
  size_t z;

  if(g_args.verbose < 2) {
    return;
  }

  h = containerof(msg, fabipc_page, msg)->tail; // client->shm->server_ring.head;
  z = znload_attrs32(buf, sizeof(buf), fabipc_msg_type_attrs, msg->type);
  printf("rx head %5u id 0x%016"PRIx64" code %8d type %.*s", h, msg->id, msg->code, (int)z, buf);
  if(msg->type == FABIPC_MSG_EVENTS)
  {
    z = znload_attrs32(buf, sizeof(buf), fabipc_event_type_attrs, msg->evtype);
    printf(" attrs %.*s", (int)z, buf);
  }
  else if(msg->attrs)
  {
    printf(" attrs 0x%08x", msg->attrs);
  }
  printf(" size %hu", msg->size);
  if(msg->type == FABIPC_MSG_RESPONSE || (msg->type == FABIPC_MSG_EVENTS && msg->evtype == FABIPC_EVENT_SYSTEM_STATE))
  {
    printf(" %.*s", (int)msg->size, msg->text);
  }
  printf("\n");
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

  h = containerof(msg, fabipc_page, msg)->tail; // client->shm->client_ring.tail;
  z = znload_attrs32(buf, sizeof(buf), fabipc_msg_type_attrs, msg->type);
  printf("tx tail %5u id 0x%016"PRIx64" code %8d type %.*s", h, msg->id, msg->code, (int)z, buf);
  if(msg->type == FABIPC_MSG_EVENTS)
  {
    z = znload_attrs32(buf, sizeof(buf), fabipc_event_type_attrs, msg->evtype);
    printf(" attrs %.*s", (int)z, buf);
  }
  else if(msg->attrs)
  {
    printf(" attrs 0x%08x", msg->attrs);
  }
  printf(" size %hu ", msg->size);
  printf("%.*s", (int)msg->size, msg->text);
  printf("\n");
#endif
}

static void xmain()
{
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

  sigutil_install_handlers();
  sigfillset(&sigs);
  sigdelset(&sigs, SIGINT);
  sigdelset(&sigs, SIGTERM);
  sigdelset(&sigs, SIGQUIT);
  sigdelset(&sigs, SIGUSR1);
  xpthread_sigmask(SIG_SETMASK, &sigs, 0);

#if DEVEL
  static char space[512];
  snprintf(space, sizeof(space), "%s/../fabd/fabd.devel.void", g_params.exedir);
  g_fab_client_fabd_path = space;
  if(g_args.system_config_path.s) {
    g_fab_client_system_config_path = strndup(g_args.system_config_path.s, g_args.system_config_path.len);
  }
  if(g_args.user_config_path.s) {
    g_fab_client_user_config_path = strndup(g_args.user_config_path.s, g_args.user_config_path.len);
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

  fab_client_create(&client, ".", XQUOTE(FABIPCDIR));
  fab_client_prepare(client);

  /* kill an existing fabd - waiting at most 3s */
  if(g_args.kill && client->fabd_pid != 0)
  {
    fab_client_kill(client);

    for(x = 0; x < (5 * 3); x++)
    {
      r = uxkill(client->fabd_pid, 0);
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
    fprintf(stderr, "fabd not running\n");
    RUNTIME_ABORT();
  }
#endif

  if(g_cmd == &kill_command) {
    goto end;
  }

  fab_client_solicit(client);

  /* wait for the acknowledgement signal carrying the channel id */
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGRTMIN);

  while(!g_sigterm && !g_params.shutdown)
  {
    sigutil_timedwait(&err, &sigs, &info, &interval);
    if(err == 0) {                  // signal received
      break;
    } else if(err == EAGAIN) {      // timeout expired
      fprintf(stderr, "TIMEOUT EXPIRED\n");
      goto end;
    } else if(err == EINTR) {       // interrupted by signal
      continue;
    }

    RUNTIME_ABORT();
  }

  channel_shmid = info.si_value.sival_int;
  fab_client_attach(client, channel_shmid);

  /* always-on subscriptions */
  msg = fab_client_produce(client);
  msg->type = FABIPC_MSG_EVENTSUB;
  eventsubid = msg->id = ++client->msgid;
  msg->attrs = 0
    | FABIPC_EVENT_SYSTEM_STATE
    ;
  client_post(client, msg);

  /* event subscription and send the request */
  g_cmd->connected(g_cmd, client);

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
        fprintf(stderr, "daemon exited unexpectedly\n");
        break;
      }

      client->shm->server_ring.waiters = 1;
      smp_wmb();
      err = uxfutex(&client->shm->server_ring.waiters, FUTEX_WAIT, 1, &interval, 0, 0);
      if(err == EINTR || err == EAGAIN) {
        continue;
      }

      /* 5 seconds */
      if(((iter++) % 25) == 0) {
        if(pulse == client->shm->server_pulse) {
          /* watchdog timeout - unresponsive daemon */
          fprintf(stderr, "daemon unresponsive\n");
          break;
        }
        pulse = client->shm->server_pulse;
      }

      continue;
    }

    RUNTIME_ASSERT(msg->type);
    client_acquired(client, msg);
    if((msg->type == FABIPC_MSG_RESPONSE && msg->id == eventsubid))
    {
      RUNTIME_ASSERT(msg->code == 0);
      fab_client_consume(client, msg);
      continue;
    }
    if((msg->type == FABIPC_MSG_RESULT || msg->type == FABIPC_MSG_RESPONSE) && msg->code != 0)
    {
      write(2, msg->text, msg->size);
      write(2, "\n", 1);
      waserror = true;
    }
    else if((msg->type == FABIPC_MSG_EVENTS && msg->evtype == FABIPC_EVENT_SYSTEM_STATE && msg->code != 0))
    {
      write(2, msg->text, msg->size);
      write(2, "\n", 1);
      waserror = true;
    }
    g_cmd->process(g_cmd, client, msg);
    fab_client_consume(client, msg);
  }

end:
  fab_client_disconnect(client);
  fab_client_xfree(client);
}

int main(int argc, char ** argv)
{
  tid = syscall(SYS_gettid);

  // load libraries
  fab_load();
  narrator_load();
  value_load();

  // load modules
  params_setup();
  args_parse(argc, argv);
  build_command_setup();

  xmain();

  // modules
  params_teardown();
  build_command_cleanup();

  // libraries
  fab_unload();
  narrator_unload();
  value_unload();
}
