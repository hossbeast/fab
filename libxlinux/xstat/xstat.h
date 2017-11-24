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

#ifndef _XSTAT_H
#define _XSTAT_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdarg.h>

#include "xapi.h"

#define restrict __restrict

/// xstat
//
// SUMMARY
//  proxy for stat
//
// VARIANTS
//  s/f/vf - different ways to pass path
//
xapi xstats(struct stat * restrict buf, const char * restrict path)
  __attribute__((nonnull(1, 2)));

xapi xstatf(struct stat * restrict buf, const char * restrict path_fmt, ...)
  __attribute__((nonnull(1, 2)));

xapi xstatvf(struct stat * restrict buf, const char * restrict path_fmt, va_list va)
  __attribute__((nonnull(1, 2)));

/// uxstat
//
// SUMMARY
//  proxy for stat which only fails when errno != ENOENT and errno != ENOTDIR
//  if stat fails but uxstat does not, buf is zeroed
//
// VARIANTS
//  s/f/vf - different ways to pass path
//
xapi uxstats(int * restrict r, struct stat * restrict buf, const char * restrict path)
  __attribute__((nonnull(2,3)));

xapi uxstatf(int * restrict r, struct stat * restrict buf, const char * restrict path_fmt, ...)
  __attribute__((nonnull(2,3)));

xapi uxstatvf(int * restrict r, struct stat * restrict buf, const char * restrict path_fmt, va_list va)
  __attribute__((nonnull(2,3)));

/// xlstat
//
// SUMMARY
//  proxy for lstat
//
// VARIANTS
//  s/f/vf - different ways to pass path
//
xapi xlstats(int * restrict r, struct stat * restrict buf, const char * restrict path)
  __attribute__((nonnull(2,3)));

xapi xlstatf(int * restrict r, struct stat * restrict buf, const char * restrict path_fmt, ...)
  __attribute__((nonnull(2,3)));

xapi xlstatvf(int * restrict r, struct stat * restrict buf, const char * restrict path_fmt, va_list va)
  __attribute__((nonnull(2,3)));

/// uxlstat
//
// SUMMARY
//  proxy for lstat which only fails when errno != ENOENT and errno != ENOTDIR
//  if lstat fails but uxlstat does not, buf is zeroed
//
// VARIANTS
//  s/f/vf - different ways to pass path
//
xapi uxlstats(int * restrict r, struct stat * restrict buf, const char * restrict path)
  __attribute__((nonnull(2,3)));

xapi uxlstatf(int * restrict r, struct stat * restrict buf, const char * restrict path_fmt, ...)
  __attribute__((nonnull(2,3)));

xapi uxlstatvf(int * restrict r, struct stat * restrict buf, const char * restrict path_fmt, va_list va)
  __attribute__((nonnull(2,3)));

/// xfstat
//
// SUMMARY
//  proxy for fstat
//
xapi xfstat(int fd, struct stat * restrict buf)
  __attribute__((nonnull));

/// uxfstat
//
// SUMMARY
//  proxy for stat which only fails when errno != ENOENT and errno != ENOTDIR
//  if stat fails and errno == ENOENT, buf is zeroed
//
xapi uxfstat(int fd, struct stat * restrict buf)
  __attribute__((nonnull));

/// xfutimens
//
// SUMMARY
//  proxy for futimens
//
xapi xfutimens(int fd, const struct timespec times[2]);

/// xutimensat
//
// SUMMARY
//  proxy for utimensat
//
xapi xutimensats(int dirfd, const struct timespec times[2], int flags, const char * const restrict path)
  __attribute__((nonnull));

/// xutimensat
//
// SUMMARY
//  proxy for utimensat that only fails when errno != ENOENT
//
xapi uxutimensats(int dirfd, const struct timespec times[2], int flags, int * restrict r, const char * const restrict path)
  __attribute__((nonnull(5)));

/// xmkdir
//
// SUMMARY
//  proxy for mkdir
//
xapi xmkdirs(mode_t mode, const char * path)
  __attribute__((nonnull));

xapi xmkdirf(mode_t mode, const char * path_fmt, ...)
  __attribute__((nonnull));

xapi xmkdirvf(mode_t mode, const char * path_fmt, va_list va)
  __attribute__((nonnull));

/// uxmkdir
//
// SUMMARY
//  proxy for mkdir which only fails when errno != ENOENT
//
xapi uxmkdirs(mode_t mode, const char * path)
  __attribute__((nonnull));

xapi uxmkdirf(mode_t mode, const char * path_fmt, ...)
  __attribute__((nonnull));

xapi uxmkdirvf(mode_t mode, const char * path_fmt, va_list va)
  __attribute__((nonnull));

/// xfchmod
//
// SUMMARY
//  proxy for fchmod
//
xapi xfchmod(int fd, mode_t mode);

#undef restrict
#endif
