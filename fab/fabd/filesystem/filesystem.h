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

#ifndef _FILESYSTEM_H
#define _FILESYSTEM_H

struct path;				// path.h

#define FILESYSTEM_INVALIDATION_TABLE(x)																								\
	_FILESYSTEM_INVALIDATION(STAT			, 0x00	, x)	/* stat hash (default) */							\
	_FILESYSTEM_INVALIDATION(CONTENT	, 0x01	, x)	/* content hash */										\
	_FILESYSTEM_INVALIDATION(NOTIFY		, 0x02	, x)	/* filesystem event subscription */		\
	_FILESYSTEM_INVALIDATION(ALWAYS		, 0x03	, x)	/* always considered invalid */				\
	_FILESYSTEM_INVALIDATION(NEVER		, 0x04	, x)	/* never considered invalid */
	
enum
{
#define _FILESYSTEM_INVALIDATION(a, b, c) FILESYSTEM_INVALIDATION_ ## a = b,
FILESYSTEM_INVALIDATION_TABLE(0)
#undef _FILESYSTEM_INVALIDATION
};

// filesystem
typedef struct filesystem
{
	struct path *		path;		// path to the root of the filesystem
	uint32_t				attrs;	// attributes
} filesystem;

#define restrict __restrict

/// filesystem_free
//
// SUMMARY
//  free a filesystem with free semantics
//
void filesystem_free(filesystem * const restrict fs);

/// filesystem_xfree
//
// SUMMARY
//  free an filesystem with xfree semantics
//
void filesystem_xfree(filesystem ** const restrict fs)
	__attribute__((nonnull));

/// filesystem_mk
//
// SUMMARY
//  create/update a filesystem
//
// PARAMETERS
//  [e]     - existing filesystem to update
//  [path]  - set the path
//  [attrs] - set the attrs
//  [fs]    - (returns) the filesystem
//
int filesystem_mk(filesystem * const restrict e, char * const restrict path, uint64_t attrs, filesystem ** restrict fs);

/// filesystem_setup
//
// SUMMARY
//  setup
//
int filesystem_setup();

/// filesystem_teardown
//
// SUMMARY
//  teardown
//
void filesystem_teardown();

#undef restrict
#endif
