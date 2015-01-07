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

#ifndef _LWUTIL_H
#define _LWUTIL_H

#include "listwise.h"
#include "listwise/transform.h"

// listwise interface to list objects
//  (this type is a placeholder - instances of this object are NOT ever passed into liblistwise)
listwise_object listlw;

#define LISTWISE_TYPE_LIST	0x02		/* listwise type id */

#define restrict __restrict

/// lw_ensure
//
// SUMMARY
//  ensure allocation of listwise stax up to staxp
//
// PARAMETERS
//  stax  - listwise stax
//  staxa - listwise stax
//  staxp - offset to next free stax
//
int lw_ensure(lwx *** stax, int * staxa, int staxp)
	__attribute__((nonnull));

/// lw_reset
//
// SUMMARY
//  ensure allocation of listwise stax up to staxp, reset entry @ stax[staxp]
//
// PARAMETERS
//  stax  - listwise stax
//  staxa - listwise stax
//  staxp - offset to next free stax
//
int lw_reset(lwx *** stax, int * staxa, int staxp)
	__attribute__((nonnull));

/// lw_flatten
//
// replace LISTWISE_TYPE_LIST objects with their contents
//
int lw_flatten(lwx * restrict ls)
	__attribute__((nonnull));

/// lw_lstack_exec
//
// SUMMARY
//  execute listwise
//
int lw_exec(transform * gen, lwx ** ls)
	__attribute__((nonnull));

/// lw_configure_logging
//
// SUMMARY
//  setup liblistwise logging
//
void lw_configure_logging();

#undef restrict
#endif
