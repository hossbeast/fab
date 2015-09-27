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

#ifndef _CANON_H
#define _CANON_H

#include <stdint.h>
#include "xapi.h"

#define restrict __restrict

#define CAN_SLASH					0x00			// resolve extra "/" characters
#define CAN_INIT_DOT			0x01			// resolve references to the "." and ".." directories at the initial component 
#define CAN_NEXT_DOT			0x02			// resolve references to the "." and ".." directories at positions other than the initial component 
#define CAN_FORCE_DOT			0x04			// force the path to begin with "."
#define CAN_NEXT_SYM			0x08			// resolve symlinks at positions other than the final component which do not cross mount points
#define CAN_NEXT_SYMMNT		0x10			// resolve symlinks at positions other than the final component which cross mount points
#define CAN_FINL_SYM			0x20			// resolve symlinks at the final component which do not cross mount points
#define CAN_FINL_SYMMNT		0x40			// resolve symlinks at the final component which cross mount points

#define CAN_REALPATH			0xFF			// canon = realpath (except ENOENT is not an error)

/// canon - man 3 realpath
//
// SUMMARY
//  convert a path into a more canonical representation by performing a subset
//  of path translation operations. when all operations are specified, the result
//  is a canonicalized absolute pathname, as returned by the realpath function (except
//  that ENOENT is not an error)
//
// PARAMETERS
//  path        - path to be canonicalized
//  [pathl]     - length of path, 0 for strlen
//  [base]      - used to resolve initial references to "." and ".."
//  [basel]     - length of base, 0 for strlen
//  dst         - resolved path is written here
//  siz         - max bytes to write to dst
//  [z]         - returns the number of bytes written
//  opts        - bitwise mask of operations to perform - 0 = CAN_REALPATH
//
// RETURNS
//  0 on ENOMEM, 1 otherwise
//
xapi canon(
	  const char * restrict path
	, int pathl
	, const char * const restrict base
	, int basel
	, char * const restrict dst
	, const size_t siz
	, size_t * restrict z
	, uint32_t opts
)
	__attribute((nonnull(1, 5)));

/// rebase
//
// reformulate a normalized absolute path into a relative path in terms of some absolute base path
//
// PARAMETERS
//  path      - absolute, normalized path to rebase
//  [pathl]   - length of path, 0 for strlen
//  base      - absolute directory path to rebase against
//  [basel]   - length of base, 0 for strlen
//  dst       - path relative to base is written here
//  siz       - max bytes to write to dst
//  [z]       - returns the number of bytes written
//
xapi rebase(
	  const char * const restrict path
	, int pathl
	, const char * const restrict base
	, int basel
	, char * const restrict dst
	, const size_t siz
	, size_t * restrict z
)
	__attribute__((nonnull(1, 3, 5)));

#undef restrict
#endif
