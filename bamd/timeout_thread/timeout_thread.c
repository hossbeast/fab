/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <inttypes.h>

#include "xlinux/xpthread.h"
#include "xlinux/xsignal.h"
#include "xlinux/xtime.h"
#include "bam/ipc.h"
#include "bacon/value.h"
#include "bacon/location.h"

#include "timeout_thread.h"
#include "params.h"
#include "configuration.h"
#include "system_state.h"
#include "channel.h"
#include "events.h"

#include "atomics.h"
#include "times.h"
#include "zcat.h"

/* 10 minutes : default timeout when unspecified */
#define TIMEOUT_PERIOD_DEFAULT MIN_AS_NSEC(10)

/* 90 days : max timeout when negative value specified */
#define TIMEOUT_PERIOD_MAX (1000ULL * 1000 * 1000 * 60 * 60 * 24 * 90)

struct timespec timeout_thread_deadline;

static int64_t interval;      // configured timeout interval
static bool relaunch;

static void timeout_thread()
{
  sigset_t sigs;
  sigset_t orig;
  int r;
  uint64_t period;

  // signals handled on this thread
  sigfillset(&sigs);
  sigdelset(&sigs, SIGUSR1);

  while(!g_params.shutdown)
  {
    if(interval == 0) {
      period = TIMEOUT_PERIOD_DEFAULT;
    } else if(interval < 0) {
      period = TIMEOUT_PERIOD_MAX;
    } else {
      period = SEC_AS_NSEC(interval);
    }

    while(!g_params.shutdown && !relaunch)
    {
      xclock_gettime(CLOCK_MONOTONIC, &timeout_thread_deadline);
      timespec_add(&timeout_thread_deadline, period);

      /* sleep */
      xpthread_sigmask(SIG_SETMASK, &sigs, &orig);
      r = uxclock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &timeout_thread_deadline, 0);
      xpthread_sigmask(SIG_SETMASK, &orig, 0);

      if(r == 0) {
        /* deadline was reached */
        g_params.shutdown = true;
      }
    }
    relaunch = false;
  }
}

static void *timeout_thread_jump(void * arg)
{
  tid = g_params.thread_timeout = gettid();
  errnop = &errno;

  timeout_thread();

  atomic_fetch_dec(&g_params.thread_count);
  syscall(SYS_tgkill, g_params.pid, g_params.thread_monitor, SIGUSR1);

  return 0;
}

//
// public
//

void timeout_thread_launch()
{
  pthread_t pthread_id;
  pthread_attr_t attr;

  xpthread_attr_init(&attr);
  xpthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  atomic_fetch_inc(&g_params.thread_count);
  xpthread_create(&pthread_id, &attr, timeout_thread_jump, 0);

  pthread_attr_destroy(&attr);
}

void timeout_thread_reconfigure(struct configuration * restrict cfg, bool dryrun)
{
  channel *chan;
  bamipc_message *msg;
  bacon_value *val;

  val = cfg->timeout.interval.v;

  if(dryrun) {
    if(cfg->timeout.interval.i64 > TIMEOUT_PERIOD_MAX) {
      msg = system_error_event(&chan);
      msg->code = EINVAL;
      msg->size = 0;
      msg->size += zcatf(msg->text + msg->size, sizeof(msg->text) - msg->size, "interval %"PRIi64, cfg->timeout.interval.i64);
      msg->size += zcatf(msg->text + msg->size, sizeof(msg->text) - msg->size, " location [%d,%d - %d,%d]"
        , val->loc->f_lin + 1
        , val->loc->f_col + 1
        , val->loc->l_lin + 1
        , val->loc->l_col + 1
      );
      events_publish(chan, msg);
    }
    return;
  }

  interval = cfg->timeout.interval.i64;

  relaunch = true;
  uxtgkill(g_params.pid, g_params.thread_timeout, SIGUSR1);
}
