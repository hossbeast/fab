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

#include <errno.h>

#include "xepoll.h"
#include "macros.h"

//
// api
//

int API xepoll_create()
{
  int fd;

  fd = epoll_create(0x42);
  RUNTIME_ASSERT(fd >= 0);

  return fd;
}

void API xepoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
{
  RUNTIME_ASSERT(epoll_ctl(epfd, op, fd, event) == 0);
}

void API xepoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout)
{
  RUNTIME_ASSERT(epoll_wait(epfd, events, maxevents, timeout) != -1);
}

void API xepoll_pwait(int epfd, struct epoll_event *events, int maxevents, int timeout, const sigset_t *sigmask)
{
  RUNTIME_ASSERT(epoll_pwait(epfd, events, maxevents, timeout, sigmask) != -1);
}

int API uxepoll_pwait(int epfd, struct epoll_event *events, int maxevents, int timeout, const sigset_t *sigmask)
{
  int r;

  r = epoll_pwait(epfd, events, maxevents, timeout, sigmask);
  RUNTIME_ASSERT(r == 0 || errno == EINTR);

  return r;
}
