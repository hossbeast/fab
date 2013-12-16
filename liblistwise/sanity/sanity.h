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

#ifndef _LISTWISE_SANITY_H
#define _LISTWISE_SANITY_H

#ifdef SANITY

#include "listwise/lwx.h"

#define restrict __restrict

typedef struct
{
	struct
	{
		int l;
		int a;
		uint8_t t;

		int x;
		int y;
		typeof(((struct lwx_t*)0)->s[0].s[0].s[0]) *	s;
		typeof(((struct lwx_t*)0)->s[0].s[0].s[0]) *	o;
		int ol;
		int oa;
	} *																	s_strings;
	int																	s_stringsl;
	int																	s_stringsa;
} sanityblock;

int sanityblock_create(sanityblock ** const restrict sb)
	__attribute__((nonnull));

void sanityblock_free(sanityblock * const restrict sb)
	__attribute__((nonnull));

void sanityblock_reset(sanityblock * const restrict sb)
	__attribute__((nonnull));

int sanity(struct lwx_t * const restrict lx, sanityblock * const restrict sb)
	__attribute__((nonnull));

#undef restrict
#endif
#endif
