/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

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

#ifndef _BAKE_H
#define _BAKE_H

#include <listwise.h>
#include <listwise/generator.h>

#include "bp.h"

#define restrict __restrict

/// bake_bp
//
// render the buildplan to a bakescript
//
int bake_bp(
	  const bp * const bp
	, map * const restrict vmap
	, generator_parser * const gp
	, lstack *** const restrict stax
	, int * const restrict staxa
	, int staxp
	, map * const restrict bakemap
	, ts *** const restrict ts
	, int * const restrict tsa
	, const int * const restrict tsw
	, const char * const restrict dst
)
	__attribute__((nonnull));

#undef restrict
#endif

