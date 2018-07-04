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

#ifndef _XSTDIO_H
#define _XSTDIO_H

#include <stdarg.h>

#include "xapi.h"
#include "types.h"

/// xvdprintf
//
// SUMMARY
//  proxy for vdprintf
//
// PARAMETERS
//  as for vdprintf
//
xapi xvdprintf(int fd, const char * const restrict fmt, va_list va)
  __attribute__((nonnull(2)));

/// xrename
//
// SUMMARY
//  xapi proxy for rename
//
// VARIANTS
//  s/f/vf - different ways to pass path
//
xapi xrenames(const char * const restrict old, const char * const restrict new)
  __attribute__((nonnull));

xapi xrenamef(const char * const restrict oldfmt, const char * const restrict newfmt, ...)
  __attribute__((nonnull(1, 2)));

xapi xrenamevf(const char * const restrict oldfmt, const char * const restrict newfmt, va_list va)
  __attribute__((nonnull(1, 2)));

/// uxrename
//
// SUMMARY
//  proxy for rename that only fails when errno not in { ENOENT, ENOTDIR }
//

xapi uxrenames(const char * const restrict old, const char * const restrict new)
  __attribute__((nonnull));

xapi uxrenamef(const char * const restrict oldfmt, const char * const restrict newfmt, ...)
  __attribute__((nonnull(1, 2)));

xapi uxrenamevf(const char * const restrict oldfmt, const char * const restrict newfmt, va_list va)
  __attribute__((nonnull(1, 2)));

#endif
