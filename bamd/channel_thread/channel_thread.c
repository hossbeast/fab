/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "bam/sigutil.h"
#include "bam/ipc.h"
#include "bacon/writer.h"
#include "bacon/builder.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xunistd.h"
#include "xlinux/xpthread.h"
#include "xlinux/xfutex.h"
#include "xlinux/xtime.h"
#include "common/attrs.h"

#include "channel_thread.h"
#include "channel_server_thread.h"
#include "run_thread.h"
#include "channel.h"
#include "goals.h"
#include "request_handler.h"
#include "params.h"
#include "request.h"
#include "selector_parser.h"
#include "request_parser.h"
#include "pattern_parser.internal.h"
#include "times.h"
#include "system.h"
#include "client.h"
#include "timeout_thread.h"
#include "events/events.h"
#include "disintegrate.h"

#include "zcat.h"
#include "threads.h"

static struct futexlock channel_thread_list_lock;
rcu_list channel_thread_list;

static void __attribute__((constructor)) init(void)
{
  rcu_list_init(&channel_thread_list);
}

typedef struct channel_thread_params {
  pid_t client_pid;
  pid_t client_tid;
} channel_thread_params;

static void parse_request(request_handler * restrict handler, bamipc_message * restrict msg, request * restrict req)
{
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;

  if((request_parser_parse(handler->request_parser, msg->text, msg->len, "request", req)) == 0) {
    req->msgid = msg->id;
    return;
  }

  msg = request_handler_tx(handler, BAMIPC_MSG_RESULT, BAMIPC_MSG_BACON, EINVAL);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING STRING("request parse error")
    { SET
      { MAPPING STRING("error")
        BYTES(handler->request_parser->error, handler->request_parser->error_len);
      }
    }
  }
  bacon_writer_close(&writer);

  msg->len = fixed.l;
  request_handler_post(handler, msg);
}

static void request_in_progress_error(request_handler * restrict handler, uint64_t prev_msgid, uint64_t msgid)
{
  bamipc_message *msg;
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;

  msg = channel_produce(handler->channel, msgid);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING STRING("client error")
    { SET
      { MAPPING STRING("message")
        LOADF("request received before previous request completed")
      }
      { MAPPING STRING("previous")
        UINT(prev_msgid)
      }
      { MAPPING STRING("request")
        UINT(msgid)
      }
    }
  }
  bacon_writer_close(&writer);

  msg->type = BAMIPC_MSG_RESULT;
  msg->code = EINVAL;
  msg->len = fixed.l;
  channel_post(handler->channel, msg);
}

static void runlock_error(struct request_handler * restrict handler)
{
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  events_context ectx;
  bamipc_message *msg;

  if(handler) {
    msg = request_handler_tx(handler, BAMIPC_MSG_RESULT, BAMIPC_MSG_BACON, ENOENT);
  } else {
    msg = system_error_event(&ectx);
  }

  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING STRING("run error")
    { SET
      STRING("run already in progress")
    }
  }
  bacon_writer_close(&writer);

  msg->code = ENOENT;
  msg->len = fixed.l;
  if(handler) {
    msg->type = BAMIPC_MSG_RESULT;
    request_handler_post(handler, msg);
  } else {
    events_publish(&ectx);
  }
}

