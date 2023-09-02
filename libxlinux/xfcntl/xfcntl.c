/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "types.h"
#include "macros.h"

#include "xfcntl.h"
#include "fmt.h"

int API xopens(int flags, const char * path)
{
  return xopenats(flags, AT_FDCWD, path);
}

int API xopenf(int flags, const char * path_fmt, ...)
{
  int fd;
  va_list va;

  va_start(va, path_fmt);
  fd = xopenatvf(flags, AT_FDCWD, path_fmt, va);
  va_end(va);

  return fd;
}

int API xopenvf(int flags, const char * path_fmt, va_list va)
{
  return xopenatvf(flags, AT_FDCWD, path_fmt, va);
}

int API xopenats(int flags, int dirfd, const char * path)
{
  int fd;

  if((fd = openat(dirfd, path, flags)) == -1)
  {
    RUNTIME_ABORT();
  }

  return fd;
}

int API xopenatf(int flags, int dirfd, const char * path_fmt, ...)
{
  int fd;
  va_list va;
  va_start(va, path_fmt);

  fd = xopenatvf(flags, dirfd, path_fmt, va);
  va_end(va);

  return fd;
}

int API xopenatvf(int flags, int dirfd, const char * path_fmt, va_list va)
{
  char path[512];
  fmt_apply(path, sizeof(path), path_fmt, va);
  return xopenats(flags, dirfd, path);
}

int API uxopens(int flags, const char * path)
{
  return uxopenats(flags, AT_FDCWD, path);
}

int API uxopenf(int flags, const char * path_fmt, ...)
{
  int fd;
  va_list va;

  va_start(va, path_fmt);
  fd = uxopenatvf(flags, AT_FDCWD, path_fmt, va);
  va_end(va);

  return fd;
}

int API uxopenvf(int flags, const char * path_fmt, va_list va)
{
  return uxopenatvf(flags, AT_FDCWD, path_fmt, va);
}

int API uxopenats(int flags, int dirfd, const char * path)
{
  int fd;

  if((fd = openat(dirfd, path, flags)) == -1)
  {
    if(errno != ENOENT && errno != EEXIST && errno != ENOTDIR)
    {
      RUNTIME_ABORT();
    }
  }

  return fd;
}

int API uxopenatf(int flags, int dirfd, const char * path_fmt, ...)
{
  int fd;
  va_list va;

  va_start(va, path_fmt);
  fd = uxopenatvf(flags, dirfd, path_fmt, va);
  va_end(va);

  return fd;
}

int API uxopenatvf(int flags, int dirfd, const char * path_fmt, va_list va)
{
  int fd;
  char path[512];

  fmt_apply(path, sizeof(path), path_fmt, va);
  fd = uxopenats(flags, dirfd, path);
  va_end(va);

  return fd;
}

int API xopen_modes(int flags, mode_t mode, const char * path)
{
  return xopenat_modes(flags, mode, AT_FDCWD, path);
}

int API xopen_modef(int flags, mode_t mode, const char * path_fmt, ...)
{
  int fd;
  va_list va;

  va_start(va, path_fmt);
  fd = xopenat_modevf(flags, mode, AT_FDCWD, path_fmt, va);
  va_end(va);

  return fd;
}

int API xopen_modevf(int flags, mode_t mode, const char * path_fmt, va_list va)
{
  return xopenat_modevf(flags, mode, AT_FDCWD, path_fmt, va);
}

int API xopenat_modes(int flags, mode_t mode, int dirfd, const char * path)
{
  int fd;

  fd = openat(dirfd, path, flags, mode);
  RUNTIME_ASSERT(fd != -1);

  return fd;
}

int API xopenat_modef(int flags, mode_t mode, int dirfd, const char * path_fmt, ...)
{
  int fd;
  va_list va;

  va_start(va, path_fmt);
  fd = xopenat_modevf(flags, mode, dirfd, path_fmt, va);
  va_end(va);

  return fd;
}

int API xopenat_modevf(int flags, mode_t mode, int dirfd, const char * path_fmt, va_list va)
{
  int fd;
  char path[512];

  fmt_apply(path, sizeof(path), path_fmt, va);
  fd = xopenat_modes(flags, mode, dirfd, path);
  va_end(va);

  return fd;
}

int API uxopen_modes(int flags, mode_t mode, const char * path)
{
  return uxopenat_modes(flags, mode, AT_FDCWD, path);
}

int API uxopen_modef(int flags, mode_t mode, const char * path_fmt, ...)
{
  int fd;
  va_list va;

  va_start(va, path_fmt);
  fd = uxopenat_modevf(flags, mode, AT_FDCWD, path_fmt, va);
  va_end(va);

  return fd;
}

int API uxopen_modevf(int flags, mode_t mode, const char * path_fmt, va_list va)
{
  return uxopenat_modevf(flags, mode, AT_FDCWD, path_fmt, va);
}

int API uxopenat_modes(int flags, mode_t mode, int dirfd, const char * path)
{
  int fd;

  if((fd = openat(dirfd, path, flags, mode)) == -1)
  {
    if(errno != ENOENT && errno != EEXIST)
    {
      RUNTIME_ABORT();
    }
  }

  return fd;
}

int API uxopenat_modef(int flags, mode_t mode, int dirfd, const char * path_fmt, ...)
{
  int fd;
  va_list va;

  va_start(va, path_fmt);
  fd = uxopenat_modevf(flags, mode, dirfd, path_fmt, va);
  va_end(va);

  return fd;
}

int API uxopenat_modevf(int flags, mode_t mode, int dirfd, const char * path_fmt, va_list va)
{
  int fd;
  char path[512];

  fmt_apply(path, sizeof(path), path_fmt, va);
  fd = uxopenat_modes(flags, mode, dirfd, path);
  va_end(va);

  return fd;
}

ssize_t API xsplice(int fd_in, loff_t *off_in, int fd_out, loff_t *off_out, size_t len, unsigned int flags)
{
  ssize_t rv;

  rv = splice(fd_in, off_in, fd_out, off_out, len, flags);
  RUNTIME_ASSERT(rv != -1);

  return rv;
}
