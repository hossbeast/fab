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
#include "xapi/trace.h"
#include "fab/sigutil.h"
#include "fab/ipc.h"
#include "logger/config.h"
#include "narrator.h"
#include "xapi/SYS.errtab.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xpthread.h"
#include "xlinux/xfutex.h"
#include "xlinux/xtime.h"

#include "handler_thread.h"
#include "run_thread.h"
#include "channel.h"
#include "goals.h"
#include "handler.h"
#include "logging.h"
#include "params.h"
#include "request.internal.h"
#include "request_parser.h"
#include "server_thread.h"
#include "times.h"
#include "system_state.h"

#include "common/attrs.h"

struct futexlock handlers_lock;

static xapi parse_request(handler_context * restrict ctx, fabipc_message * restrict msg, request * restrict req)
{
  enter;

  xapi exit;
  char trace[4096];
  size_t tracesz;

  if((exit = invoke(request_parser_parse, ctx->request_parser, msg->text, msg->size, "fab-request", req)))
  {
    channel_consume(ctx->chan, msg);

#if DEBUG || DEVEL
    tracesz = xapi_trace_full(trace, sizeof(trace), 0);
#else
    tracesz = xapi_trace_pithy(trace, sizeof(trace), 0);
#endif
    xapi_calltree_unwind();

    channel_responsew(ctx->chan, EINVAL, trace, tracesz);
    goto XAPI_FINALLY;
  }

  finally : coda;
}

static xapi handler_thread(handler_context * restrict ctx)
{
  enter;

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
  bool built;

  request_init(&req);

#if DEBUG || DEVEL
  logs(L_IPC, "starting");
#endif

  sigemptyset(&sigs);
  sigaddset(&sigs, SIGUSR1);
  interval.tv_sec = 0;
  interval.tv_nsec = MSEC_AS_NSEC(125);

  chan = ctx->chan;
  chan->ipc.server_pulse = (uint16_t)tid;
  chan->ipc.client_pulse = 0;

  sival.sival_int = chan->ipc.shmid;
  fatal(sigutil_uxrt_tgsigqueueinfo, &r, ctx->client_pid, ctx->client_tid, SIGRTMIN, sival);

  chan->ipc.client_pid = ctx->client_pid;
  chan->ipc.client_tid = ctx->client_tid;

  if(chan->ipc.client_pid == 0) {
    dprintf(2, "channel not initialized\n");
    fail(SYS_ABORT);
  }

  iter = 0;
  pulse = chan->ipc.client_pulse;
  rcu_register(&rcu_self);
  while(!g_params.shutdown)
  {
    rcu_quiesce(&rcu_self);
    chan->ipc.server_pulse++;

    if(ctx->running)
    {
      fatal(sigutil_timedwait, &r, &sigs, &siginfo, &interval);
    }
    else if(cmd && !ctx->chan->error)
    {
      if(cmd->type == COMMAND_RECONCILE || cmd->type == COMMAND_RUN || cmd->type == COMMAND_AUTORUN)
      {
        if(cmd->type == COMMAND_RUN) { run_thread_autorun = false; }
        if(cmd->type == COMMAND_AUTORUN) { run_thread_autorun = true; }

        ctx->running = true;
        built = true;
        fatal(run_thread_launch, ctx, cmd->type);
      }
      else
      {
        fatal(handler_process_command, ctx, cmd);
      }
      cmd = llist_next(&req.commands, cmd, lln);
    }
    else if(client_msg)
    {
      channel_response(ctx->chan, !!ctx->chan->error);
      client_msg = 0;

      if(ctx->invalidation.any && run_thread_autorun && !built)
      {
        /* no build-command in this request, but some nodes were invalidated while processing it */
        fatal(run_thread_launch, 0, COMMAND_AUTORUN);
      }
    }
    else if(!(client_msg = channel_acquire(ctx->chan)))
    {
      if(chan->ipc.client_exit) {
        break;
      }

      atomic_store(&chan->ipc.client_ring.waiters, 1);
      fatal(uxfutex, &r, &chan->ipc.client_ring.waiters, FUTEX_WAIT, 1, &interval, 0, 0);
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
      if(client_msg->type == FABIPC_MSG_EVENTSUB)
      {
        ctx->chan->event_mask |= client_msg->attrs;
        channel_consume(ctx->chan, client_msg);
        continue;
      }

      RUNTIME_ASSERT(client_msg->type == FABIPC_MSG_REQUEST);
      request_destroy(&req);
      fatal(parse_request, ctx, client_msg, &req);
      channel_consume(ctx->chan, client_msg);

      /* prepare to process the request */
      fatal(handler_reset, ctx);
      graph_invalidation_end(&ctx->invalidation);
      fatal(graph_invalidation_begin, &ctx->invalidation);
      cmd = llist_first(&req.commands, typeof(*cmd), lln);
      built = false;
    }
  }

finally:
  if(chan)
  {
    chan->ipc.server_exit = true;
    syscall(SYS_tgkill, chan->ipc.client_pid, chan->ipc.client_tid, SIGUSR1);
  }

#if DEBUG || DEVEL
  logs(L_IPC, "terminating");
#endif

  rcu_unregister(&rcu_self);

  request_destroy(&req);
coda;
}

static void * handler_thread_jump(void * arg)
{
  enter;

  xapi R;
  handler_context *ctx;
  channel *chan = 0;

  ctx = arg;
  ctx->tid = tid = gettid();
  logger_set_thread_name("handler");
  logger_set_thread_categories(L_HANDLER);

  fatal(channel_create, &chan, tid);
  ctx->chan = chan;

  futexlock_acquire(&handlers_lock);
  rcu_list_push(&g_handlers, &ctx->stk);
  futexlock_release(&handlers_lock);

  fatal(handler_thread, ctx);

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL
    xapi_infos("thread", "handler");
    xapi_infof("pid", "%ld", (long)g_params.pid);
    xapi_infof("tid", "%"PRId32, tid);
    fatal(logger_xtrace_full, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#else
    fatal(logger_xtrace_pithy, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#endif
  }
conclude(&R);

  futexlock_acquire(&handlers_lock);
  rcu_list_delete(&ctx->stk);
  futexlock_release(&handlers_lock);

  if(R) {
    g_params.shutdown = true;
    g_params.handler_error = true;
  }

  rcu_synchronize();
  channel_release(chan);
  handler_release(ctx);

  atomic_fetch_dec(&g_params.thread_count);
  syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, SIGUSR1);
  return 0;
}

//
// public
//

xapi handler_thread_launch(pid_t client_pid, pid_t client_tid)
{
  enter;

  pthread_t pthread_id;
  pthread_attr_t attr;
  int rv;
  handler_context *ctx = 0;

  fatal(handler_alloc, &ctx);

  ctx->client_pid = client_pid;
  ctx->client_tid = client_tid;

  fatal(xpthread_attr_init, &attr);
  fatal(xpthread_attr_setdetachstate, &attr, PTHREAD_CREATE_DETACHED);

  atomic_fetch_inc(&g_params.thread_count);
  if((rv = pthread_create(&pthread_id, &attr, handler_thread_jump, ctx)) != 0)
  {
    atomic_fetch_dec(&g_params.thread_count);
    tfail(perrtab_KERNEL, rv);
  }

finally:
  pthread_attr_destroy(&attr);
coda;
}
