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

#include "internal.h"
#include "xepoll.h"

#include "KERNEL.errtab.h"

//
// api
//

API xapi xepoll_create(int * restrict fd)
{
  enter;

  int r;
  if((r = epoll_create(0x42)) < 0)
  {
    tfail(perrtab_KERNEL, errno);
  }

  *fd = r;

  finally : coda;
}

API xapi xepoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
{
  enter;

  tfatalize(perrtab_KERNEL, errno, epoll_ctl, epfd, op, fd, event);

  finally : coda;
}

API xapi xepoll_wait(int * restrict r, int epfd, struct epoll_event *events, int maxevents, int timeout)
{
  enter;

  if(((*r) = epoll_wait(epfd, events, maxevents, timeout)) == -1)
  {
    tfail(perrtab_KERNEL, errno);
  }

  finally : coda;
}

API xapi xepoll_pwait(int * restrict r, int epfd, struct epoll_event *events, int maxevents, int timeout, const sigset_t *sigmask)
{
  enter;

  if(((*r) = epoll_pwait(epfd, events, maxevents, timeout, sigmask)) == -1)
  {
    tfail(perrtab_KERNEL, errno);
  }

  finally : coda;
}

API xapi uxepoll_pwait(int * restrict r, int epfd, struct epoll_event *events, int maxevents, int timeout, const sigset_t *sigmask)
{
  enter;

  if(((*r) = epoll_pwait(epfd, events, maxevents, timeout, sigmask)) == -1 && errno != EINTR)
  {
    tfail(perrtab_KERNEL, errno);
  }

  finally : coda;
}
