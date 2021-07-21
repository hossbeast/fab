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

#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "types.h"
#include "macros.h"

#include "xstdlib/xstdlib.h"

#include "common/fmt.h"

//
// api
//

void API xmalloc(void* target, size_t size)
{
  RUNTIME_ASSERT(posix_memalign(target, sizeof(void*) * 2, size) == 0);
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
    RUNTIME_ASSERT((mem = realloc(*t, es * ec)) != 0);
    *t = mem;

    if(((ssize_t)ec - (ssize_t)oec) > 0)
    {
      memset(((char*)*t) + (oec * es), 0, ((ssize_t)ec - (ssize_t)oec) * es);
    }
  }
}

void API iwfree(void* target)
{
  void** t = (void**)target;

  wfree(*t);
  *t = 0;
}

void API wfree(void* target)
{
  free(target);
}

ssize_t API xreadlinks(const char * pathname, char * buf, size_t bufsiz)
{
  ssize_t r;

  r = readlink(pathname, buf, bufsiz);
  RUNTIME_ASSERT(r != -1);

  return r;
}

ssize_t API xreadlinkf(const char * pathname_fmt, char * buf, size_t bufsiz, ...)
{
  ssize_t r;
  va_list va;

  va_start(va, bufsiz);
  r = xreadlinkvf(pathname_fmt, buf, bufsiz, va);
  va_end(va);

  return r;
}

ssize_t API xreadlinkvf(const char * pathname_fmt, char * buf, size_t bufsiz, va_list va)
{
  char pathname[512];

  fmt_apply(pathname, sizeof(pathname), pathname_fmt, va);
  return xreadlinks(pathname, buf, bufsiz);
}

char * API xrealpaths(char * restrict resolved_path, const char * restrict path)
{
  char *r;

  r = realpath(path, resolved_path);
  RUNTIME_ASSERT(r != 0);

  return r;
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
  char pathname[512];

  fmt_apply(pathname, sizeof(pathname), path_fmt, va);
  return xrealpaths(resolved_path, pathname);
}

int API xsystem(const char * restrict command)
{
  int r;

  r = system(command);

  RUNTIME_ASSERT(r != -1);

  return r;
}
