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
#include "channel.h"
#include "reconcile_thread.h"
#include "goals.h"
#include "handler.h"
#include "logging.h"
#include "params.h"
#include "request.internal.h"
#include "request_parser.h"
#include "server_thread.h"
#include "times.h"
#include "build_thread.h"

#include "common/attrs.h"

static struct futexlock handlers_lock;

static xapi parse_request(handler_context * restrict ctx, fabipc_message * restrict msg, request * restrict req)
{
  enter;

  narrator * N;
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

  if(log_would(L_PROTOCOL))
  {
    fatal(log_start, L_PROTOCOL, &N);
    xsayf("request (msgid 0x%016"PRIx64") >>\n", msg->id);
    fatal(request_say, req, N);
    fatal(log_finish);
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
  request req = { };
  bool consider_autorun = false;

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

  iter = 1;
  pulse = chan->ipc.client_pulse;
  rcu_register(&rcu_self);
  while(!g_params.shutdown)
  {
    rcu_quiesce(&rcu_self);
    chan->ipc.server_pulse++;

    if(ctx->state == HANDLER_BUILD_PENDING)
    {
      if(trylock_acquire(&handler_build_lock))
      {
        ctx->state = HANDLER_BUILD_IN_PROGRESS;
        fatal(goals_kickoff, ctx);
      }
      else
      {
        fatal(sigutil_timedwait, &r, &sigs, &siginfo, &interval);
      }
    }
    else if(ctx->state == HANDLER_BUILD_IN_PROGRESS)
    {
      fatal(sigutil_timedwait, &r, &sigs, &siginfo, &interval);
    }
    else if(ctx->state == HANDLER_BUILD_SUCCEEDED || ctx->state == HANDLER_BUILD_FAILED)
    {
      ctx->state = 0;
      trylock_release(&handler_build_lock);
    }
    else if(ctx->state == HANDLER_RECONCILE_PENDING)
    {
      if(trylock_acquire(&reconcile_lock))
      {
        ctx->state = HANDLER_RECONCILE_IN_PROGRESS;
        fatal(reconcile_thread_launch, ctx);
      }
      else
      {
        fatal(sigutil_timedwait, &r, &sigs, &siginfo, &interval);
      }
    }
    else if(ctx->state == HANDLER_RECONCILE_IN_PROGRESS)
    {
      fatal(sigutil_timedwait, &r, &sigs, &siginfo, &interval);
    }
    else if(ctx->state == HANDLER_RECONCILE_DONE)
    {
      ctx->state = 0;
      trylock_release(&reconcile_lock);
    }
    else if(cmd)
    {
      if(cmd->last)
      {
        ctx->state = HANDLER_BUILD_PENDING;
      }
      else if(cmd->first)
      {
        ctx->state = HANDLER_RECONCILE_PENDING;
      }

      fatal(handler_process_command, ctx, cmd);
      cmd = llist_next(&req.commands, cmd, lln);
    }
    else if(client_msg)
    {
      channel_response(ctx->chan, 0);
      client_msg = 0;

      if(consider_autorun && ctx->invalidation.any && goals_autorun)
      {
        /* no build-command in this request, but some nodes were invalidated while processing it */
        fatal(handler_thread_launch, 0, 0, true);
      }
    }
    else if(!(client_msg = channel_acquire(ctx->chan)))
    {
      if(chan->ipc.client_exit) {
        break;
      }

      chan->ipc.client_ring.waiters = 1;
      smp_wmb();
      fatal(uxfutex, &r, &chan->ipc.client_ring.waiters, FUTEX_WAIT, 1, &interval, 0, 0);
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
      consider_autorun = false;

      /* message received */
      if(client_msg->type == FABIPC_MSG_EVENTSUB)
      {
        ctx->event_mask = client_msg->attrs;
        channel_consume(ctx->chan, client_msg);
        continue;
      }

      RUNTIME_ASSERT(client_msg->type == FABIPC_MSG_REQUEST);
      fatal(parse_request, ctx, client_msg, &req);
      channel_consume(ctx->chan, client_msg);

      /* prepare to process the request */
      fatal(handler_reset, ctx);
      graph_invalidation_end(&ctx->invalidation);
      fatal(graph_invalidation_begin, &ctx->invalidation);
      cmd = llist_first(&req.commands, typeof(*cmd), lln);
      consider_autorun = !req.has_last_command;
    }
  }

finally:
  trylock_release(&handler_build_lock);
  trylock_release(&reconcile_lock);

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

  atomic_dec(&g_params.thread_count);
  syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, SIGUSR1);
  return 0;
}

static xapi autorun_thread(handler_context * restrict ctx)
{
  enter;

  sigset_t sigs;
  siginfo_t siginfo;
  rcu_thread rcu_self = { };

#if DEBUG || DEVEL
  logs(L_IPC, "starting");
#endif

  rcu_register(&rcu_self);

  // signals handled on this thread
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGUSR1);

  if(!trylock_acquire(&reconcile_lock)) {
    fprintf(stderr, "reconcile already in progress %d\n", reconcile_lock.i32);
    goto XAPI_FINALIZE;
  }

  if(!trylock_acquire(&handler_build_lock)) {
    fprintf(stderr, "build already in progress by %d\n", handler_build_lock.i32);
    goto XAPI_FINALIZE;
  }

  /* global reconciliation */
  ctx->state = HANDLER_RECONCILE_IN_PROGRESS;
  fatal(reconcile_thread_launch, ctx);
  while(!g_params.shutdown && ctx->state == HANDLER_RECONCILE_IN_PROGRESS) {
    rcu_quiesce(&rcu_self);
    fatal(sigutil_wait, &sigs, &siginfo);
  }
  if(ctx->chan->error) {
    goto XAPI_FINALLY;
  }

  /* prosecute the same goals */
  ctx->state = HANDLER_BUILD_IN_PROGRESS;
  fatal(goals_kickoff, ctx);

  /* wait for the build to complete */
  while(!g_params.shutdown && ctx->state == HANDLER_BUILD_IN_PROGRESS) {
    rcu_quiesce(&rcu_self);
    fatal(sigutil_wait, &sigs, &siginfo);
  }

