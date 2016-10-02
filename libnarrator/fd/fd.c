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

#include <stdio.h>

#include "xapi.h"
#include "xlinux/xstdio.h"
#include "xlinux/xunistd.h"
#include "xlinux/xstdlib.h"

#include "internal.h"
#include "fd/fd.internal.h"

#define restrict __restrict

APIDATA narrator * g_narrator_stdout;
APIDATA narrator * g_narrator_stderr;

//
// public
//

xapi fd_setup()
{
  enter;

  fatal(narrator_fd_create, &g_narrator_stdout, 1);
  fatal(narrator_fd_create, &g_narrator_stderr, 2);

  finally : coda;
}

xapi fd_cleanup()
{
  enter;

  fatal(narrator_xfree, g_narrator_stdout);
  fatal(narrator_xfree, g_narrator_stderr);

  finally : coda;
}

xapi fd_vsayf(narrator_fd * const restrict n, const char * const restrict fmt, va_list va)
{
  xproxy(xvdprintf, n->fd, fmt, va);
}

xapi fd_sayw(narrator_fd * const restrict n, const char * const restrict b, size_t l)
{
  xproxy(axwrite, n->fd, b, l);
}

xapi fd_seek(narrator_fd * const restrict n, off_t offset, int whence, off_t * restrict res)
{
  xproxy(xlseek, n->fd, offset, whence, res);
}

void fd_destroy(narrator_fd * const restrict n)
{
}

//
// api
//

API xapi narrator_fd_create(narrator ** const restrict n, int fd)
{
  enter;

  fatal(xmalloc, n, sizeof(**n));
  (*n)->type = NARRATOR_FD;
  (*n)->fd.fd = fd;

  finally : coda;
}

API int narrator_fd_fd(narrator * const restrict n)
{
  return n->fd.fd;
}
