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

#include "xapi.h"
#include "xlinux/xstdio.h"
#include "xlinux/xunistd.h"
#include "xlinux/xstdlib.h"

#include "narrator.h"
#include "fd.internal.h"
#include "vtable.h"

#include "macros.h"

narrator * APIDATA g_narrator_stdout;
narrator * APIDATA g_narrator_stderr;

//
// static
//

static xapi __attribute__((nonnull)) fd_sayvf(narrator * const restrict N, const char * const restrict fmt, va_list va)
{
  enter;

  narrator_fd *n = containerof(N, typeof(*n), base);

  fatal(xvdprintf, n->fd, fmt, va);

  finally : coda;
}

static xapi __attribute__((nonnull)) fd_sayw(narrator * const restrict N, const void * const restrict b, size_t l)
{
  enter;

  narrator_fd *n = containerof(N, typeof(*n), base);

  fatal(axwrite, n->fd, b, l);

  finally : coda;
}

static xapi __attribute__((nonnull)) fd_seek(narrator * const restrict N, off_t offset, int whence, off_t * restrict res)
{
  enter;

  narrator_fd *n = containerof(N, typeof(*n), base);

  fatal(xlseek, n->fd, offset, whence, res);

  finally : coda;
}

static xapi __attribute__((nonnull(1, 2))) fd_read(narrator * restrict N, void * dst, size_t count, size_t * restrict r)
{
  enter;

  narrator_fd *n = containerof(N, typeof(*n), base);

  fatal(axread, n->fd,dst, count);
  if(r) {
    *r = count;
  }

  finally : coda;
}

static xapi __attribute__((nonnull)) fd_flush(narrator * const restrict N)
{
  enter;

  finally : coda;
}

static xapi __attribute__((nonnull)) fd_destroy(narrator * const restrict N)
{
  enter;

  finally : coda;
}

struct narrator_vtable API narrator_fd_vtable = NARRATOR_VTABLE(fd);

//
// public
//

xapi fd_setup()
{
  enter;

  narrator_fd *fd;

  fatal(narrator_fd_create, &fd, 1);
  g_narrator_stdout = &fd->base;

  fatal(narrator_fd_create, &fd, 2);
  g_narrator_stderr = &fd->base;

  finally : coda;
}

xapi fd_cleanup()
{
  enter;

  fatal(narrator_fd_free, containerof(g_narrator_stdout, narrator_fd, base));
  fatal(narrator_fd_free, containerof(g_narrator_stderr, narrator_fd, base));

  finally : coda;
}

//
// api
//

xapi API narrator_fd_create(narrator_fd ** const restrict n, int fd)
{
  enter;

  narrator_fd *nfd;

  fatal(xmalloc, &nfd, sizeof(*nfd));

  nfd->base.vtab = &narrator_fd_vtable;
  nfd->fd = fd;
  *n = nfd;

  finally : coda;
}

xapi API narrator_fd_free(narrator_fd * const restrict n)
{
  enter;

  fatal(fd_destroy, &n->base);
  wfree(n);

  finally : coda;
}
