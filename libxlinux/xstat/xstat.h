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
#include <fcntl.h>

#include "xapi.h"

/// xfstatat
//
// SUMMARY
//  proxy for fstatat
//
// VARIANTS
//  s/f/vf - different ways to pass path
//
xapi xfstatats(int dirfd, int flags, struct stat * restrict buf, const char * restrict path)
  __attribute__((nonnull(3, 4)));

xapi xfstatatf(int dirfd, int flags, struct stat * restrict buf, const char * restrict path_fmt, ...)
  __attribute__((nonnull(3, 4)))
  __attribute__((format(printf, 4, 5)));

xapi xfstatatvf(int dirfd, int flags, struct stat * restrict buf, const char * restrict path_fmt, va_list va)
  __attribute__((nonnull(3, 4)));

/// uxfstatat
//
// SUMMARY
//  proxy for fstatat which only fails when errno != ENOENT and errno != ENOTDIR
//  if fstatat fails but uxfstatat does not, buf is zeroed
//
// VARIANTS
//  s/f/vf - different ways to pass path
//
xapi uxfstatats(int * restrict r, int dirfd, int flags, struct stat * restrict buf, const char * restrict path)
  __attribute__((nonnull(4, 5)));

xapi uxfstatatf(int * restrict r, int dirfd, int flags, struct stat * restrict buf, const char * restrict path_fmt, ...)
  __attribute__((nonnull(4, 5)))
  __attribute__((format(printf, 5, 6)));

xapi uxfstatatvf(int * restrict r, int dirfd, int flags, struct stat * restrict buf, const char * restrict path_fmt, va_list va)
  __attribute__((nonnull(4, 5)));


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

#endif
