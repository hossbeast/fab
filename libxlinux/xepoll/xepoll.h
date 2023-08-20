/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <sys/epoll.h>
#include <signal.h>

#include "types.h"

struct epoll_event;

/// xepoll_create
//
// SUMMARY
//  proxy for epoll_create
//
// PARAMETERS
//  fd   - (returns) newly created epoll fd
//
int xepoll_create(void);

/// xepoll_ctl
//
// SUMMARY
//  proxy for epoll_ctl
//
void xepoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
  __attribute__((nonnull));

/// xepoll_wait
//
// SUMMARY
//  proxy for epoll_wait
//
void xepoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
  __attribute__((nonnull));

/// xepoll_pwait
//
// SUMMARY
//  proxy for epoll_pwait
//
void xepoll_pwait(int epfd, struct epoll_event *events, int maxevents, int timeout, const sigset_t *sigmask)
  __attribute__((nonnull));

/// uxepoll_pwait
//
// SUMMARY
//  proxy for epoll_pwait that fails only when errno != EINTR
//
int uxepoll_pwait(int epfd, struct epoll_event *events, int maxevents, int timeout, const sigset_t *sigmask)
  __attribute__((nonnull));
