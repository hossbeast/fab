/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>
#include <string.h>

#include "xstat.h"
#include "fmt.h"

void xfstatats(int dirfd, int flags, struct stat * restrict buf, const char * restrict path)
{
  RUNTIME_ASSERT(fstatat(dirfd, path, buf, flags) == 0);
}

void xfstatatf(int dirfd, int flags, struct stat * restrict buf, const char * restrict path_fmt, ...)
{
  va_list va;
  va_start(va, path_fmt);

  xfstatatvf(dirfd, flags, buf, path_fmt, va);
  va_end(va);
}

void xfstatatvf(int dirfd, int flags, struct stat * restrict buf, const char * restrict path_fmt, va_list va)
{
  char path[PATH_MAX];

  fmt_apply(path, sizeof(path), path_fmt, va);
  xfstatats(dirfd, flags, buf, path);
}

int uxfstatats(int dirfd, int flags, struct stat * restrict buf, const char * restrict path)
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

int uxfstatatf(int dirfd, int flags, struct stat * restrict buf, const char * restrict path_fmt, ...)
{
  int r;
  va_list va;

  va_start(va, path_fmt);
  r = uxfstatatvf(dirfd, flags, buf, path_fmt, va);
  va_end(va);

  return r;
}

int uxfstatatvf(int dirfd, int flags, struct stat * restrict buf, const char * restrict path_fmt, va_list va)
{
  char path[PATH_MAX];

  fmt_apply(path, sizeof(path), path_fmt, va);
  return uxfstatats(dirfd, flags, buf, path);
}

void xfutimens(int fd, const struct timespec times[2])
{
  RUNTIME_ASSERT(futimens(fd, times) == 0);
}

void xutimensats(int dirfd, const struct timespec times[2], int flags, const char * const restrict path)
{
  RUNTIME_ASSERT(utimensat(dirfd, path, times, flags) == 0);
}

int uxutimensats(int dirfd, const struct timespec times[2], int flags, const char * const restrict path)
{
  int r;

  r = utimensat(dirfd, path, times, flags);
  if(r != 0)
  {
    RUNTIME_ASSERT(errno == ENOENT);
  }

  return r;
}

void xmkdirs(mode_t mode, const char * path)
{
  RUNTIME_ASSERT(mkdir(path, mode) == 0);
}

void xmkdirf(mode_t mode, const char * path_fmt, ...)
{
  va_list va;
  va_start(va, path_fmt);

  xmkdirvf(mode, path_fmt, va);

  va_end(va);
}

void xmkdirvf(mode_t mode, const char * path_fmt, va_list va)
{
  char path[PATH_MAX];

  fmt_apply(path, sizeof(path), path_fmt, va);
  xmkdirs(mode, path);
}

int uxmkdirs(mode_t mode, const char * path)
{
  int r;

  r = mkdir(path, mode);
  RUNTIME_ASSERT(r == 0 || errno == EEXIST);

  return r;
}

int uxmkdirf(mode_t mode, const char * path_fmt, ...)
{
  int r;
  va_list va;

  va_start(va, path_fmt);
  r = uxmkdirvf(mode, path_fmt, va);
  va_end(va);

  return r;
}

int uxmkdirvf(mode_t mode, const char * path_fmt, va_list va)
{
  char path[PATH_MAX];

  fmt_apply(path, sizeof(path), path_fmt, va);
  return uxmkdirs(mode, path);
}

void xfchmod(int fd, mode_t mode)
{
  RUNTIME_ASSERT(fchmod(fd, mode) == 0);
}
