/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdio.h>
#include <inttypes.h>
#include <errno.h>

#include "bam/load.h"
#include "narrator/load.h"
#include "bacon/load.h"

#include "xlinux/xfutex.h"
#include "xlinux/xpthread.h"
#include "xlinux/xsignal.h"

#include "common/attrs.h"
#include "bam/client.h"
#include "bam/events.h"
#include "bam/ipc.h"
#include "bam/sigutil.h"

#include "args.h"
#include "build.h"
#include "command.h"
#include "kill.h"
#include "params.h"
#include "backtracef.h"

#include "barriers.h"

__thread int32_t tid;
static uint64_t eventsubid;
static bool waserror;

static void client_acquired(bam_client * restrict client, bamipc_message * restrict msg)
{
  uint32_t h;
  uint32_t i;
  char buf[64];
  size_t z;
  struct bamipc_page *page;

  if(g_args.verbose < 2) {
    return;
  }

  page = containerof(msg, bamipc_page, msg);
  h = page->head;
  i = page - client->shm->server_ring.pages;
  z = attrs32_zcat(buf, sizeof(buf), bamipc_msg_type_attrs, msg->type);
  printf("rx head %5u idx %5u id 0x%016"PRIx64" code %8d type %.*s", h, i, msg->id, msg->code, (int)z, buf);
  if(msg->type == BAMIPC_MSG_EVENTS)
  {
    z = attrs32_zcat(buf, sizeof(buf), bamipc_event_type_attrs, msg->evtype);
    printf(" attrs %.*s", (int)z, buf);
  }
  else if(msg->attrs)
  {
    printf(" attrs 0x%08x", msg->attrs);
  }
  printf(" size %hu", msg->size);
  if(msg->type == BAMIPC_MSG_RESPONSE || (msg->type == BAMIPC_MSG_EVENTS && msg->evtype == BAMIPC_EVENT_SYSTEM_STATE))
  {
    printf(" %.*s", (int)msg->size, msg->text);
  }
  printf("\n");
}

void client_post(struct bam_client * restrict client, bamipc_message * restrict msg)
{
  bam_client_post(client, msg);

  uint32_t h;
  char buf[64];
  size_t z;

  if(g_args.verbose < 1) {
    return;
  }

  h = containerof(msg, bamipc_page, msg)->tail;
  z = attrs32_zcat(buf, sizeof(buf), bamipc_msg_type_attrs, msg->type);
  printf("tx tail %5u id 0x%016"PRIx64" code %8d type %.*s", h, msg->id, msg->code, (int)z, buf);
  if(msg->type == BAMIPC_MSG_EVENTS)
  {
    z = attrs32_zcat(buf, sizeof(buf), bamipc_event_type_attrs, msg->evtype);
    printf(" attrs %.*s", (int)z, buf);
  }
  else if(msg->attrs)
  {
    printf(" attrs 0x%08x", msg->attrs);
  }
  printf(" size %hu ", msg->size);
  printf("%.*s", (int)msg->size, msg->text);
  printf("\n");
}

static void xmain()
{
  sigset_t sigs;
  siginfo_t info;
  bam_client * client = 0;
  bamipc_message * msg = 0;
  struct timespec interval;
  int err;
  uint16_t pulse;
  int iter;
  int channel_shmid;
  int x;
  int r = 0;

  sigutil_install_handlers();
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGRTMIN);
  xpthread_sigmask(SIG_SETMASK, &sigs, 0);

#if CONFIG_DEVEL
  static char space[512];
  snprintf(space, sizeof(space), "%s", "/home/todd/fab-build/bam/bamd/devel/bamd");
  g_bam_client_bamd_path = space;
  if(g_args.system_config_path.s) {
    g_bam_client_system_config_path = strndup(g_args.system_config_path.s, g_args.system_config_path.len);
  }
  if(g_args.user_config_path.s) {
    g_bam_client_user_config_path = strndup(g_args.user_config_path.s, g_args.user_config_path.len);
  }
  if(g_args.default_filesystem_invalidate.s) {
    g_bam_client_default_filesystem_invalidate = strndup(g_args.default_filesystem_invalidate.s, g_args.default_filesystem_invalidate.len);
  }
  if(g_args.sweeper_period_nsec.s) {
    g_bam_client_sweeper_period_nsec = strndup(g_args.sweeper_period_nsec.s, g_args.sweeper_period_nsec.len);
  }
