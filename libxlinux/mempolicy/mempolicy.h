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

#ifndef _XLINUX_MEMPOLICY_H
#define _XLINUX_MEMPOLICY_H

#include <sys/types.h>

typedef struct mempolicy
{
	int (*malloc)(struct mempolicy * plc, void * target, size_t size)
		__attribute__((nonnull));

	int (*realloc)(struct mempolicy * plc, void * target, size_t es, size_t ec, size_t oec)
		__attribute__((nonnull));
	
	void (*ifree)(struct mempolicy * plc, void * target)
		__attribute__((nonnull));

	void (*free)(struct mempolicy * plc, void * target);
} mempolicy;

#define restrict __restrict

/// mempolicy_push
//
// SUMMARY
//  cause xlinux memory allocations on this thread to be handled by the specified policy
//
// PARAMETERS
//  [plc] - policy, no-op if null
//  [mpc] - incremented when the push succeeds
//
// ERRORS
//  MPOLICY - operation would exceed policy stack max size
//
int mempolicy_push(mempolicy * plc, int * const restrict mpc);

/// mempolicy_pop
//
// SUMMARY
//  pop a single mempolicy from the stack
//
// PARAMETERS
//  [mpc] - decremented
//
// RETURNS
//  returns the previously active mempolicy, if any
//
mempolicy * mempolicy_pop(int * const restrict mpc);

/// mempolicy_unwind
//
// SUMMARY
//  pop *mpc policies from the stack
//
// PARAMETERS
//  [mpc] - zeroed
//
// RETURNS
//  returns the previously active mempolicy, if any
//
mempolicy * mempolicy_unwind(int * const restrict mpc);

#undef restrict
#endif
