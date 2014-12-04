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

#ifndef _DEPBLOCK_H
#define _DEPBLOCK_H

#include <stdint.h>
#include <sys/types.h>

#include "path.h"

#define restrict __restrict

/*
** for now a single depblock corresponds to a single PRIMARY file, and may contain relations
** A -> B from up to 8 nodes A
**
**  foo.m32.pic.o/weak
**  foo.m32.pic.o/weak
**  foo.m64.nopic.o/weak
**  foo.m64.nopic.o/weak
**  foo.m32.pic.o/noweak
**  foo.m32.pic.o/noweak
**  foo.m64.nopic.o/noweak
**  foo.m64.nopic.o/noweak
**
*/

typedef struct
{
	uint8_t setsl;

	struct
	{
		uint8_t	weak;
		uint8_t bridge;

		char		needs[256];
		char		nbase[256];

		uint8_t	feedsl;
		char		fbase[256];
		char		feeds[256][256];
	} sets[16];
} dep_relations;

typedef struct depblock
{
	char *						blockpath;		// path to the block file

	// mmap fields
	void *						addr;
	size_t						size;
	int								fd;

	// dependency block - may point to heap-allocated or memmapped memory, depending on whether
  // depblock_read or depblock_allocate was called
	dep_relations *		block;
} depblock;

/// depblock_create
//
// allocate a depblock instance and populate blockpath
//
int depblock_create(depblock ** const restrict block, const char * const restrict fmt, ...)
	__attribute__((nonnull(1, 2)));

///  depblock_free
//
// free a depblock (free semantics)
//
void depblock_free(depblock * const restrict block);

///  depblock_xfree
//
// free a depblock (xfree semantics)
//
void depblock_xfree(depblock ** const restrict block)
	__attribute__((nonnull));

/*
** call path : allocate -> addrelation* -> write -> close
*/

/// depblock_allocate
//
// 
//
int depblock_allocate(depblock * const restrict block)
	__attribute__((nonnull));

/// depblock_addrelation
//
//
//
int depblock_addrelation(depblock * const restrict block, const path * const restrict A, const path * const restrict B, int isweak, int isbridge)
	__attribute__((nonnull));

/// depblock_write
//
//
//
int depblock_write(const depblock * const restrict block)
	__attribute__((nonnull));

/*
** call path : read -> process -> close
*/

/// depblock_read
//
// load the depblock from the backing cache file
//
int depblock_read(depblock * const restrict block)
	__attribute__((nonnull));

/// depblock_close
//
//
//
int depblock_close(depblock * const restrict block)
	__attribute__((nonnull));

#undef restrict
#endif
