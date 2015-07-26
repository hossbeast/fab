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

#ifndef _BS_H
#define _BS_H

struct bp;                  // bp.h
struct map;                 // map.h
struct transform_parser;    // listwise/transform.h
struct lwx;                 // listwise.h
struct ts;                  // ts.h

#define restrict __restrict

/// buildscript_mk
//
// SUMMARY
//  create a buildscript from the buildplan
//
// PARAMETERS
//  dst - write the buildscript to this path
//
int buildscript_mk(
	  const struct bp * const bp
	, const char * const restrict argvs
	, struct map * const restrict vmap
	, struct transform_parser * const gp
	, struct lwx *** const restrict stax
	, size_t * const restrict staxa
	, size_t staxp
	, struct map * const restrict bakemap
	, struct ts *** const restrict ts
	, size_t * const restrict tsa
	, const size_t * const restrict tsw
	, const char * const restrict dst
)
	__attribute__((nonnull));

#undef restrict
#endif
