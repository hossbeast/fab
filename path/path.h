#ifndef _PATH_H
#define _PATH_H

#include <stdint.h>

#define restrict __restrict

typedef struct
{
	union
	{
		struct
		{
			char * strings[16];
		};

		struct
		{
			char *			can;				// fully canonicalized representation
			char *			abs;				// non-normalized, but in absolute form
			char *			rel;				// relative form
			char *			can_dir;
			char *			abs_dir;
			char *			rel_dir;
			char *			name;
			char *			ext;
			char * 			ext_last;

			char *			in;
			char *			base;
		};
	};

	int					canl;
	int					absl;
	int					rell;

	int					can_dirl;
	int					abs_dirl;
	int					rel_dirl;

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
//  p    - results go here
//  base - for resolving the absolute path
//  fmt  - 
//
int path_create(path ** const restrict p, const char * const restrict base, const char * const restrict fmt, ...)
	__attribute__((nonnull));

/// path_create_canon
//
// SUMMARY
//  create path instance from an already canonicalized path
//
// PARAMETERS
//  p  - results go here
//  s  - canonicalized path string
//  l  - length of s, 0 for strlen
//
int path_create_canon(path ** const restrict p, const char * const restrict fmt, ...)
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
int path_copy(const path * const restrict A, path ** const restrict B)
	__attribute__((nonnull));

#undef restrict
#endif
