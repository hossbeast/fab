/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/types.h>
#include <signal.h>


/// xkill
//
// SUMMARY
//  proxy for kill
//
// PARAMETERS
//  pid - pid
//  sig - signal
//
void xkill(pid_t pid, int sig);

/// uxkill
//
// SUMMARY
//  proxy for kill that only fails when errno != ESRCH (no such pid)
//
// PARAMETERS
//  [r] - (returns) the return value from kill
//  pid - pid
//  sig - signal
//
int uxkill(pid_t pid, int sig);

/// xtgkill
//
// SUMMARY
//  proxy for tgkill
//
void xtgkill(int tgid, int tid, int sig);

/// xtgkill
//
// SUMMARY
//  proxy for tgkill that only fails when errno != ESRCH (no such pid/thread)
//
int uxtgkill(int tgid, int tid, int sig);

/// xsigaction
//
// SUMMARY
//  xapi proxy for sigaction
//
void xsigaction(int signum, const struct sigaction * act, struct sigaction * oldact)
  __attribute__((nonnull(2)));

/// xsigprocmask
//
// SUMMARY
//  xapi proxy for sigprocmask
//
void xsigprocmask(int how, const sigset_t * set, sigset_t * oldset);

/// uxsigsuspend
//
// SUMMARY
//  xapi proxy for sigsuspend that only fails when errno != EINTR
//  (it is kind of the whole point ...)
//
void uxsigsuspend(const sigset_t * mask)
  __attribute__((nonnull));

/// sigwaitinfo
//
// SUMMARY
//  xapi proxy for sigwaitinfo
//
void xsigwaitinfo(const sigset_t * mask, siginfo_t * info)
  __attribute__((nonnull));

/// uxsigwaitinfo
//
// SUMMARY
//  xapi proxy for sigwaitinfo that only fails when errno != EINTR
//
int uxsigwaitinfo(const sigset_t * mask, siginfo_t * info)
  __attribute__((nonnull));

/**
 * xapi wrapper for sigtimedwait that only fails when errno != { EINTR, EAGAIN }
 *
 * err - (returns) zero on success, errno otherwise
 */
int uxsigtimedwait(
    const sigset_t * restrict mask
  , siginfo_t * restrict info
  , const struct timespec * restrict timeout
)
  __attribute__((nonnull));

/// xsignal
//
// SUMMARY
//  xapi proxy for signal
//
void xsignal(int signum, sighandler_t handler);

int uxrt_sigqueueinfo(pid_t tgid, int sig, siginfo_t *info)
  __attribute__((nonnull));

int uxrt_tgsigqueueinfo(pid_t tgid, pid_t tid, int sig, siginfo_t *info)
  __attribute__((nonnull));

const char * signame(int signo);
