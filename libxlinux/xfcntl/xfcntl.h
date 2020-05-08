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

#ifndef _XFCNTL_H
#define _XFCNTL_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>

#include "xapi.h"

/// xopen
//
// SUMMARY
//  proxy for open
//
xapi xopens(int * fd, int flags, const char * path)
  __attribute__((nonnull(3)));

xapi xopenf(int * fd, int flags, const char * path_fmt, ...)
  __attribute__((nonnull(3)))
  __attribute__((format(printf, 3, 4)));

xapi xopenvf(int * fd, int flags, const char * path_fmt, va_list va)
  __attribute__((nonnull(3)));

/// xopenat
//
// SUMMARY
//  proxy for opent
//
xapi xopenats(int * fd, int flags, int dirfd, const char * path)
  __attribute__((nonnull(4)));

xapi xopenatf(int * fd, int flags, int dirfd, const char * path_fmt, ...)
  __attribute__((nonnull(4)))
  __attribute__((format(printf, 4, 5)));

xapi xopenatvf(int * fd, int flags, int dirfd, const char * path_fmt, va_list va)
  __attribute__((nonnull(4)));

/// xopen_mode
//
// SUMMARY
//  proxy for open
//
xapi xopen_modes(int * fd, int flags, mode_t mode, const char * path)
  __attribute__((nonnull(4)));

xapi xopen_modef(int * fd, int flags, mode_t mode, const char * path_fmt, ...)
  __attribute__((nonnull(4)))
  __attribute__((format(printf, 4, 5)));

xapi xopen_modevf(int * fd, int flags, mode_t mode, const char * path_fmt, va_list va)
  __attribute__((nonnull(4)));

/// xopenat_mode
//
// SUMMARY
//  proxy for open
//
xapi xopenat_modes(int * fd, int flags, mode_t mode, int dirfd, const char * path)
  __attribute__((nonnull(5)));

xapi xopenat_modef(int * fd, int flags, mode_t mode, int dirfd, const char * path_fmt, ...)
  __attribute__((nonnull(5)))
  __attribute__((format(printf, 5, 6)));


xapi xopenat_modevf(int * fd, int flags, mode_t mode, int dirfd, const char * path_fmt, va_list va)
  __attribute__((nonnull(5)));

/// uxopen
//
// SUMMARY
//  proxy for open that only fails when errno not in { ENOENT, EEXIST }
//
xapi uxopens(int * fd, int flags, const char * path)
  __attribute__((nonnull(3)));

xapi uxopenf(int * fd, int flags, const char * path_fmt, ...)
  __attribute__((nonnull(3)))
  __attribute__((format(printf, 3, 4)));

xapi uxopenvf(int * fd, int flags, const char * path_fmt, va_list va)
  __attribute__((nonnull(3)));

/// uxopenat
//
// SUMMARY
//  proxy for openat that only fails when errno not in { ENOENT, EEXIST }
//
xapi uxopenats(int * fd, int flags, int dirfd, const char * path)
  __attribute__((nonnull(4)));

xapi uxopenatf(int * fd, int flags, int dirfd, const char * path_fmt, ...)
  __attribute__((nonnull(4)))
  __attribute__((format(printf, 4, 5)));

xapi uxopenatvf(int * fd, int flags, int dirfd, const char * path_fmt, va_list va)
  __attribute__((nonnull(4)));

/// xopen_mode
//
// SUMMARY
//  proxy for open that only fails when errno not in { ENOENT, EEXIST }
//
xapi uxopen_modes(int * fd, int flags, mode_t mode, const char * path)
  __attribute__((nonnull(4)));

xapi uxopen_modef(int * fd, int flags, mode_t mode, const char * path_fmt, ...)
  __attribute__((nonnull(4)))
  __attribute__((format(printf, 4, 5)));

xapi uxopen_modevf(int * fd, int flags, mode_t mode, const char * path_fmt, va_list va)
  __attribute__((nonnull(4)));

/// xopenat_mode
//
// SUMMARY
//  proxy for open that only fails when errno not in { ENOENT, EEXIST }
//
xapi uxopenat_modes(int * fd, int flags, mode_t mode, int dirfd, const char * path)
  __attribute__((nonnull(5)));

xapi uxopenat_modef(int * fd, int flags, mode_t mode, int dirfd, const char * path_fmt, ...)
  __attribute__((nonnull(5)))
  __attribute__((format(printf, 5, 6)));

xapi uxopenat_modevf(int * fd, int flags, mode_t mode, int dirfd, const char * path_fmt, va_list va)
  __attribute__((nonnull(5)));

/// xsplice
//
// SUMMARY
//  proxy for splice
//
xapi xsplice(ssize_t * restrict r, int fd_in, loff_t *off_in, int fd_out, loff_t *off_out, size_t len, unsigned int flags);

#endif
