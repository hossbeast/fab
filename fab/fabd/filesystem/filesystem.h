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

#define restrict __restrict

#define FILESYSTEM_INVALIDATION_METHOD_TABLE(x)																			\
	_FILESYSTEM_INVALIDATION_METHOD(STAT		, 0x00	, x)	/* stat hash (default) */		\
	_FILESYSTEM_INVALIDATION_METHOD(CONTENT	, 0x01	, x)	/* content hash */					\
	_FILESYSTEM_INVALIDATION_METHOD(INOTIFY	, 0x02	, x)	/* inotify monitoring */
	
enum
{
#define _FILESYSTEM_INVALIDATION_METHOD(a, b, c) FILESYSTEM_INVALIDATION_METHOD_ ## a = b,
FILESYSTEM_INVALIDATION_METHOD_TABLE(0)
#undef _FILESYSTEM_INVALIDATION_METHOD
};

// filesystem
typedef struct filesystem
{
	char *		path;		// path to the root of the filesystem
	uint32_t	attrs;	// attributes
} filesystem;

/// filesystem_free
//
// free a filesystem with free semantics
//
void filesystem_free(filesystem * const restrict fs);

/// filesystem_xfree
//
// free an filesystem with xfree semantics
//
void filesystem_xfree(filesystem ** const restrict fs)
	__attribute__((nonnull));

#undef restrict
#endif