finally:
  trylock_release(&handler_build_lock);
  trylock_release(&reconcile_lock);

#if DEBUG || DEVEL
  logs(L_IPC, "terminating");
#endif

  rcu_unregister(&rcu_self);
coda;
}

static void * autorun_thread_jump(void * arg)
{
  enter;

  xapi R;
  handler_context *ctx = 0;
  channel *chan = 0;

  ctx = arg;
  ctx->tid = tid = gettid();
  logger_set_thread_name("autorun");
  logger_set_thread_categories(L_HANDLER);

  fatal(channel_create, &chan, tid);
  ctx->chan = chan;

  printf("[%d] autorun thread start\n", tid);

  futexlock_acquire(&handlers_lock);
  rcu_list_push(&g_handlers, &ctx->stk);
  futexlock_release(&handlers_lock);

  fatal(autorun_thread, ctx);

finally:
  printf("[%d] autorun thread done\n", tid);

  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL
    xapi_infos("name", "autorun");
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

  atomic_dec(&g_params.thread_count);
  syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, SIGUSR1);
  return 0;
}

//
// public
//

xapi handler_thread_launch(pid_t client_pid, pid_t client_tid, bool autorun)
{
  enter;

  pthread_t pthread_id;
  pthread_attr_t attr;
  int rv;
  handler_context *ctx = 0;
  void *(*handler_fn)(void *);

  fatal(handler_alloc, &ctx);

  ctx->client_pid = client_pid;
  ctx->client_tid = client_tid;
  ctx->autorun = autorun;

  fatal(xpthread_attr_init, &attr);
  fatal(xpthread_attr_setdetachstate, &attr, PTHREAD_CREATE_DETACHED);

  if(ctx->autorun)
  {
    handler_fn = autorun_thread_jump;
  }
  else
  {
    handler_fn = handler_thread_jump;
  }
  atomic_inc(&g_params.thread_count);
  if((rv = pthread_create(&pthread_id, &attr, handler_fn, ctx)) != 0)
  {
    atomic_dec(&g_params.thread_count);
    tfail(perrtab_KERNEL, rv);
  }

finally:
  pthread_attr_destroy(&attr);
coda;
}


#if 0
  /* if the bootstrap thread has failed, bail with its error */
  if(bootstrap_err.l) {
    channel_responsew(ctx->chan, ENOSYS, bootstrap_err.s, bootstrap_err.l);
    g_params.shutdown = true;
    goto XAPI_FINALLY;
  }
#endif

#if 0
  /* for the reconcile command, the reconcile lock is required */
  if(request->first_command)
  {
    while(!g_params.shutdown && !trylock_acquire(&reconcile_lock))
    {
      ctx->chan->ipc.server_pulse++;
      interval.tv_sec = 0;
      interval.tv_nsec = MSEC_AS_NSEC(1);
      fatal(uxclock_nanosleep, 0, CLOCK_MONOTONIC, 0, &interval, 0);
    }
  }


  if(request->first_command)
  {
    while(!g_params.shutdown && !trylock_acquire(&reconcile_lock))
    {
      rcu_quiesce(&rcu_self);
      ctx->chan->ipc.server_pulse++;
      fatal(sigutil_timedwait, &r, &sigs, &siginfo, &interval);
    }

    ctx->state = HANDLER_RECONCILE_IN_PROGRESS;
    fatal(reconcile_thread_launch, ctx);
  }

  if(request->first_command && request->first_command->type == COMMAND_BOOTSTRAP)
  {
    channel_response(ctx->chan, 0);
    goto XAPI_FINALLY;
  }

  /* for a request including a build command, the global build lock is required */
  if(request->last_command)
  {
    while(!g_params.shutdown && !trylock_acquire(&handler_build_lock))
    {
      rcu_quiesce(&rcu_self);
      ctx->chan->ipc.server_pulse++;
      fatal(sigutil_timedwait, &r, &sigs, &siginfo, &interval);
    }
  }

  if(request->last_command)
  {
    goals_autorun = (request->last_command->type == COMMAND_AUTORUN);
  }

  /* prepare to execute the request */
  fatal(handler_reset, ctx);
  fatal(graph_invalidation_begin, &ctx->invalidation);
  fatal(handler_process_request, ctx, request);
  graph_invalidation_end(&ctx->invalidation);

  if(request->last_command)
  {
    fatal(goals_kickoff, ctx);
  }
  else
  {
    channel_response(ctx->chan, 0);

    // no build-command in this request, but some nodes were invalidated
    if(ctx->invalidation.any && goals_autorun) {
      fatal(handler_thread_launch, 0, 0, true);
    }
  }
#endif
