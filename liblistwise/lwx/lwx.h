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

#ifndef _LISTWISE_LWX_H
#define _LISTWISE_LWX_H

#include <stdint.h>

//
// listwise execution context
//
struct lwx_t
{
	// temp storage associated with each row
	struct
	{
		struct // stack
		{
			struct  // tmp space
			{
				char*   s;
			
				int     l;
				int     a;
			
				int     y;  // isset (0=unset, 2=window)
			} * t;
			
			int     l;  // len - number of rows
			int     a;  // alloc
		} * s;

		int		l;		// len - number of lists
		int		a;		// alloc
	} tmp;

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
		} * s;

		int l;		// len - number of rows (first list)
		int a;		// alloc
	}	win;

	// subset of rows - top list only
	struct
	{
		struct selection
		{
			uint8_t *	s;		// bitvector
			int				l;		// number of bits set in s
			int				sl;		// length of s
			int				sa;		// allocated size of s

			char			all;	// if true, all rows are selected - otherwise, exactly those rows
											// as specified in s comprise the current selection
		} storage[2];

		struct selection *		active;
		struct selection *		staged;
	} sel;
};

#endif
