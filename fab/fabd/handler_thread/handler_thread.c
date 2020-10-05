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

#include <sys/syscall.h>

#include "xapi.h"
#include "xapi/trace.h"
#include "valyria/llist.h"
#include "valyria/stack.h"

#include "fab/sigutil.h"
#include "logger/config.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "value/writer.h"
#include "xapi/SYS.errtab.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xshm.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xpthread.h"
#include "xlinux/xfutex.h"

#include "handler_thread.h"
#include "server_thread.h"
#include "handler.h"
#include "logging.h"
#include "params.h"
#include "request_parser.h"
#include "request.internal.h"
#include "goals.h"
#include "channel.h"
#include "rcu.h"
#include "module.h"
#include "formula.h"

#include "common/attrs.h"
#include "atomics.h"
#include "barriers.h"
#include "locks.h"
#include "threads.h"

static int32_t handlers_lock;

static xapi update(handler_context * restrict ctx)
{
  enter;

  fatal(node_full_refresh);
  fatal(graph_invalidation_begin, &ctx->invalidation);
  fatal(module_full_refresh, &ctx->invalidation);
  graph_invalidation_end(&ctx->invalidation);
  fatal(graph_full_refresh);
  fatal(formula_full_refresh);

  finally : coda;
}

static xapi handle_request(handler_context * restrict ctx, fabipc_message * restrict msg)
{
  enter;

  request * request = 0;
  narrator * N;

  futex_acquire(&handler_lock);
printf("%.*s\n", (int)msg->size, msg->text);
  fatal(request_parser_parse, ctx->request_parser, msg->text, msg->size, "fab-request", &request);
  ctx->client_msg_id = msg->id;
  request->msg_id = msg->id;
  handler_consume(ctx);

  if(log_would(L_PROTOCOL))
  {
    fatal(log_start, L_PROTOCOL, &N);
    xsayf("request (msgid %u) >>\n", request->msg_id);
    fatal(request_say, request, N);
    fatal(log_finish);
  }

  fatal(update, ctx);

  /* for a request including a build command, the build lock is also required */
  if(request->build_command)
  {
    if(!trylock_acquire(&handler_build_lock))
    {
      handler_request_completes(ctx, EBUSY, MMS("build command already in progress"));
      goto XAPI_FINALLY;
    }
  }

  /* prepare to execute the request */
  fatal(handler_reset, ctx);
  fatal(graph_invalidation_begin, &ctx->invalidation);
  fatal(handler_process_request, ctx, request);
  graph_invalidation_end(&ctx->invalidation);

  if(request->build_command)
  {
    ctx->build_state = FAB_BUILD_IN_PROGRESS;
    fatal(goals_kickoff, ctx);
  }
  else
  {
    handler_request_complete(ctx, 0);

    /* trigger a new build on invalidation if autorun is enabled */
    if(ctx->invalidation.any && g_server_autorun) {
      fatal(handler_thread_launch, 0, 0, true);
    }
  }

finally:
  futex_release(&handler_lock);
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

#if DEBUG || DEVEL
  logs(L_IPC, "starting");
#endif

  sigemptyset(&sigs);
  sigaddset(&sigs, SIGUSR1);
  interval.tv_sec = 0;
  interval.tv_nsec = 125000000;   // 125 millis

  chan = ctx->chan;
  chan->server_pulse = (uint16_t)g_tid;
  chan->client_pulse = 0;

#if DEVEL
  if(g_server_initial_channel)
  {
    chan = g_server_initial_channel;
    chan->client_pid = g_params.pid;
    chan->client_tid = g_tid;
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
  while(!g_params.shutdown)
  {
    chan->server_pulse++;
    if(ctx->build_state == FAB_BUILD_IN_PROGRESS)
    {
      fatal(sigutil_timedwait, &r, &sigs, &siginfo, &interval);
      continue;
    }
    else if(ctx->build_state > FAB_BUILD_IN_PROGRESS)
    {
printf("complete, chan %p\n", ctx->chan);
      handler_request_complete(ctx, ctx->build_state == FAB_BUILD_FAILED);
      ctx->build_state = 0;
      trylock_release(&handler_build_lock);
    }

    if(!(client_msg = handler_acquire(ctx)))
    {
      if(chan->client_exit) {
printf("BREAK ; client_exit\n");
        break;
      }

      chan->client_ring.waiters = 1;
      smp_wmb();
      fatal(uxfutex, &r, &chan->client_ring.waiters, FUTEX_WAIT, 1, &interval, 0, 0);
      if(r == EINTR || r == EAGAIN) {
        continue;
      }

      if(((iter++) % 5) == 0) {
        if(pulse == chan->client_pulse) {
printf("BREAK ; client_pulse\n");
          break;
        }
        pulse = chan->client_pulse;
      }

      continue;
    }

    if(client_msg->type == FABIPC_MSG_EVENTSUB)
    {
      ctx->event_mask = client_msg->attrs;
      handler_consume(ctx);
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
coda;
}

static void * handler_thread_jump(void * arg)
{
  enter;

  xapi R;
  handler_context *ctx;
  fabipc_channel *chan = 0;

  g_tid = gettid();
  logger_set_thread_name("handler");
  logger_set_thread_categories(L_HANDLER);

  ctx = arg;

  fatal(channel_create, &chan, g_tid);
  ctx->chan = chan;

  futex_acquire(&handlers_lock);
  stack_push(&g_handlers, &ctx->stk);
  futex_release(&handlers_lock);

  fatal(handler_thread, ctx);

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL
    xapi_infos("name", "fabd/handler");
    xapi_infof("pgid", "%ld", (long)g_params.pgid);
    xapi_infof("pid", "%ld", (long)g_params.pid);
    xapi_infof("tid", "%"PRId32, g_tid);
    fatal(logger_xtrace_full, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#else
    fatal(logger_xtrace_pithy, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#endif
  }
conclude(&R);

  futex_acquire(&handlers_lock);
  stack_delete(&ctx->stk);
  futex_release(&handlers_lock);

  if(R) {
    g_params.shutdown = true;
    g_params.handler_error = true;

    /* speed up the rcu_synchronize wait time */
    syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, SIGUSR1);
  }

  rcu_synchronize();

  /* dispose of the context */
  if(chan) {
    RUNTIME_ASSERT(shmdt(chan) == 0);
  }
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

#if DEBUG || DEVEL
  logs(L_IPC, "starting");
#endif

  // signals handled on this thread
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGUSR1);

  fatal(goals_kickoff, ctx);
  fatal(sigutil_wait, &sigs, &siginfo);

finally:
#if DEBUG || DEVEL
  logs(L_IPC, "terminating");
#endif
coda;
}

static void * autorun_thread_jump(void * arg)
{
  enter;

  xapi R;
  handler_context *ctx = 0;

  g_tid = gettid();
  logger_set_thread_name("handler");
  logger_set_thread_categories(L_HANDLER);

  ctx = arg;

  futex_acquire(&handlers_lock);
  stack_push(&g_handlers, &ctx->stk);
  futex_release(&handlers_lock);

  fatal(autorun_thread, ctx);

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL
    xapi_infos("name", "fabd/handler");
    xapi_infof("pgid", "%ld", (long)g_params.pgid);
    xapi_infof("pid", "%ld", (long)g_params.pid);
    xapi_infof("tid", "%"PRId32, g_tid);
    fatal(logger_xtrace_full, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#else
    fatal(logger_xtrace_pithy, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#endif
  }
conclude(&R);

  futex_acquire(&handlers_lock);
  stack_delete(&ctx->stk);
  futex_release(&handlers_lock);

  if(R) {
    g_params.shutdown = true;
    g_params.handler_error = true;

    /* speed up the rcu_synchronize wait time */
    syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, SIGUSR1);
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

printf("launching\n");

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
  ctx = 0;

finally:
  pthread_attr_destroy(&attr);
  handler_release(ctx);
coda;
}
