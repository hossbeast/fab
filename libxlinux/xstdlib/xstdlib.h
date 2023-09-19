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

#ifndef _XSTDLIB_H
#define _XSTDLIB_H

#include <stdlib.h>
#include <sys/types.h>
#include <stdarg.h>

/// xmalloc
//
// SUMMARY
//  does this: *target = calloc(size, 1);
//  recall that calloc zeroes out the memory it allocates
//
// RETURNS
//  returns 0 on success
//
void xmalloc(void* target, size_t size)
  __attribute__((nonnull));

/// xrealloc
//
// SUMMARY
//  does this: realloc(*target, size) AND ensures that any new portion of memory is zeroed out
//
// PARAMETERS
//  es  - element size
//  ec  - element count
//  oec - old element count
//
// RETURNS
//  returns 0 on success
//
void xrealloc(void* target, size_t es, size_t ec, size_t oec)
  __attribute__((nonnull));

/// wfree
//
// SUMMARY
//  xproxy for free
//
void wfree(void* target);

/// iwfree
//
// SUMMARY
//  does this: wfree(*target); *target = 0;
//
void iwfree(void* target)
  __attribute__((nonnull));

/// xreadlink
//
// SUMMARY
//  proxy for readlink
//
// VARIANTS
//  s/f/vf - different ways of providing pathname
//
ssize_t xreadlinks(const char * pathname, char * buf, size_t bufsiz)
  __attribute__((nonnull(1, 2)));

ssize_t xreadlinkf(const char * pathname_fmt, char * buf, size_t bufsiz, ...)
  __attribute__((nonnull(1, 2)));

ssize_t xreadlinkvf(const char * pathname_fmt, char * buf, size_t bufsiz, va_list va)
  __attribute__((nonnull(1, 2)));

/// xrealpath
//
// SUMMARY
//  proxy for realpath
//
char * xrealpaths(char * restrict resolved_path, const char * restrict path)
  __attribute__((nonnull(2)));

char * xrealpathf(char * restrict resolved_path, const char * restrict path_fmt, ...)
  __attribute__((nonnull(2)))
  __attribute__((format(printf, 2, 3)));

char * xrealpathvf(char * restrict resolved_path, const char * restrict path_fmt, va_list va)
  __attribute__((nonnull(2)));

int xsystem(const char * restrict command)
  __attribute__((nonnull));

#endif
