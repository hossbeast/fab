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

#include "listwise.h"
#include "listwise/transform.h"

#include "bp.h"

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
	  const bp * const bp
	, const char * const restrict argvs
	, map * const restrict vmap
	, transform_parser * const gp
	, lwx *** const restrict stax
	, size_t * const restrict staxa
	, size_t staxp
	, map * const restrict bakemap
	, ts *** const restrict ts
	, size_t * const restrict tsa
	, const int * const restrict tsw
	, const char * const restrict dst
)
	__attribute__((nonnull));

#undef restrict
#endif
