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

#include "xapi.h"

typedef struct mempolicy
{
	xapi (*malloc)(struct mempolicy * plc, void * target, size_t size)
		__attribute__((nonnull));

	xapi (*realloc)(struct mempolicy * plc, void * target, size_t es, size_t ec, size_t oec)
		__attribute__((nonnull));
	
	void (*ifree)(struct mempolicy * plc, void * target)
		__attribute__((nonnull));

	void (*free)(struct mempolicy * plc, void * target);
} mempolicy;

/// mempolicy_engage
//
// SUMMARY
//  cause xlinux memory allocation functions on this thread to be handled by the specified policy
//
void mempolicy_engage(mempolicy * plc)
	__attribute__((nonnull));

/// mempolicy_engage
//
// SUMMARY
//  cause xlinux memory allocation functions on this thread to resume the default behavior
//
// RETURNS
//  returns the previously active mempolicy, if any
//
mempolicy * mempolicy_release();

#endif
