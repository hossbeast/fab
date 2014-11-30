
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

#ifndef _MEMBLK_H
#define _MEMBLK_H

#include "xapi.h"

struct mempolicy;		// xlinux/mempolicy.h

#define restrict __restrict

//
// api
//

typedef struct memblk 
{
	char *	s;
	size_t	l;
	size_t	a;

	#ifndef MEMBLK_INTERNALS
	# define MEMBLK_INTERNALS
	#endif
	MEMBLK_INTERNALS
} memblk;

xapi memblk_mk(memblk ** mb, size_t sz);

xapi memblk_alloc(memblk * restrict mb, void * restrict p, size_t sz)
	__attribute__((nonnull));

xapi memblk_realloc(memblk * restrict mb, void * restrict p, size_t es, size_t ec, size_t oec)
	__attribute__((nonnull));

void memblk_free(memblk * mb);

void memblk_xfree(memblk ** mb)
	__attribute__((nonnull));

struct mempolicy * memblk_getpolicy(memblk * mb);

// freeze
// thaw

#undef restrict
#endif
