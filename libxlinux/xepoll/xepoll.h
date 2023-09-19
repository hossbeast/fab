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

#ifndef _XEPOLL_H
#define _XEPOLL_H

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

#endif