static void channel_thread_fn(request_handler * restrict handler)
{
  struct channel *channel;
  struct channel_thread *thread;
  sigset_t sigs;
  bamipc_message *msg = 0;
  union sigval sival;
  struct timespec interval_short;
  struct timespec interval_long;
  command *cmd = 0;
  request req;
  bool built;
  uint64_t ping_id;
  bool run_thread_pending;
  int r;
  rcu_thread rcu;

  request_init(&req);

  sigemptyset(&sigs);
  sigaddset(&sigs, SIGUSR1);
  xpthread_sigmask(SIG_UNBLOCK, &sigs, 0);

  channel = handler->channel;
  thread = handler->channel_thread;

  /* provide the clients id */
  channel->ipc->client_id_len = zcatw(channel->ipc->client_id, sizeof(channel->ipc->client_id)
    , channel->client->id, channel->client->id_len
  );

  /* inform the client to attach */
  sival.sival_int = channel->ipc->shmid;
  sigutil_uxrt_tgsigqueueinfo(channel->ipc->client_pid, channel->ipc->client_tid, SIGRTMIN, sival);

  memset(&interval_short, 0, sizeof(interval_short));
  /* be more lenient than clients, which ping every 1s */
  memset(&interval_long, 0, sizeof(interval_long));
  timespec_add(&interval_long, MSEC_AS_NSEC(1250));

  run_thread_pending = false;
  ping_id = 0;
  built = false;
  rcu_enter(&rcu);
  while(!g_params.shutdown)
  {
    if(channel->fault) {
      break;
    }
    if(channel->ipc->client_exit) {
      break;
    }

    rcu_quiesce(&rcu);

    /* run thread has completed */
    if(run_thread_pending && !thread->run_thread_pending) {
      run_thread_pending = false;
    }

    if(cmd) {
      r = channel_rx(channel, &interval_short, &msg);
    } else {
      r = channel_rx(channel, &interval_long, &msg);
    }

    /* times out after at most 1.25s */
    if(r == BAMIPC_OK) {
      /* channel-maintenance message types */
      switch(msg->type) {
        case BAMIPC_MSG_PING:
          channel_pong(channel, msg->id);
          channel_release(channel, msg);
          msg = 0;
          continue;
        case BAMIPC_MSG_PONG:
          if(msg->id == ping_id) {
            ping_id = 0;
          }
          channel_release(channel, msg);
          msg = 0;
          continue;
        case BAMIPC_MSG_EVENTSUB:
          /* should call events_subscribe here */
          thread->event_mask = msg->attrs;
          channel_release(channel, msg);
          msg = 0;
          continue;
        default:
          break;
      }

      RUNTIME_ASSERT(msg->type == BAMIPC_MSG_REQUEST);

      if(cmd) {
        /* received a new message with a new request while still processing
         * commands from the request in the previously received message
         *
         * this is a client error - the protocol is that the client should wait
         * to receive the response to the previous request
         *
         * the exception to that rule is that either peer may issue a ping/pong
         * message to check for liveness, at any time
         */
        request_in_progress_error(handler, req.msgid, msg->id);

        dprintf(2, "request processing error - closing channel\n");
        vertex_show(&channel->client->vertex);
        break;
      }

      /* request-bearing message ; reset the timeout thread */
      timeout_thread_refresh_on_request();

      request_destroy(&req);
      parse_request(handler, msg, &req);
      handler->request_msgid = msg->id;
      channel_release(channel, msg);

      /* prepare to process the request */
      request_handler_reset(handler);
      built = false;
      if((cmd = llist_first(&req.commands, typeof(*cmd), lln)) == 0) {
        channel_response(channel, handler->request_msgid, !handler->request_error);
        msg = 0;
      }
    } else if(r == BAMIPC_ETIMEDOUT && !cmd) {
      if(ping_id) {
        /* watchdog timeout - unresponsive client */
        dprintf(2, "unresponsive client - closing channel\n");
        vertex_show(&channel->client->vertex);
        break;
      }
      ping_id = ++channel->tx_msgid;
      channel_ping(channel, ping_id);
    }

    if(!cmd) {
      continue;
    }

    /* perform the current command */
    switch(cmd->type) {
      case COMMAND_GOALS:
        /* invalidates //goals which triggers autorun */
        goals_set(
            channel->ipc->client_pid
          , cmd->goals.token
          , cmd->goals.label
          , cmd->goals.label_len
          , cmd->goals.reconcile
          , cmd->goals.build
          , cmd->goals.target_direct
          , cmd->goals.target_transitive
        );

        /* goals takes ownership of the selectors */
        cmd->goals.target_direct = 0;
        cmd->goals.target_transitive = 0;

        /* invalidates //goals which triggers autorun */
        handler->invalidated_any = true;
        break;
      case COMMAND_RECONCILE:
      case COMMAND_RUN:
        run_thread_pending = true;
        thread->run_thread_pending = true;
        built = true;
        if(!run_thread_launch(handler, thread, cmd->type)) {
          runlock_error(handler);
        }
        break;
      default:
        request_handler_process_command(handler, cmd);
    }
    if(handler->request_error) {
      request_destroy(&req);
      cmd = 0;
    }
    if((cmd = llist_next(&req.commands, cmd, lln)) == 0) {
      /* complete the current request */
      channel_response(channel, handler->request_msgid, !handler->request_error);
      msg = 0;

      if(handler->invalidated_any && run_thread_autorun && !built && !handler->request_error)
      {
        /* no build-command in this request, but some nodes were invalidated while processing it */
        run_thread_launch(handler, 0, COMMAND_AUTORUN);
      }
    }
  }
  rcu_leave(&rcu);

  if(channel) {
    channel->ipc->server_exit = 1;
  }

  request_destroy(&req);
}

static void *channel_thread_jump(void * arg)
{
  channel_thread_params *params;
  channel_thread thread = {};
  struct channel *channel;
  request_handler *handler;
  struct client *client;
  rcu_thread rcu;

  params = arg;
  errnop = &errno;
  thread.tid = tid = gettid();

  client = client_alloc();
  channel = channel_alloc();
  handler = request_handler_alloc();

  handler->channel = channel;
  handler->channel_thread = &thread;

  thread.channel = channel;
  channel->ipc->client_pid = params->client_pid;
  channel->ipc->client_tid = params->client_tid;

  channel->ipc->server_pid = getpid();
  channel->ipc->server_tid = tid;

  free(params);

  channel->client = client;
  client->channel = channel;

  /* publish the channel */
  futexlock_acquire(&channel_thread_list_lock);
  rcu_list_push(&channel_thread_list, &thread.stk);
  futexlock_release(&channel_thread_list_lock);

  /* publish the client */
  client_advertise(channel->client);

  channel_thread_fn(handler);

  /* rescind the channel */
  futexlock_acquire(&channel_thread_list_lock);
  rcu_list_delete(&thread.stk);
  futexlock_release(&channel_thread_list_lock);

  rcu_synchronize();

  rcu_enter(&rcu);

  graph_demolish(&client->vertex);

  channel_free(channel);
  request_handler_release(handler);

  rcu_leave(&rcu);

  atomic_fetch_dec(&g_params.thread_count);
  syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, SIGUSR1);

  return 0;
}

//
// public
//

void channel_thread_launch(pid_t client_pid, pid_t client_tid)
{
  pthread_t pthread_id;
  pthread_attr_t attr;
  channel_thread_params *params;

  xmalloc(&params, sizeof(*params));
  params->client_pid = client_pid;
  params->client_tid = client_tid;

  xpthread_attr_init(&attr);
  xpthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  atomic_fetch_inc(&g_params.thread_count);
  xpthread_create(&pthread_id, &attr, channel_thread_jump, params);

  pthread_attr_destroy(&attr);
}
