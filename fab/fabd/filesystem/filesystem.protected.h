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

#ifndef _FILESYSTEM_PROTECTED_H
#define _FILESYSTEM_PROTECTED_H

#include <stdint.h>

struct path;				// path.h
struct filesystem;	// filesystem.h

#define restrict __restrict

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
int filesystem_mk(struct filesystem * const restrict e, char * const restrict path, uint64_t attrs, struct filesystem ** restrict fs);

/// filesystem_free
//
// SUMMARY
//  free a filesystem with free semantics
//
void filesystem_free(struct filesystem * const restrict fs);

/// filesystem_xfree
//
// SUMMARY
//  free an filesystem with xfree semantics
//
void filesystem_xfree(struct filesystem ** const restrict fs)
	__attribute__((nonnull));

#undef restrict
#endif
