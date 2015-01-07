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

#ifndef _LISTWISE_LWX_H
#define _LISTWISE_LWX_H

#include <stdint.h>

/// listwise execution context
//
// contains the lstack - stack of lists of rows
//
struct lwx
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

/*
	// access order - top list only
	int * order;
	int ordera;
*/

	// subset of string for each row - top list only
	struct
	{
		struct
		{
			struct lwx_windows
			{
				struct
				{
					int   	o;    	// offset
					int   	l;    	// length
				}       	*s;

				int     	l;    	// len - elements in s that are in use
				int     	a;    	// alloc

				int     	zl;   	// sum(s[-].l)
				int				state;	// LWX_WINDOWS_* ; see window.h ; if ALL or NONE, s is ignored

				uint64_t	lease;	// era in which this window is valid
				int				mark;
			} storage[2];

			struct lwx_windows *	active;
			int										active_storage_index;		// index into storage that active points to

			struct lwx_windows *	staged;
			int										staged_storage_index;		// index into storage that staged points to
		} * s;

		int a;											// alloc

		uint64_t		active_era;			// for aging active windows
		uint64_t		staged_era;			// for aging staged windows
	} win;

	// subset of rows - top list only
	struct
	{
		struct lwx_selection
		{
			uint8_t *	s;			// bitvector
			int				l;			// total bits set in s
			int				sl;			// length of s
			int				sa;			// allocated size of s

			int				state;	// LWX_SELECTION_* ; see selection.h ; if ALL or NONE, s is ignored

			uint64_t	lease;	// era in which this selection is valid
		} storage[2];

		struct lwx_selection * active;
		struct lwx_selection * staged;

		uint64_t		active_era;			// for aging active selections
		uint64_t		staged_era;			// for aging staged selections
	} sel;

	uint64_t  flags;		// available for application-use
	void *    ptr;			// available for application-use
};

#endif
