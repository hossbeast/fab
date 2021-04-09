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
#include "global.h"
#include "goals.h"
#include "handler.h"
#include "logging.h"
#include "params.h"
#include "request.internal.h"
#include "request_parser.h"
#include "server_thread.h"
#include "times.h"

static struct futexlock handlers_lock;

static xapi handle_request(handler_context * restrict ctx, fabipc_message * restrict msg)
{
  enter;

  request * request = 0;
  narrator * N;
  bool reconciled;
  xapi exit;
  char trace[4096 << 1];
  size_t trace_len;
  struct timespec interval;

  if((exit = invoke(request_parser_parse, ctx->request_parser, msg->text, msg->size, "fab-request", &request)))
  {
    handler_consume(ctx, msg);

#if DEBUG || DEVEL || XAPI
    trace_len = xapi_trace_full(trace, sizeof(trace), 0);
#else
    trace_len = xapi_trace_pithy(trace, sizeof(trace), 0);
#endif
    xapi_calltree_unwind();

    handler_request_completes(ctx, EINVAL, trace, trace_len);

    xlogs(L_WARN, L_NOCATEGORY, trace);
    goto XAPI_FINALLY;
  }

  ctx->client_msg_id = msg->id;
  request->msg_id = msg->id;
  handler_consume(ctx, msg);

  if(log_would(L_PROTOCOL))
  {
    fatal(log_start, L_PROTOCOL, &N);
    xsayf("request (msgid %u) >>\n", request->msg_id);
    fatal(request_say, request, N);
    fatal(log_finish);
  }

  if(request->first_command && request->first_command->type == COMMAND_BOOTSTRAP)
  {
    handler_request_complete(ctx, 0);
    goto XAPI_FINALLY;
  }

  /* for the reconcile command, the reconcile lock is required */
  if(request->first_command)
  {
    while(!g_params.shutdown && !trylock_acquire(&global_system_reconcile_lock))
    {
      ctx->chan->server_pulse++;
      interval.tv_sec = 0;
      interval.tv_nsec = MSEC_AS_NSEC(1);
      fatal(uxclock_nanosleep, 0, CLOCK_MONOTONIC, 0, &interval, 0);
    }
  }

  /* for a request including a build command, the global build lock is required */
  if(request->last_command)
  {
    while(!g_params.shutdown && !trylock_acquire(&handler_build_lock))
    {
      ctx->chan->server_pulse++;
      interval.tv_sec = 0;
      interval.tv_nsec = MSEC_AS_NSEC(1);
      fatal(uxclock_nanosleep, 0, CLOCK_MONOTONIC, 0, &interval, 0);
    }
  }

  if(request->first_command)
  {
    fatal(global_system_reconcile, ctx, &reconciled);

    if(!reconciled) {
      handler_request_completes(ctx, EBUSY, MMS("global reconciliation did not complete"));
      goto XAPI_FINALLY;
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

  if(ctx->errlen)
  {
    handler_request_complete(ctx, EINVAL);
  }
  else if(request->last_command)
  {
    ctx->build_state = FAB_BUILD_IN_PROGRESS;
    fatal(goals_kickoff, ctx);
  }
  else
  {
    handler_request_complete(ctx, 0);

    // no build-command in this request, but some nodes were invalidated
    if(ctx->invalidation.any && goals_autorun) {
      fatal(handler_thread_launch, 0, 0, true);
    }
  }

finally:
  trylock_release(&global_system_reconcile_lock);
  trylock_release(&handler_build_lock);
  graph_invalidation_end(&ctx->invalidation);
  fatal(request_xfree, request);
coda;
}

static xapi handler_thread(handler_context * restrict ctx)
{
  enter;

  sigset_t sigs;
  siginfo_t siginfo;
  fabipc_message *client_msg;
  union sigval sival;
  fabipc_channel *chan = 0;
  struct timespec interval;
  uint16_t pulse;
  int r;
  uint16_t iter;
  rcu_thread rcu_self = { };

#if DEBUG || DEVEL
  logs(L_IPC, "starting");
#endif

  sigemptyset(&sigs);
  sigaddset(&sigs, SIGUSR1);
  interval.tv_sec = 0;
  interval.tv_nsec = 125000000;   // 125 millis

  chan = ctx->chan;
  chan->server_pulse = (uint16_t)tid;
  chan->client_pulse = 0;

#if DEVEL
  if(g_server_initial_channel)
  {
    chan = g_server_initial_channel;
    chan->client_pid = g_params.pid;
    chan->client_tid = tid;
  }
  else
  {
#endif
    sival.sival_int = chan->shmid;
    fatal(sigutil_uxrt_tgsigqueueinfo, &r, ctx->client_pid, ctx->client_tid, SIGRTMIN, sival);

    chan->client_pid = ctx->client_pid;
    chan->client_tid = ctx->client_tid;
#if DEVEL
  }
#endif

  if(!chan->client_pid) {
    fprintf(stderr, "channel not initialized!!\n");
    fail(SYS_ABORT);
    goto XAPI_FINALIZE;
  }

  iter = 1;
  pulse = chan->client_pulse;
  rcu_register(&rcu_self);
  while(!g_params.shutdown)
  {
    rcu_quiesce(&rcu_self);
    chan->server_pulse++;

    if(ctx->build_state == FAB_BUILD_IN_PROGRESS)
    {
      fatal(sigutil_timedwait, &r, &sigs, &siginfo, &interval);
      continue;
    }
    else if(ctx->build_state > FAB_BUILD_IN_PROGRESS)
    {
      handler_request_complete(ctx, ctx->build_state == FAB_BUILD_FAILED);
      ctx->build_state = 0;
      trylock_release(&handler_build_lock);
    }

    if(!(client_msg = handler_acquire(ctx)))
    {
      if(chan->client_exit) {
        break;
      }

      chan->client_ring.waiters = 1;
      smp_wmb();
      fatal(uxfutex, &r, &chan->client_ring.waiters, FUTEX_WAIT, 1, &interval, 0, 0);
      if(r == EINTR || r == EAGAIN) {
        continue;
      }

      /* 5 seconds */
      if(((iter++) % 25) == 0) {
        if(pulse == chan->client_pulse) {
          break;
        }
        pulse = chan->client_pulse;
      }

      continue;
    }

    if(client_msg->type == FABIPC_MSG_EVENTSUB)
    {
      ctx->event_mask = client_msg->attrs;
      handler_consume(ctx, client_msg);
      continue;
    }

    RUNTIME_ASSERT(client_msg->type == FABIPC_MSG_REQUEST);
    fatal(handle_request, ctx, client_msg);
  }

finally:
  if(chan)
  {
    chan->server_exit = true;
    syscall(SYS_tgkill, chan->client_pid, chan->client_tid, SIGUSR1);
  }
#if DEVEL
  if(chan == g_server_initial_channel)
  {
    wfree(chan);
    ctx->chan = 0;
    g_server_initial_channel = 0;
  }
#endif

#if DEBUG || DEVEL
  logs(L_IPC, "terminating");
#endif

  rcu_unregister(&rcu_self);
coda;
}

static void * handler_thread_jump(void * arg)
{
  enter;

  xapi R;
  handler_context *ctx;
  fabipc_channel *chan = 0;

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
  bool reconciled;
  rcu_thread rcu_self = { };

#if DEBUG || DEVEL
  logs(L_IPC, "starting");
#endif

  rcu_register(&rcu_self);

  // signals handled on this thread
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGUSR1);

  if(!trylock_acquire(&handler_build_lock)) {
    fprintf(stderr, "build already in progress by %d\n", handler_build_lock.i32);
    goto XAPI_FINALIZE;
  }

  /* global reload */
  fatal(global_system_reconcile, ctx, &reconciled);
  if(!reconciled) {
    goto XAPI_FINALLY;
  }

  /* prosecute the same goals */
  ctx->build_state = FAB_BUILD_IN_PROGRESS;
  fatal(goals_kickoff, ctx);

  /* wait for the build to complete */
  while(!g_params.shutdown && ctx->build_state == FAB_BUILD_IN_PROGRESS) {
    rcu_quiesce(&rcu_self);
    fatal(sigutil_wait, &sigs, &siginfo);
  }

finally:
  trylock_release(&handler_build_lock);

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

  ctx = arg;
  ctx->tid = tid = gettid();
  logger_set_thread_name("autorun");
  logger_set_thread_categories(L_HANDLER);

  futexlock_acquire(&handlers_lock);
  rcu_list_push(&g_handlers, &ctx->stk);
  futexlock_release(&handlers_lock);

  fatal(autorun_thread, ctx);

finally:
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
