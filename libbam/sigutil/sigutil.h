/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <signal.h>

#include "types.h"

extern bool g_sigterm;

void sigutil_install_handlers(void);

/* sigutil_wait
 *
 * block until receipt of one of a set of signals
 *
 * sigs   - set of signals
 * [info] - returns information about the signal received
 */
void sigutil_wait(const sigset_t * restrict sigs, siginfo_t * restrict info)
  __attribute__((nonnull(1)));

/* sigutil_timedwait
 *
 * block until receipt of one of a set of signals
 *
 * sigs  - set of signals
 * info  - returns information about the signal received
 * timeout - max time to wait
 *
 * returns
 *  zero on sucess (one of the signals in sigs caught), errno otherwise (one of EINTR, EAGAIN)
 */
int sigutil_timedwait(const sigset_t * restrict sigs, siginfo_t * restrict info, const struct timespec * restrict timeout)
  __attribute__((nonnull));

/// sigutil_exchange
//
// SUMMARY
//  send a signal, then block until receipt of one of a set of signals
//
// PARAMETERS
//  sig    - signal to send
//  pid    - process to kill
//  sigs   - set of signals
//  [info] - returns information about the signal received
//
void sigutil_exchange(int sig, pid_t pid, const sigset_t * restrict sigs, siginfo_t * restrict info)
  __attribute__((nonnull(3)));

/// sigutil_kill
//
// SUMMARY
//  call xkill and log under L_IPC
//
void sigutil_kill(pid_t pid, int sig);
int sigutil_uxkill(pid_t pid, int sig);
void sigutil_tgkill(pid_t pid, pid_t tid, int sig);

int sigutil_uxtgkill(pid_t pid, pid_t tid, int signo)
  __attribute__((nonnull));

int sigutil_uxrt_sigqueueinfo(pid_t pid, int signo, union sigval value)
  __attribute__((nonnull));

int sigutil_uxrt_tgsigqueueinfo(pid_t pid, pid_t tid, int signo, union sigval value)
  __attribute__((nonnull));
