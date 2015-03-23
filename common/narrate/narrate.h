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

#ifndef _NARRATE_H
#define _NARRATE_H

#include <sys/types.h>

struct pstring;			// pstring.h

#define restrict __restrict

typedef struct
{
	int (*sayf)(char * const restrict dst, const size_t sz, struct pstring ** restrict ps, size_t * restrict z, const char * const restrict fmt, ...);
	int (*sayw)(char * const restrict dst, const size_t sz, struct pstring ** restrict ps, size_t * restrict z, const char * const restrict buf, size_t bufl);
} narrator;

/// narrate_fixed
//
// SUMMARY
//  write to a fixed size buffer
//
narrator * narrate_fixed;

/// narrate_dynamic
//
// SUMMARY
//  write to a dynamically-resizing buffer
//
narrator * narrate_dynamic;

#define SAYF(fmt, ...) fatal(writer, dst, sz, z, ps, fmt, ##__VA_ARGS__)
#define SAYS(s) SAYW(s, strlen(s))
#define SAYW(s, l) fatal(writer, dst, sz, ps, z, s, l)

#undef restrict
#endif
