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

#include <errno.h>

#include "fab/sigutil.h"
#include "fab/ipc.h"
#include "narrator.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xpthread.h"
#include "xlinux/xfutex.h"
#include "xlinux/xtime.h"

#include "handler_thread.h"
#include "run_thread.h"
#include "channel.h"
#include "goals.h"
#include "handler.h"
#include "params.h"
#include "request.internal.h"
#include "request_parser.h"
#include "server_thread.h"
#include "times.h"
#include "system_state.h"

#include "common/attrs.h"
#include "zbuffer.h"

struct futexlock handlers_lock;

static void parse_request(handler_context * restrict ctx, fabipc_message * restrict msg, request * restrict req)
{
  int exit;
  char trace[4096];
  size_t tracesz;

  if((exit = request_parser_parse(ctx->request_parser, msg->text, msg->size, "fab-request", req)))
  {
    channel_consume(ctx->chan, msg);

    tracesz = znloadw(trace, sizeof(trace), ctx->request_parser->yyu.error_str, ctx->request_parser->yyu.error_len);

    channel_responsew(ctx->chan, EINVAL, trace, tracesz);
  }
}

static void handler_thread(handler_context * restrict ctx)
{
  sigset_t sigs;
  siginfo_t siginfo;
  fabipc_message *client_msg = 0;
  union sigval sival;
  channel *chan = 0;
  struct timespec interval;
  uint16_t pulse;
  int r;
  uint16_t iter;
  rcu_thread rcu_self = { };
  command *cmd = 0;
  request req;

  request_init(&req);

  sigemptyset(&sigs);
  sigaddset(&sigs, SIGUSR1);
  interval.tv_sec = 0;
  interval.tv_nsec = MSEC_AS_NSEC(125);

  chan = ctx->chan;
  chan->ipc.server_pulse = (uint16_t)tid;
  chan->ipc.client_pulse = 0;

  sival.sival_int = chan->ipc.shmid;
  r = sigutil_uxrt_tgsigqueueinfo(ctx->client_pid, ctx->client_tid, SIGRTMIN, sival);

  chan->ipc.client_pid = ctx->client_pid;
  chan->ipc.client_tid = ctx->client_tid;

  if(chan->ipc.client_pid == 0) {
    dprintf(2, "channel not initialized\n");
    RUNTIME_ABORT();
  }

  iter = 1;
  pulse = chan->ipc.client_pulse;
  rcu_register(&rcu_self);
  while(!g_params.shutdown)
  {
    rcu_quiesce(&rcu_self);
    chan->ipc.server_pulse++;

    if(ctx->running)
    {
      sigutil_timedwait(&r, &sigs, &siginfo, &interval);
    }
    else if(cmd && !ctx->chan->error)
    {
      if(cmd->type == COMMAND_RECONCILE || cmd->type == COMMAND_RUN || cmd->type == COMMAND_AUTORUN)
      {
        if(cmd->type == COMMAND_RUN) { run_thread_autorun = false; }
        if(cmd->type == COMMAND_AUTORUN) { run_thread_autorun = true; }

        ctx->running = true;
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

      if(ctx->invalidation.any && run_thread_autorun)
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

      chan->ipc.client_ring.waiters = 1;
      smp_wmb();
      r = uxfutex(&chan->ipc.client_ring.waiters, FUTEX_WAIT, 1, &interval, 0, 0);
      if(r == EINTR || r == EAGAIN) {
        continue;
      }

      /* 5 seconds */
      if(((iter++) % 25) == 0) {
        if(pulse == chan->ipc.client_pulse) {
          break;
        }
        pulse = chan->ipc.client_pulse;
      }
    }
    else
    {
      ctx->chan->msgid = client_msg->id;

      /* message received */
      if(client_msg->type == FABIPC_MSG_EVENTSUB)
      {
        ctx->chan->event_mask |= client_msg->attrs;
        channel_consume(ctx->chan, client_msg);
        continue;
      }

      RUNTIME_ASSERT(client_msg->type == FABIPC_MSG_REQUEST);
      request_destroy(&req);
      parse_request(ctx, client_msg, &req);
      channel_consume(ctx->chan, client_msg);

      /* prepare to process the request */
      handler_reset(ctx);
      graph_invalidation_end(&ctx->invalidation);
      graph_invalidation_begin(&ctx->invalidation);
      cmd = llist_first(&req.commands, typeof(*cmd), lln);
    }
  }

  if(chan)
  {
    chan->ipc.server_exit = true;
    syscall(SYS_tgkill, chan->ipc.client_pid, chan->ipc.client_tid, SIGUSR1);
  }

  rcu_unregister(&rcu_self);
  request_destroy(&req);
}

static void * handler_thread_jump(void * arg)
{
  handler_context *ctx;
  channel *chan = 0;

  ctx = arg;
  ctx->tid = tid = gettid();

  channel_create(&chan, tid);
  ctx->chan = chan;

  futexlock_acquire(&handlers_lock);
  rcu_list_push(&g_handlers, &ctx->stk);
  futexlock_release(&handlers_lock);

  handler_thread(ctx);

  futexlock_acquire(&handlers_lock);
  rcu_list_delete(&ctx->stk);
  futexlock_release(&handlers_lock);

//  if(R) {
//    g_params.shutdown = true;
//    g_params.handler_error = true;
//  }

  rcu_synchronize();
  channel_release(chan);
  handler_release(ctx);

  atomic_dec(&g_params.thread_count);
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
  handler_context *ctx = 0;

  handler_alloc(&ctx);

  ctx->client_pid = client_pid;
  ctx->client_tid = client_tid;

  xpthread_attr_init(&attr);
  xpthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  atomic_inc(&g_params.thread_count);
  xpthread_create(&pthread_id, &attr, handler_thread_jump, ctx);

  pthread_attr_destroy(&attr);
}
