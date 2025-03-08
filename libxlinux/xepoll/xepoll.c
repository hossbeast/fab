/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "xepoll.h"
#include "macros.h"

//
// api
//

int xepoll_create()
{
  int fd;

  fd = epoll_create(0x42);
  RUNTIME_ASSERT(fd >= 0);

  return fd;
}

void xepoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
{
  RUNTIME_ASSERT(epoll_ctl(epfd, op, fd, event) == 0);
}

int xepoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
{
  int r;

  r = epoll_wait(epfd, events, maxevents, timeout);
  RUNTIME_ASSERT(r != -1);

  return r;
}

int uxepoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
{
  int r;

  r = epoll_wait(epfd, events, maxevents, timeout);
  RUNTIME_ASSERT(r != -1 || errno == EINTR);

  return r;
}

int xepoll_pwait(int epfd, struct epoll_event *events, int maxevents, int timeout, const sigset_t *sigmask)
{
  int r;

  r = epoll_pwait(epfd, events, maxevents, timeout, sigmask);
  RUNTIME_ASSERT(r != -1);

  return r;
}

int uxepoll_pwait(int epfd, struct epoll_event *events, int maxevents, int timeout, const sigset_t *sigmask)
{
  int r;

  r = epoll_pwait(epfd, events, maxevents, timeout, sigmask);
  RUNTIME_ASSERT(r != -1 || errno == EINTR);

  return r;
}