#endif

  interval.tv_sec = 0;
  interval.tv_nsec = 500000000;   // 500 millis

  bam_client_create(&client, ".", XQUOTE(BAMIPCDIR));
  bam_client_prepare(client);

  /* kill an existing bamd - waiting at most 3s */
  if(g_args.kill && client->bamd_pid != 0)
  {
    bam_client_kill(client);

    for(x = 0; x < (5 * 3); x++)
    {
      r = uxkill(client->bamd_pid, 0);
      if(r) {
        break;
      }
      usleep(200 * 1000);
    }
  }
  if(g_args.no_launch && client->bamd_pid == 0)
  {
    fprintf(stderr, "bamd not running\n");
    exit(1);
  }
  if(g_args.kill) {
    if(client->bamd_pid != 0 && r == 0) {
      fprintf(stderr, "bamd did not respond - SIGKILL\n");
      uxkill(client->bamd_pid, SIGKILL);
    }
    client->bamd_pid = 0;
  }

  if(g_cmd == &kill_command) {
    goto end;
  }

  bam_client_solicit(client);

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

    RTA;
  }

  channel_shmid = info.si_value.sival_int;
  bam_client_attach(client, channel_shmid);

  g_cmd->attached(g_cmd, client);

  /* always-on subscriptions */
  msg = bam_client_produce(client);
  msg->type = BAMIPC_MSG_EVENTSUB;
  eventsubid = msg->id = ++client->msgid;
  msg->attrs = 0
    | BAMIPC_EVENT_SYSTEM_STATE
    ;
  client_post(client, msg);

  /* event subscription and send the request */
  g_cmd->connected(g_cmd, client);

  /* processing */
  interval.tv_nsec = 125 * 1000 * 1000;   // 125 millis
  iter = 0;
  pulse = client->shm->server_pulse;
  while(!g_sigterm && !g_params.shutdown)
  {
    client->shm->client_pulse++;
    if(!(msg = bam_client_acquire(client))) {
      if(client->shm->server_exit) {
        /* normally the daemon will send a response message with nonzero code on error */
        fprintf(stderr, "daemon exited unexpectedly\n");
        break;
      }

      atomic_store(&client->shm->server_ring.waiters, 1);
      err = uxfutex(&client->shm->server_ring.waiters, FUTEX_WAIT, 1, &interval, 0, 0);
      if(err == EINTR || err == EAGAIN) {
        continue;
      }

      /* 5 seconds */
      if(((++iter) % 25) == 0) {
        if(pulse == client->shm->server_pulse) {
          /* watchdog timeout - unresponsive daemon */
          fprintf(stderr, "daemon unresponsive\n");
          break;
        }
        pulse = client->shm->server_pulse;
      }

      continue;
    }

    iter = 0;
    RUNTIME_ASSERT(msg->type);
    client_acquired(client, msg);
    if((msg->type == BAMIPC_MSG_RESPONSE && msg->id == eventsubid))
    {
      RUNTIME_ASSERT(msg->code == 0);
      bam_client_consume(client, msg);
      continue;
    }
    if((msg->type == BAMIPC_MSG_RESULT || msg->type == BAMIPC_MSG_RESPONSE) && msg->code != 0)
    {
      write(2, msg->text, msg->size);
      write(2, "\n", 1);
      waserror = true;
    }
    else if((msg->type == BAMIPC_MSG_EVENTS && msg->evtype == BAMIPC_EVENT_SYSTEM_STATE && msg->code != 0))
    {
      write(2, msg->text, msg->size);
      write(2, "\n", 1);
      waserror = true;
    }
    g_cmd->process(g_cmd, client, msg);
    bam_client_consume(client, msg);
  }

end:
  bam_client_disconnect(client);
  bam_client_xfree(client);

  g_cmd->disconnected(g_cmd);
}

int main(int argc, char ** argv)
{
  tid = syscall(SYS_gettid);

#ifdef CONFIG_DEVEL
  backtracef_setup();
#endif

  // load libraries
  bam_load();
  narrator_load();
  bacon_load();

  // load modules
  params_setup();
  args_parse(argc, argv);
  build_command_setup();

  xmain();

  // modules
  params_teardown();
  build_command_cleanup();
  args_cleanup();

  // libraries
  bam_unload();
  narrator_unload();
  bacon_unload();

  return !!(waserror || build_was_failed_slot);
}
