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

#ifndef _LORIEN_SYMLINKP_H
#define _LORIEN_SYMLINKP_H

#include <stdarg.h>


/// symlinkp
//
// SUMMARY
//  rm linkpath (but dont fail if linkpath doesnt exist), then symlink(target, linkpath)
//
// PARAMETERS
//  target   - symlink value
//  linkpath - symlink location
//
// VARIANTS
//  s/f/vf - different ways of providing parameters
//
void symlinkps(const char * const restrict target, const char * const restrict linkpath)
  __attribute__((nonnull));

void symlinkpf(const char * const restrict target_fmt, const char * const restrict linkpath_fmt, ...)
  __attribute__((nonnull));

void symlinkpvf(const char * const restrict target_fmt, const char * const restrict linkpath_fmt, va_list va)
  __attribute__((nonnull));

#endif
