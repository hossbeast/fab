/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/syscall.h>
#include <fcntl.h>

#include "types.h"
#include "macros.h"

#include "xstdlib.h"

#include "fmt.h"
#include "zcat.h"

//
// api
//

void API xmalloc(void* target, size_t size)
{
  int r;

  r = posix_memalign(target, sizeof(void*) * 2, size);
  RUNTIME_ASSERT(r == 0);

  memset(*(void**)target, 0, size);
}

void API xrealloc(void* target, size_t es, size_t ec, size_t oec)
{
  void** t;
  void * mem = 0;

  t = ((void**)target);
  if(!es || !ec)
  {
    *t = 0;
  }
  else
  {
    mem = realloc(*t, es * ec);

    RUNTIME_ASSERT(mem != 0);
    *t = mem;

    if(((ssize_t)ec - (ssize_t)oec) > 0)
    {
      memset(((char*)*t) + (oec * es), 0, ((ssize_t)ec - (ssize_t)oec) * es);
    }
  }
}

void API ifree(void* target)
{
  void** t = (void**)target;

  free(*t);
  *t = 0;
}

/* readlinkat */

ssize_t uxreadlinkatw(int dfd, const char * pathname, uint16_t pathname_len, char * buf, size_t bufsiz)
{
  char pathbuf[PATH_MAX];
  int len;

  len = zcatw(pathbuf, sizeof(pathbuf), pathname, pathname_len);
  pathbuf[len] = 0;

  return readlinkat(dfd, pathbuf, buf, bufsiz);
}

ssize_t uxreadlinkatf(int dfd, const char * pathname_fmt, char * buf, size_t bufsiz, ...)
{
  ssize_t r;
  va_list va;

  va_start(va, bufsiz);
  r = uxreadlinkatvf(dfd, pathname_fmt, buf, bufsiz, va);
  va_end(va);

  return r;
}

ssize_t uxreadlinkatvf(int dfd, const char * pathname_fmt, char * buf, size_t bufsiz, va_list va)
{
  char pathname[PATH_MAX];

  fmt_apply(pathname, sizeof(pathname), pathname_fmt, va);
  return readlinkat(dfd, pathname, buf, bufsiz);
}

ssize_t xreadlinkats(int dfd, const char * pathname, char * buf, size_t bufsiz)
{
  ssize_t r;

  r = readlinkat(dfd, pathname, buf, bufsiz);
  RUNTIME_ASSERT(r != -1);

  return r;
}

ssize_t xreadlinkatw(int dfd, const char * pathname, uint16_t pathname_len, char * buf, size_t bufsiz)
{
  char pathbuf[PATH_MAX];
  int len;

  len = zcatw(pathbuf, sizeof(pathbuf), pathname, pathname_len);
  pathbuf[len] = 0;

  return xreadlinkats(dfd, pathbuf, buf, bufsiz);
}

ssize_t xreadlinkatf(int dfd, const char * pathname_fmt, char * buf, size_t bufsiz, ...)
{
  ssize_t r;
  va_list va;

  va_start(va, bufsiz);
  r = xreadlinkatvf(dfd, pathname_fmt, buf, bufsiz, va);
  va_end(va);

  return r;
}

ssize_t xreadlinkatvf(int dfd, const char * pathname_fmt, char * buf, size_t bufsiz, va_list va)
{
  char pathname[PATH_MAX];

  fmt_apply(pathname, sizeof(pathname), pathname_fmt, va);
  return xreadlinkats(dfd, pathname, buf, bufsiz);
}

/* readlink */

ssize_t uxreadlinkw(const char * pathname, uint16_t pathname_len, char * buf, size_t bufsiz)
{
  return uxreadlinkatw(AT_FDCWD, pathname, pathname_len, buf, bufsiz);
}

ssize_t uxreadlinkf(const char * pathname_fmt, char * buf, size_t bufsiz, ...)
{
  va_list va;
  ssize_t r;

  va_start(va, bufsiz);
  r = uxreadlinkatvf(AT_FDCWD, pathname_fmt, buf, bufsiz, va);
  va_end(va);

  return r;
}

ssize_t uxreadlinkvf(const char * pathname_fmt, char * buf, size_t bufsiz, va_list va)
{
  return uxreadlinkatvf(AT_FDCWD, pathname_fmt, buf, bufsiz, va);
}

ssize_t xreadlinks(const char * pathname, char * buf, size_t bufsiz)
{
  return xreadlinkats(AT_FDCWD, pathname, buf, bufsiz);
}

ssize_t xreadlinkw(const char * pathname, uint16_t pathname_len, char * buf, size_t bufsiz)
{
  return uxreadlinkatw(AT_FDCWD, pathname, pathname_len, buf, bufsiz);
}

ssize_t xreadlinkf(const char * pathname_fmt, char * buf, size_t bufsiz, ...)
{
  va_list va;
  ssize_t r;

  va_start(va, bufsiz);
  r = xreadlinkatvf(AT_FDCWD, pathname_fmt, buf, bufsiz, va);
  va_end(va);

  return r;
}

ssize_t xreadlinkvf(const char * pathname_fmt, char * buf, size_t bufsiz, va_list va)
{
  return uxreadlinkatvf(AT_FDCWD, pathname_fmt, buf, bufsiz, va);
}

char * API xrealpaths(char * restrict resolved_path, const char * restrict path)
{
  char *r;

  r = realpath(path, resolved_path);
  RUNTIME_ASSERT(r != 0);

  return r;
}

char * API xrealpathw(char * restrict resolved_path, const char * restrict path, uint16_t path_len)
{
  char pathname[PATH_MAX];
  size_t z;

  z = zcatw(pathname, sizeof(pathname), path, path_len);
  pathname[z] = 0;

  return xrealpaths(resolved_path, pathname);
}

char * API xrealpathf(char * restrict resolved_path, const char * restrict path_fmt, ...)
{
  char * r;
  va_list va;

  va_start(va, path_fmt);
  r = xrealpathvf(resolved_path, path_fmt, va);
  va_end(va);

  return r;
}

char * API xrealpathvf(char * restrict resolved_path, const char * restrict path_fmt, va_list va)
{
  char pathname[PATH_MAX];

  fmt_apply(pathname, sizeof(pathname), path_fmt, va);
  return xrealpaths(resolved_path, pathname);
}
