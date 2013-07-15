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

#ifndef _PATH_H
#define _PATH_H

#include <stdint.h>

#define restrict __restrict

typedef struct
{
	// managed strings
	union
	{
		struct
		{
			char * strings[16];
		};

		struct
		{
			char *			in_path;			// input path
			char *			in_base;			// input base

			char *			can;					// fully canonicalized representation
			char *			can_dir;
			char *			abs;					// non-normalized, but in absolute form
			char *			abs_dir;
			char *			rel_cwd;			// relative to base
			char *			rel_cwd_dir;
			char *			rel_fab;			// relative to base
			char *			rel_fab_dir;
			char *			rel_nofile;		// relative to base
			char *			rel_nofile_dir;

			char *			name;
			char *			ext;
			char * 			ext_last;
		};
	};

	int					in_pathl;
	int					in_basel;

	int					canl;
	int					can_dirl;
	int					absl;
	int					abs_dirl;

	int					rel_cwdl;					// relative to cwd
	int					rel_cwd_dirl;
	int					rel_fabl;					// relative to init-fabfile-dir
	int					rel_fab_dirl;
	int					rel_nofilel;			// relative to /..
	int					rel_nofile_dirl;

	int					namel;
	int					extl;
	int					ext_lastl;

	uint32_t		can_hash;
	int					is_nofile;
} path;

/// path_create
//
// SUMMARY
//  create path instance from a relative path representation
//
// PARAMETERS
//  p        - result path object
//  in_base  - input path is relative to this base
//  fmt      - input path
//
int path_create(path ** const restrict p, const char * const restrict in_base, const char * const restrict fmt, ...)
	__attribute__((nonnull));

int path_create_init(path ** const restrict p, const char * const restrict in_base, const char * const restrict fmt, ...)
	__attribute__((nonnull));

/// path_cmp
//
// SUMMARY
//  compare two paths
//
int path_cmp(const path * const restrict, const path * const restrict)
	__attribute__((nonnull));

/// path_free
//
// SUMMARY
//  free a path with free semantics
//
void path_free(path * const restrict);

/// path_xfree
//
// SUMMARY
//  free a path with xfree semantics
//
void path_xfree(path ** const restrict)
	__attribute__((nonnull));

/// path_copy
//
// SUMMARY
//  create a deep copy of a path object
//
// PARAMETERS
//  dst -
//  src -
//
int path_copy(path ** const restrict dst, const path * const restrict src)
	__attribute__((nonnull));

#undef restrict
#endif
