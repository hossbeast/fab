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

#ifndef _LISTWISE_FS_H
#define _LISTWISE_FS_H

#include <sys/types.h>
#include <stdint.h>

#include "xapi.h"

#define restrict __restrict

/// fs_statfmt
//
// SUMMARY
//
// PARAMETERS
//  s       - path
//  [sl]    - length of s, 0 for strlen
//  [fmt]   - printf-style expression (modelled after find)
//  [flags] - flags string (L, C, F)
//  dst     - write here
//  sz      - maximum number of bytes to write to dst
//  z       - returns the number of bytes written
//
// REMARKS
//
xapi fs_statfmt(
    const char * const restrict s
  , int sl
  , const char * restrict fmt
  , const char * const restrict flags
  , char * const restrict dst
  , size_t sz
  , size_t * const restrict z
)
  __attribute__((nonnull(1, 5, 7)));

#undef restrict
#endif
