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
xapi xopen(const char * path, int flags, int * const fd);

/// uxopen
//
// SUMMARY
//  proxy for open that only fails when errno != ENOENT
//
xapi uxopen(const char * path, int flags, int * const fd);

xapi xopenf(const char * path_fmt, int flags, int * const fd, ...)
  __attribute__((nonnull(1)));

xapi xopenvf(const char * path_fmt, int flags, int * const fd, va_list va)
  __attribute__((nonnull(1)));


/// xopen_mode
//
// SUMMARY
//  proxy for open
//
xapi xopen_mode(const char * path, int flags, mode_t mode, int * const fd);

/// xopen_mode
//
// SUMMARY
//  proxy for open that only fails when errno != ENOENT
//
xapi uxopen_mode(const char * path, int flags, mode_t mode, int * const fd);

#endif
