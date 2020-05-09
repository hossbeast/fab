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

#ifndef _MKDIRP_H
#define _MKDIRP_H

#include <stdarg.h>
#include <sys/types.h>

#include "xapi.h"

/// mkdirp
//
// SUMMARY
//  ensure a directory path exists, iteratively creating each component of the path
//  if it does not already exist
//
// PARAMETERS
//  mode    - mode to create directories with
//  dirpath - path to directory to create
//
// PARAMETERS
//  s/w/f/vf - different ways of providing path
//
xapi mkdirps(mode_t mode, const char * const restrict path)
  __attribute__((nonnull));

xapi mkdirpw(mode_t mode, const char * const restrict path, size_t pathl)
  __attribute__((nonnull));

xapi mkdirpf(mode_t mode, const char * const restrict fmt, ...)
  __attribute__((nonnull));

xapi mkdirpvf(mode_t mode, const char * const restrict fmt, va_list va)
  __attribute__((nonnull));

#endif
