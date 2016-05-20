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

#ifndef _MEMBLK_DEF_H
#define _MEMBLK_DEF_H

struct memblk 
{
	struct {
		char *	s;
		size_t	l;
		size_t	a;
		size_t	o;			// cumulative offset
	} *			blocks;
	size_t	blocksl;
	size_t	blocksa;

#ifndef MEMBLK_INTERNALS
# define MEMBLK_INTERNALS
#endif
	MEMBLK_INTERNALS
};

#define restrict __restrict

/// memblk_freeze
//
// SUMMARY
//  convert a pointer to memblk-allocd memory to an offset from the start of the memblk
//
// PARAMETERS
//  mb - memblk instance
//  p  - pointer to pointer to memory allocated with mb (T**)
//
void memblk_freeze(struct memblk * const restrict mb, void * restrict p)
	__attribute__((nonnull));

/// memblk_unfreeze
//
// SUMMARY
//  reverse the effects of memblk_freeze
//
// PARAMETERS
//  mb - memblk instance
//  p  - pointer to pointer to memory allocated with mb (T**)
//
void memblk_unfreeze(struct memblk * const restrict mb, void * restrict p)
	__attribute__((nonnull));

/// memblk_thaw
//
// SUMMARY
//  recover objects from an consolidated memblk
//
// PARAMETERS
//  mb - pointer to memory consolidated from an memblk (memblk_writeto, memblk_copyto)
//  p  - pointer to pointer to memory allocated with mb (T**)
//
void memblk_thaw(char * const restrict mb, void * restrict p)
	__attribute__((nonnull));

#undef restrict
#endif
