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

#ifndef _LISTWISE_INTERNAL_H
#define _LISTWISE_INTERNAL_H

#include "listwise.h"
#include "listwise/operator.h"
#include "listwise/generator.h"
#include "listwise/ops.h"
#include "listwise/object.h"
#include "listwise/xtra.h"

#include "coll.h"
#include "idx.h"
#include "lwx.h"

#define API __attribute__((visibility("protected")))
#define APIDATA

// collection of registered object types
// with lookup index by type id
extern union object_registry_t
{
	coll_doubly c;

	struct
	{
		int l;
		int a;
		int z;

		struct listwise_object ** e;		// object defs

		idx * by_type;					// indexed by type
	};
} object_registry;

/// listwise execution context
//
// contains the lstack - stack of lists of rows
//
struct lwx_t
{
	struct // stack of lists
	{
		struct // list of rows
		{
			char*   s;    // string

			int     l;    // len - number of characters
			int     a;    // alloc
	
			uint8_t type; // object type - see listwise/object.h interface
		} * s;

		struct  // list of tmp space
		{
			char*   s;

			int     l;
			int     a;

#define LWTMP_UNSET		0
#define LWTMP_STRING	1
#define LWTMP_WINDOW	2

			int     y;  // isset - one of LWTMP_*
		} * t;
			
		int     l;  // len - number of rows
		int     a;  // alloc
	} * s;

	int     l;  // len - number of lists
	int     a;  // alloc

	// subset of string for each row - applies to top list only
	struct
	{
		struct
		{
			struct window
			{
				struct
				{
					int   o;    // offset
					int   l;    // length
				}       *s;
				
				int     l;    // len - elements in s that are in use
				int     a;    // alloc
				
				int     zl;   // sum(s[-].l)
			} storage[2];

			struct window * active;
			struct window * staged;

			uint64_t lease;
		} * s;

		int l;		// len - number of rows (first list)
		int a;		// alloc
	} win;

	// subset of rows - top list only
	struct
	{
		struct selection
		{
			uint8_t *	s;		// bitvector
			int				l;		// number of bits set in s
			int				sl;		// length of s
			int				sa;		// allocated size of s
		} storage[2];

		struct selection * active;
		struct selection * staged;
	} sel;

	uint64_t 	era;			// for aging windows

	uint64_t  flags;		// available for application-use
	void *    ptr;			// available for application-use
} lstack;

#endif
