/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <time.h>

#include "xlinux/xsignal.h"
#include "bam/probes.h"

#include "timers.h"
#include "times.h"

static struct {
  int token;
  int is_once;
  void (*once_cb)(int);
  void (*periodic_cb)(int, int);
} handlers[16];
static int handlers_len;

static void timers_handler(int signum, siginfo_t *info, void *ctx)
{
  int token;
  int overruns;
  int x;

  sigrx_probe(info);

  /* both are ints */
  overruns = info->si_overrun;
  token = info->si_value.sival_int;

  for(x = 0; x < handlers_len; x++) {
    if(handlers[x].token == token) {
      if(handlers[x].is_once) {
        if(handlers[x].once_cb) {
          handlers[x].once_cb(token);
        }
      } else if(handlers[x].periodic_cb) {
        handlers[x].periodic_cb(token, overruns);
      }
      return;
    }
  }

  RTA;
}

void timers_setup()
{
  struct sigaction timer_act;

  memset(&timer_act, 0, sizeof(timer_act));
  timer_act.sa_flags = SA_SIGINFO;
  timer_act.sa_sigaction = timers_handler;

  xsigaction(SIGUSR2, &timer_act, 0);
}

void timers_once_put(timer_t * restrict timerid, uint64_t nsec)
{
  struct itimerspec its;
  int r;

  /* fires just once */
  memset(&its, 0, sizeof(its));
  timespec_add(&its.it_value, nsec);
  r = timer_settime(*timerid, 0, &its, 0);
  RUNTIME_ASSERT(r == 0);
}

void timers_once_start(timer_t * restrict timerid, int32_t threadid, uint64_t nsec, int token, void (*handler)(int token))
{
  struct sigevent evt;
  int r;

  RUNTIME_ASSERT(handlers_len < ARRAY_LEN(handlers));
  handlers[handlers_len].token = token;
  handlers[handlers_len].is_once = 1;
  handlers[handlers_len++].once_cb = handler;

  /* create periodic timer to send signals to this thread */
  memset(&evt, 0, sizeof(evt));
  evt.sigev_notify = SIGEV_THREAD_ID;
  evt.sigev_signo = SIGUSR2;
  evt._sigev_un._tid = threadid;
  r = timer_create(CLOCK_MONOTONIC, &evt, timerid);
  RUNTIME_ASSERT(r == 0);

  /* fires just once */
  timers_once_put(timerid, nsec);
}

void timers_periodic_create(timer_t * restrict timerid, int32_t threadid, int token, void (*handler)(int, int))
{
  struct sigevent evt;
  int r;

  RUNTIME_ASSERT(handlers_len < ARRAY_LEN(handlers));
  handlers[handlers_len].token = token;
  handlers[handlers_len].is_once = 0;
  handlers[handlers_len++].periodic_cb = handler;

  /* create periodic timer that sends signals to this thread */
  memset(&evt, 0, sizeof(evt));
  evt.sigev_notify = SIGEV_THREAD_ID;
  evt.sigev_signo = SIGUSR2;
  evt.sigev_value.sival_int = token;
  evt._sigev_un._tid = threadid;
  r = timer_create(CLOCK_MONOTONIC, &evt, timerid);
  RUNTIME_ASSERT(r == 0);
}

void timers_periodic_put(timer_t timerid, uint64_t nsec)
{
  struct itimerspec its;
  int r;

  /* fires periodically */
  memset(&its, 0, sizeof(its));
  timespec_add(&its.it_value, nsec);
  timespec_add(&its.it_interval, nsec);
  r = timer_settime(timerid, 0, &its, 0);
  RUNTIME_ASSERT(r == 0);
}
