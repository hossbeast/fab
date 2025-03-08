/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "xlinux/xstdio.h"
#include "xlinux/xunistd.h"
#include "xlinux/xstdlib.h"

#include "narrator.h"
#include "fd.internal.h"
#include "vtable.h"

#include "macros.h"

narrator * g_narrator_stdout;
narrator * g_narrator_stderr;

//
// static
//

static int __attribute__((nonnull)) fd_sayvf(narrator * const restrict N, const char * const restrict fmt, va_list va)
{
  int r;
  narrator_fd *n = containerof(N, typeof(*n), base);

  r = vdprintf(n->fd, fmt, va);
  if (r < 0) {
    n->err = r;
  }

  return r;
}

static int __attribute__((nonnull)) fd_sayw(narrator * const restrict N, const void * const restrict b, size_t l)
{
  narrator_fd *n = containerof(N, typeof(*n), base);
  size_t actual;
  ssize_t cur;

  actual = 0;
  while(l - actual)
  {
    cur = write(n->fd, b + actual, l - actual);
    if(cur == -1) {
      n->err = errno;
      break;
    }
    if(cur == 0) {
      break;
    }
    actual += cur;
  }

  return actual;
}

static void __attribute__((nonnull)) fd_seek(narrator * const restrict N, off_t offset, int whence, off_t * restrict res)
{
  narrator_fd *n = containerof(N, typeof(*n), base);

  *res = xlseek(n->fd, offset, whence);
}

static void __attribute__((nonnull(1, 2))) fd_read(narrator * restrict N, void * dst, size_t count, size_t * restrict r)
{
  narrator_fd *n = containerof(N, typeof(*n), base);

  axread(n->fd,dst, count);
  if(r) {
    *r = count;
  }
}

static void __attribute__((nonnull)) fd_flush(narrator * const restrict N)
{

}

struct narrator_vtable narrator_fd_vtable = NARRATOR_VTABLE(fd);

//
// public
//

void fd_setup()
{
  narrator_fd *fd;

  narrator_fd_create(&fd, 1);
  g_narrator_stdout = &fd->base;

  narrator_fd_create(&fd, 2);
  g_narrator_stderr = &fd->base;
}

void fd_cleanup()
{
  narrator_fd_free(containerof(g_narrator_stdout, narrator_fd, base));
  narrator_fd_free(containerof(g_narrator_stderr, narrator_fd, base));
}

//
// api
//

void narrator_fd_init(narrator_fd * restrict n, int fd)
{
  n->base.vtab = &narrator_fd_vtable;
  n->fd = fd;
  n->err = 0;
  n->base.off = 0;
}

void narrator_fd_destroy(narrator_fd * restrict n)
{

}

void narrator_fd_create(narrator_fd ** const restrict n, int fd)
{
  narrator_fd *nfd;

  xmalloc(&nfd, sizeof(*nfd));
  narrator_fd_init(nfd, fd);

  *n = nfd;
}

void narrator_fd_free(narrator_fd * const restrict n)
{
  free(n);
}
