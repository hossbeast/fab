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

#ifndef _LISTWISE_H
#define _LISTWISE_H

#include <stdint.h>

#define restrict __restrict

/// list-stack
//
// stack of lists of strings
//
struct lstack;
typedef struct lstack
{
	struct		// stack
	{
		struct // list
		{
			char*		s;		// string

			int			l;		// len - number of characters
			int			a;		// alloc

			uint8_t	type;	// object type - see list/object interface
		}					*s;

		struct	// window
		{
			struct
			{
				int		o;		// offset
				int		l;		// length
			}				*s;

			int			l;		// len - elements in s that are in use
			int			a;		// alloc

			int			zl;		// sum(w[-].l)
			int			y;		// isset
		}					*w;

		struct	// internal tmp space
		{
			char*		s;

			int			l;
			int			a;

			int			y;	// isset
		}					*t;

		int			l;	// len - number of strings
		int			a;	// alloc
	}					*s;

	int			l;	// len - number of lists
	int			a;	// alloc

	struct					// items affected by the last operation - on the top list
	{
		uint8_t *	s;	// bitvector
		int				l;	// number of bits set in s
		int				sl;	// length of s
		int				sa;	// allocated size of s
	} last;

	struct 						// currently selected items - on the top list
	{
		uint8_t *	s;		// bitvector
		int				l;		// number of bits set in s
		int				sl;		// length of s
		int				sa;		// allocated size of s

		char			all;	// if true, all items are selected - otherwise, exactly those items
										// as specified in s comprise the current selection
	} sel;

	uint64_t	flags;	// available for application-use
	void *		ptr;		// available for application-use
} lstack;

/// listwise_exec
//
// SUMMARY
//  execute the listwise generator, receive an lstack result
//
// PARAMETERS
//  s      - generator string
//  l      - s length, or 0 for strlen
//  init   - items to write to the stack before executing
//  initls - lengths for items in init, 0 for strlen
//  initl  - number of items in init
//  r      - receives results
//
// RETURNS
//  1 for success
//
int listwise_exec(char* s, int l, char** init, int* initls, int initl, lstack** r);

/// lstack_create
//
// SUMMARY
//  allocate an lstack
//
int lstack_create(lstack ** const restrict)
	__attribute__((nonnull));

/// lstack_free
//
// SUMMARY
//  free lstack with free-like semantics
//
void lstack_free(lstack * const restrict);

/// lstack_xfree
//
// SUMMARY
//  free an lstack with xfree-like semantics
//
void lstack_xfree(lstack ** const restrict)
	__attribute__((nonnull));

/// lstack_deepcopy
//
// SUMMARY
//  create a deep copy of an lstack
//
// PARAMETERS
//  dst - copy returned here
//  src - 
//  all - entire allocations are copied (the default is only up to the in-use size)
//
// RETURNS
//  0 on failure (allocation) and 1 otherwise
//
// NOTES
//  selection, last and windows are not copied
//
int lstack_deepcopy(lstack ** const restrict, lstack * const restrict)
	__attribute__((nonnull));

/// listwise_err_fd
//
// SUMMARY
//  listwise operators write errors to this fd (ls a nonexistent path, for example)
//
// DEFAULT
//  2 - stderr
//
extern int listwise_err_fd;

/// listwise_identity 
//
// SUMMARY
//  pointer to singleton liblistwise-managed lstack object with a single 0-element list
//
extern lstack * listwise_identity;

#undef restrict
#endif
