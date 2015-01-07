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

#ifndef _HASHBLOCK_H
#define _HASHBLOCK_H

#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define restrict __restrict

#define HB_SAME			0		/* unchanged */
#define HB_STAT			1		/* differ in stat properties */
#define HB_CONTENT	2		/* differ in file contents */

typedef struct hashblock
{
	struct __attribute__((packed))
	{
		// stathash fields
		dev_t							dev;     /* ID of device containing file */
		ino_t							ino;     /* inode number */
		mode_t						mode;    /* protection */
		nlink_t						nlink;   /* number of hard links */
		uid_t							uid;     /* user ID of owner */
		gid_t							gid;     /* group ID of owner */
		off_t							size;    /* total size, in bytes */
		time_t						mtime;   /* time of last modification */
		time_t						ctime;   /* time of last status change */
	};

	uint32_t					stathash[2];		// hash of fs properties
	uint32_t					contenthash[2];	// hash of file contents
} hashblock;

/// hashblock_create
//
// SUMMARY
//  create a hashblock 
//
int hashblock_create(hashblock ** const restrict hb)
	__attribute__((nonnull));

/// hashblock_free
//
// SUMMARY
//  free a hashblock with free semantics
//
void hashblock_free(hashblock * const restrict hb);


/// hashblock_xfree
//
// SUMMARY
//  free a hashblock with xfree semantics
//
void hashblock_xfree(hashblock ** const restrict hb)
	__attribute__((nonnull));

/// hashblock_stat
//
// SUMMARY
//  stat the specified file and populate stathash[1] on all specified hashblocks
//
int hashblock_stat(const char * const restrict path, hashblock * const restrict hb)
	__attribute__((nonnull));

/// hashblock_cmp
//
// SUMMARY
//  compare the two states contained in the hashblock
//
// RETURNS
//  one of HBLOCK_*
//
int hashblock_cmp(const hashblock * const restrict)
	__attribute__((nonnull));

/// hashblock_reset
//
// SUMMARY
//  reset the hashblock to its initial state
//
void hashblock_reset(hashblock * const restrict)
	__attribute__((nonnull));

#undef restrict
#endif
