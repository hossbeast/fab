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

#ifndef _LISTWISE_DESCRIBE_H
#define _LISTWISE_DESCRIBE_H

#include "pstring.h"

#define restrict __restrict

/// lstack_description_write
//
// SUMMARY
//  write a multiline description of the specified list-stack to the specified buffer
//
// PARAMETERS
//  lx  - lstack instance
//  dst - buffer to write to
//  sz  - size of the buffer
//  [z] - incremented by the amount written to dst
//
int lstack_description_write(lwx * const restrict lx, char * const restrict dst, const size_t sz, size_t * restrict z)
	__attribute__((nonnull(1, 2)));

/// lstack_description_pswrite
//
// SUMMARY
//  write a multiline description of the specified list-stack to the specified pstring
//
// PARAMETERS
//  lx  - lstack instance
//  ps  - pstring to write to
//
int lstack_description_pswrite(lwx * const restrict lx, pstring ** const restrict ps)
	__attribute__((nonnull(1, 2)));

/// lstack_dump
//
// SUMMARY
//  write a multiline description of the specified list-stack to stdout
//
// PARAMETERS
//  lx   - lstack instance
//  [ps] - pstring to write to
//
int lstack_description_dump(lwx * const restrict, pstring ** restrict ps)
	__attribute__((nonnull));

/// lstack_description_log
//
// SUMMARY
//  write a multiline description of the specified list-stack to log_lstack
//
// PARAMETERS
//  lx      - lstack instance
//  [ps]    - pstring to write to
//  [udata] - passthrough
//
int lstack_description_log(lwx * const restrict, pstring ** restrict ps, void * restrict udata)
	__attribute__((nonnull));

#undef restrict
#endif
