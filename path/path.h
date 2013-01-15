#ifndef _PATH_H
#define _PATH_H

#include <stdint.h>

#define restrict __restrict

typedef struct
{
	char *			can;				// fully canonicalized representation
	int					canl;
	char *			abs;				// non-normalized, but in absolute form
	int					absl;
	char *			rel;				// relative form
	int					rell;

	char *			can_dir;
	int					can_dirl;
	char *			abs_dir;
	int					abs_dirl;
	char *			rel_dir;
	int					rel_dirl;
	char *			peer_dir;
	int					peer_dirl;

	char *			name;
	int					namel;
	char *			ext;
	int					extl;
	char * 			ext_last;
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
