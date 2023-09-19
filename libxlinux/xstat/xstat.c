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

#include <string.h>
#include <errno.h>

#include "types.h"
#include "macros.h"

#include "xstat/xstat.h"
#include "common/fmt.h"

void API xfstatats(int dirfd, int flags, struct stat * restrict buf, const char * restrict path)
{
  RUNTIME_ASSERT(fstatat(dirfd, path, buf, flags) == 0);
}

void API xfstatatf(int dirfd, int flags, struct stat * restrict buf, const char * restrict path_fmt, ...)
{
  va_list va;
  va_start(va, path_fmt);

  xfstatatvf(dirfd, flags, buf, path_fmt, va);
  va_end(va);
}

void API xfstatatvf(int dirfd, int flags, struct stat * restrict buf, const char * restrict path_fmt, va_list va)
{
  char path[512];

  fmt_apply(path, sizeof(path), path_fmt, va);
  xfstatats(dirfd, flags, buf, path);
}

int API uxfstatats(int dirfd, int flags, struct stat * restrict buf, const char * restrict path)
{
  int r;

  r = fstatat(dirfd, path, buf, flags);
  if(r != 0)
  {
    RUNTIME_ASSERT(errno == ENOENT || errno == ENOTDIR);
    memset(buf, 0, sizeof(*buf));
  }

  return r;
}

int API uxfstatatf(int dirfd, int flags, struct stat * restrict buf, const char * restrict path_fmt, ...)
{
  int r;
  va_list va;

  va_start(va, path_fmt);
  r = uxfstatatvf(dirfd, flags, buf, path_fmt, va);
  va_end(va);

  return r;
}

int API uxfstatatvf(int dirfd, int flags, struct stat * restrict buf, const char * restrict path_fmt, va_list va)
{
  char path[512];

  fmt_apply(path, sizeof(path), path_fmt, va);
  return uxfstatats(dirfd, flags, buf, path);
}

void API xfutimens(int fd, const struct timespec times[2])
{
  RUNTIME_ASSERT(futimens(fd, times) == 0);
}

void API xutimensats(int dirfd, const struct timespec times[2], int flags, const char * const restrict path)
{
  RUNTIME_ASSERT(utimensat(dirfd, path, times, flags) == 0);
}

int API uxutimensats(int dirfd, const struct timespec times[2], int flags, const char * const restrict path)
{
  int r;

  r = utimensat(dirfd, path, times, flags);
  if(r != 0)
  {
    RUNTIME_ASSERT(errno == ENOENT);
  }

  return r;
}

void API xmkdirs(mode_t mode, const char * path)
{
  RUNTIME_ASSERT(mkdir(path, mode) == 0);
}

void API xmkdirf(mode_t mode, const char * path_fmt, ...)
{
  va_list va;
  va_start(va, path_fmt);

  xmkdirvf(mode, path_fmt, va);

  va_end(va);
}

void API xmkdirvf(mode_t mode, const char * path_fmt, va_list va)
{
  char path[512];

  fmt_apply(path, sizeof(path), path_fmt, va);
  xmkdirs(mode, path);
}

int API uxmkdirs(mode_t mode, const char * path)
{
  int r;

  r = mkdir(path, mode);
  RUNTIME_ASSERT(r == 0 || errno == EEXIST);

  return r;
}

int API uxmkdirf(mode_t mode, const char * path_fmt, ...)
{
  int r;
  va_list va;

  va_start(va, path_fmt);
  r = uxmkdirvf(mode, path_fmt, va);
  va_end(va);

  return r;
}

int API uxmkdirvf(mode_t mode, const char * path_fmt, va_list va)
{
  char path[512];

  fmt_apply(path, sizeof(path), path_fmt, va);
  return uxmkdirs(mode, path);
}

void API xfchmod(int fd, mode_t mode)
{
  RUNTIME_ASSERT(fchmod(fd, mode) == 0);
}
