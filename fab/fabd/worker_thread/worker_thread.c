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
#include <unistd.h>

#include "xapi.h"
#include "xapi/trace.h"
#include "xlinux/xpthread.h"
#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xsignal.h"
#include "xlinux/xstdlib.h"
#include "logger.h"
#include "logger/config.h"
#include "valyria/llist.h"
#include "fab/sigutil.h"
#include "yyutil/box.h"

#include "worker_thread.h"
#include "work.h"
#include "params.h"
#include "logging.h"
#include "config.internal.h"

#include "rcu.h"
#include "locks.h"
#include "atomics.h"
#include "threads.h"

static int32_t g_workers_lock;
static worker_thread_context *worker_contexts;

stack g_workers = STACK_INITIALIZER(g_workers);
uint16_t g_workers_len;

static bool relaunching;
static int32_t worker_thread_lock = -1;

static xapi context_init(worker_thread_context * restrict ctx, uint16_t workers_index)
{
  enter;

  llist_init_node(&ctx->work_queue);
  ctx->workers_index = workers_index;

  finally : coda;
}

static xapi context_destroy(worker_thread_context * restrict ctx)
{
  enter;

  finally : coda;
}

static xapi worker_thread(worker_thread_context *ctx)
{
  enter;

  work *w;
  sigset_t sigs;

#if DEBUG || DEVEL
  logs(L_IPC, "starting");
#endif

  sigfillset(&sigs);
  sigdelset(&sigs, SIGUSR1);

  while(!g_params.shutdown && !relaunching)
  {
    if((w = llist_shift(&ctx->work_queue, typeof(*w), lln)) == 0) {
      fatal(uxsigwaitinfo, 0, &sigs, 0);
      continue;
    }

    fatal(w->fn, w, ctx);
    if(w->completion_tid) {
      *w->completion_sum += (ctx->workers_index + 1);
      fatal(uxtgkill, 0, g_params.pid, w->completion_tid, SIGUSR1);
    }
  }

finally:
#if DEBUG || DEVEL
  logs(L_IPC, "terminating");
#endif
  fatal(context_destroy, ctx);
coda;
}

static void * worker_thread_jump(void * arg)
{
  enter;

  xapi R;
  worker_thread_context *ctx;
  bool last;

  ctx = arg;
  g_tid = ctx->tid = gettid();
  logger_set_thread_name("worker");
  logger_set_thread_categories(L_WORKER);

  futex_acquire(&g_workers_lock);
  stack_push(&g_workers, &ctx->stk);
  futex_release(&g_workers_lock);

  fatal(worker_thread, arg);

finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL
    xapi_infos("name", "fabd/worker");
    xapi_infof("pgid", "%ld", (long)getpgid(0));
    xapi_infof("pid", "%ld", (long)getpid());
    xapi_infof("tid", "%ld", (long)gettid());
    fatal(logger_xtrace_full, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#else
    fatal(logger_xtrace_pithy, L_ERROR, L_NONAMES, XAPI_TRACE_COLORIZE | XAPI_TRACE_NONEWLINE);
#endif
  }
conclude(&R);

  futex_acquire(&g_workers_lock);
  stack_delete(&ctx->stk);
  last = stack_empty(&g_workers);
  futex_release(&g_workers_lock);

  if(R) {
    g_params.shutdown = true;
    g_params.handler_error = true;
    syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, SIGUSR1);
  }

  rcu_synchronize();
  context_destroy(ctx);

  atomic_dec(&g_params.thread_count);

  if(relaunching)
  {
    /* for the last to exit, force destroy the lock */
    if(last)
    {
      worker_thread_lock = g_tid;
      futex_release(&worker_thread_lock);
    }
  }
  else
  {
    syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, SIGUSR1);
  }

  return 0;
}

static xapi worker_thread_launch(worker_thread_context *ctx)
{
  enter;

  pthread_t pthread_id;
  pthread_attr_t attr;
  int rv;

  fatal(xpthread_attr_init, &attr);
  fatal(xpthread_attr_setdetachstate, &attr, PTHREAD_CREATE_DETACHED);

  atomic_inc(&g_params.thread_count);
  if((rv = pthread_create(&pthread_id, &attr, worker_thread_jump, ctx)) != 0)
  {
    atomic_dec(&g_params.thread_count);
    tfail(perrtab_KERNEL, rv);
  }

finally:
  pthread_attr_destroy(&attr);
coda;
}

//
// public
//

xapi worker_thread_reconfigure(config * restrict cfg, bool dry)
{
  enter;

  int16_t concurrency;
  worker_thread_context *worker;
  int x;

  if(cfg->workers.changed && !dry)
  {
    relaunching = true;
    stack_foreach(&g_workers, worker, stk) {
      fatal(sigutil_tgkill, g_params.pid, worker->tid, SIGUSR1);
    }
    if(!stack_empty(&g_workers))
    {
      futex_acquire(&worker_thread_lock);
      futex_release(&worker_thread_lock);
      worker_thread_lock = -1;
    }
    relaunching = false;

    // apply new config
    box_int16_setif(cfg->workers.concurrency, &concurrency);
    if(concurrency == 0)
    {
      concurrency = g_params.procs;
    }

    // launch
    wfree(worker_contexts);
    fatal(xmalloc, &worker_contexts, concurrency * sizeof(*worker_contexts));
    g_workers_len = concurrency;
    for(x = 0; x < g_workers_len; x++)
    {
      fatal(context_init, &worker_contexts[x], x);
      fatal(worker_thread_launch, &worker_contexts[x]);
    }
  }

  finally : coda;
}

xapi worker_thread_cleanup()
{
  enter;

  wfree(worker_contexts);

  finally : coda;
}
