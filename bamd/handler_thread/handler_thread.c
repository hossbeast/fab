/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "bam/sigutil.h"
#include "bam/ipc.h"
#include "narrator.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xunistd.h"
#include "xlinux/xpthread.h"
#include "xlinux/xfutex.h"
#include "xlinux/xtime.h"
#include "common/attrs.h"

#include "handler_thread.h"
#include "run_thread.h"
#include "channel.h"
#include "goals.h"
#include "handler.h"
#include "params.h"
#include "request.h"
#include "selector_parser.h"
#include "request_parser.h"
#include "pattern_parser.internal.h"
#include "server_thread.h"
#include "times.h"
#include "system.h"

#include "zcat.h"
#include "threads.h"

struct futexlock handlers_lock;

typedef struct handler_params {
  pid_t client_pid;
  pid_t client_tid;
} handler_params;

static void parse_request(handler_context * restrict ctx, bamipc_message * restrict msg, request * restrict req)
{
  int exit;
  char trace[4096];
  size_t size;

  if((exit = request_parser_parse(ctx->request_parser, msg->text, msg->size, "request", req)) == 0) {
    return;
  }

  channel_consume(ctx->chan, msg);

  size = 0;
  if(ctx->request_parser->selector_parser->pattern_parser->yyu && ctx->request_parser->selector_parser->pattern_parser->yyu->error_len) {
    size += zcatw(trace, sizeof(trace), ctx->request_parser->selector_parser->pattern_parser->yyu->error_str, ctx->request_parser->selector_parser->pattern_parser->yyu->error_len);
  } else if(ctx->request_parser->selector_parser->yyu.error_len) {
    size += zcatw(trace, sizeof(trace), ctx->request_parser->selector_parser->yyu.error_str, ctx->request_parser->selector_parser->yyu.error_len);
  } else {
    size += zcatw(trace, sizeof(trace), ctx->request_parser->yyu.error_str, ctx->request_parser->yyu.error_len);
  }

  channel_responsew(ctx->chan, EINVAL, trace, size);
}

static void handler_thread(handler_context * restrict ctx)
{
  sigset_t sigs;
  siginfo_t siginfo;
  bamipc_message *client_msg = 0;
  union sigval sival;
  channel *chan = 0;
  struct timespec interval;
  uint16_t pulse;
  int r;
  uint16_t iter;
  rcu_thread rcu_self = { };
  command *cmd = 0;
  request req;
  bool built;

  request_init(&req);

  sigemptyset(&sigs);
  sigaddset(&sigs, SIGUSR1);
  interval.tv_sec = 0;
  interval.tv_nsec = MSEC_AS_NSEC(125);

  chan = ctx->chan;
  chan->ipc.server_pulse = (uint16_t)tid;
  chan->ipc.client_pulse = 0;

  sival.sival_int = chan->ipc.shmid;
  r = sigutil_uxrt_tgsigqueueinfo(chan->ipc.client_pid, chan->ipc.client_tid, SIGRTMIN, sival);

  iter = 0;
  built = false;
  pulse = chan->ipc.client_pulse;
  rcu_register(&rcu_self);
  while(!g_params.shutdown)
  {
    rcu_quiesce(&rcu_self);
    chan->ipc.server_pulse++;

    if(ctx->running)
    {
      r = sigutil_timedwait(&sigs, &siginfo, &interval);
    }
    else if(cmd && !ctx->chan->error)
    {
      if(cmd->type == COMMAND_GOALS)
      {
        /* invalidates //goals which triggers autorun */
        goals_set(
            chan->ipc.client_pid
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
        ctx->invalidated_any = true;
      }
      else if(cmd->type == COMMAND_RECONCILE || cmd->type == COMMAND_RUN)
      {
        ctx->running = true;
        built = true;
        run_thread_launch(ctx, cmd->type);
      }
      else
      {
        handler_process_command(ctx, cmd);
      }
      cmd = llist_next(&req.commands, cmd, lln);
    }
    else if(client_msg)
    {
      channel_response(ctx->chan, !!ctx->chan->error);
      client_msg = 0;

      if(ctx->invalidated_any && run_thread_autorun && !built)
      {
        /* no build-command in this request, but some nodes were invalidated while processing it */
        run_thread_launch(0, COMMAND_AUTORUN);
      }
    }
    else if(!(client_msg = channel_acquire(ctx->chan)))
    {
      if(chan->ipc.client_exit) {
        break;
      }

      atomic_store(&chan->ipc.client_ring.waiters, 1);
      r = uxfutex(&chan->ipc.client_ring.waiters, FUTEX_WAIT, 1, &interval, 0, 0);
      if(r == EINTR || r == EAGAIN) {
        continue;
      }

      /* 5 seconds */
      if(((++iter) % 25) == 0) {
        if(pulse == chan->ipc.client_pulse) {
          break;
        }
        pulse = chan->ipc.client_pulse;
      }
    }
    else
    {
      iter = 0;
      ctx->chan->msgid = client_msg->id;

      /* message received */
      if(client_msg->type == BAMIPC_MSG_EVENTSUB)
      {
        ctx->event_mask = client_msg->attrs;
        channel_consume(ctx->chan, client_msg);
        continue;
      }

      RUNTIME_ASSERT(client_msg->type == BAMIPC_MSG_REQUEST);
      request_destroy(&req);
      parse_request(ctx, client_msg, &req);
      channel_consume(ctx->chan, client_msg);

      /* prepare to process the request */
      handler_reset(ctx);
      cmd = llist_first(&req.commands, typeof(*cmd), lln);
      built = false;

      /* request received ; reset the timeout thread */
      syscall(SYS_tgkill, g_params.pid, g_params.thread_timeout, SIGUSR1);
    }
  }

  if(chan)
  {
    chan->ipc.server_exit = 1;
    syscall(SYS_futex, &chan->ipc.server_ring.waiters, FUTEX_WAKE, INT32_MAX, 0, 0, 0);
  }

  rcu_unregister(&rcu_self);
  request_destroy(&req);
}

static void *handler_thread_jump(void * arg)
{
  handler_params *params;
  handler_context *handler;
  channel *chan = 0;

  params = arg;
  errnop = &errno;
  tid = gettid();

  handler_alloc(&handler);
  handler->tid = gettid();

  channel_create(&chan, tid);
  chan->ipc.client_pid = params->client_pid;
  chan->ipc.client_tid = params->client_tid;
  handler->chan = chan;
  free(params);

  futexlock_acquire(&handlers_lock);
  rcu_list_push(&g_handlers, &handler->stk);
  futexlock_release(&handlers_lock);

  handler_thread(handler);

  futexlock_acquire(&handlers_lock);
  rcu_list_delete(&handler->stk);
  futexlock_release(&handlers_lock);

  rcu_synchronize();

  channel_release(chan);
  handler_release(handler);

  atomic_fetch_dec(&g_params.thread_count);
  syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, SIGUSR1);
  return 0;
}

//
// public
//

void handler_thread_launch(pid_t client_pid, pid_t client_tid)
{
  pthread_t pthread_id;
  pthread_attr_t attr;
  handler_params *params;

  xmalloc(&params, sizeof(*params));
  params->client_pid = client_pid;
  params->client_tid = client_tid;

  xpthread_attr_init(&attr);
  xpthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  atomic_fetch_inc(&g_params.thread_count);
  xpthread_create(&pthread_id, &attr, handler_thread_jump, params);

  pthread_attr_destroy(&attr);
}
