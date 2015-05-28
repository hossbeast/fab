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

#ifndef _CFG_PROTECTED_H
#define _CFG_PROTECTED_H

#include "coll.h"

#define restrict __restrict

struct filesystem;	// filesystem.h
struct map;					// map.h
struct path;				// path.h
struct hashblock;		// hashblock.h

typedef struct cfg_file
{
	struct path *					path;	    // path to cfg file
	struct hashblock *		hb;				// hashblock

	/* filesystem declarations in the cfg file ; only present during cfg file parsing
	** after which filesystems are consolidated in the filesystem module 
	*/
	struct filesystem **	fs;				// filesystem declarations in the cfg file
	size_t								fsl;			// length
	size_t								fsa;			// allocated size
} cfg_file;

extern union cfg_files
{
	coll_doubly c;

	struct
	{
		size_t		l;				// length
		size_t		a;				// allocated
		size_t		z;				// el size

		cfg_file ** e;			// elements

		struct map * by_canpath;
	};
} cfg_files;

/// cfg_mk_fs
//
// SUMMARY
//  create or append to an cfg_file instance
//
// PARAMETERS
//  [e] - existing cfg_file to append to
//  fs  - filesystem declaration to append
//  cfg - (returns) cfg_file instance
//
int cfg_mk_fs(struct cfg_file * const restrict e, struct filesystem * const restrict fs, struct cfg_file ** const restrict cfg)
	__attribute__((nonnull(2, 3)));

#undef restrict
#endif
