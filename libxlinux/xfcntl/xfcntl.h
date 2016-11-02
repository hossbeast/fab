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
// VARIANTS
//  s/f/vf - different ways to pass path
//
xapi xopens(int * fd, int flags, const char * path)
  __attribute__((nonnull(3)));

xapi xopenf(int * fd, int flags, const char * path_fmt, ...)
  __attribute__((nonnull(3)));

xapi xopenvf(int * fd, int flags, const char * path_fmt, va_list va)
  __attribute__((nonnull(3)));

/// xopen_mode
//
// SUMMARY
//  proxy for open
//
// VARIANTS
//  s/f/vf - different ways to pass path
//
xapi xopen_modes(int * fd, int flags, mode_t mode, const char * path)
  __attribute__((nonnull(4)));

xapi xopen_modef(int * fd, int flags, mode_t mode, const char * path_fmt, ...)
  __attribute__((nonnull(4)));

xapi xopen_modevf(int * fd, int flags, mode_t mode, const char * path_fmt, va_list va)
  __attribute__((nonnull(4)));

/// uxopen
//
// SUMMARY
//  proxy for open that only fails when errno not in { ENOENT, EEXIST }
//
// VARIANTS
//  s/f/vf - different ways to pass path
//
xapi uxopens(int * fd, int flags, const char * path)
  __attribute__((nonnull(3)));

xapi uxopenf(int * fd, int flags, const char * path_fmt, ...)
  __attribute__((nonnull(3)));

xapi uxopenvf(int * fd, int flags, const char * path_fmt, va_list va)
  __attribute__((nonnull(3)));

/// xopen_mode
//
// SUMMARY
//  proxy for open that only fails when errno not in { ENOENT, EEXIST }
//
// VARIANTS
//  s/f/vf - different ways to pass path
//
xapi uxopen_modes(int * fd, int flags, mode_t mode, const char * path)
  __attribute__((nonnull(4)));

xapi uxopen_modef(int * fd, int flags, mode_t mode, const char * path_fmt, ...)
  __attribute__((nonnull(4)));

xapi uxopen_modevf(int * fd, int flags, mode_t mode, const char * path_fmt, va_list va)
  __attribute__((nonnull(4)));

#endif
