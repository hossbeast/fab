/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*
 * man7 timer_create
 *
 * wrappers around linux userspace interval timer functionality. requires exclusive use of SIGUSR2.
 */

#include "types.h"

/* initializer timers system, establish signal handling for SIGUSR2 */
void timers_setup(void);

/* create a timer that will fire once and start it */
void timers_once_start(timer_t * restrict timerid, int32_t threadid, uint64_t nsec, int token, void (*handler)(int token))
  __attribute__((nonnull(1)));

/* start a previoulsy created once-type timer */
void timers_once_put(timer_t * restrict timerid, uint64_t nsec)
  __attribute__((nonnull));

/* create a timer that will fire periodically, dont start it */
void timers_periodic_create(timer_t * restrict timerid, int32_t threadid, int token, void (*handler)(int token, int overruns))
  __attribute__((nonnull(1)));

/* start a periodic-type timer */
void timers_periodic_put(timer_t timerid, uint64_t nsec)
  __attribute__((nonnull));
