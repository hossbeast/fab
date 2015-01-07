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

#ifndef _LISTWISE_TRANSFORM_H
#define _LISTWISE_TRANSFORM_H

#include <sys/types.h>

struct pstring;		// pstring.h

/// definitions are in operator.h
struct arg;
struct operation;
typedef struct transform_parser transform_parser;

typedef struct transform
{
	struct arg **				args;		// initial list contents
	int									argsl;

	struct operation **	ops;		// operator/args pairs to be executed sequentially
	int									opsl;
} transform;

///
/// [[ TRANSFORM API ]]
///

#define restrict __restrict

/// transform_mkparser
//
// returns a transform parser
//
int transform_mkparser(transform_parser ** p);

/// (XAPI) transform_parse
//
// parse a transform string
//
// parameters
//
//  [p]    - parser returned from mkparser
//   s     - transform string
//   l     - length of transform string, or 0 for strlen
//   r     - receives parsed transform
//  [name] - also specify a name for this input (that is only used in informational and error messages)
//
int transform_parse(transform_parser ** p, char* s, int l, transform** r);
int transform_parse2(transform_parser ** p, char* s, int l, transform** r, void * udata);

/// transform_parse_named
//
// see transform_parse
//
int transform_parse_named(transform_parser ** p, char* s, int l, char * name, int namel, transform** r);
int transform_parse_named2(transform_parser ** p, char* s, int l, char * name, int namel, transform** r, void * udata);

/// transform_freeparser 
//
// frees a transform parser returned from mkparser
//
void transform_parser_free(transform_parser*);
void transform_parser_xfree(transform_parser**);

/// transform_free
//
// frees a transform returned from parse
//
void transform_free(transform*);
void transform_xfree(transform**);

///
/// transform : describe
///

/// transform_canon_write
//
// SUMMARY
//  write a canonical transform-string for the specified transform to the specified buffer
//
// PARAMETERS
//  g   - transform
//  dst - buffer
//  sz  - size of buffer
//  [z] - incremented by the amount written to dst
//
// RETURNS
//  the number of bytes written
//
int transform_canon_write(transform * const restrict g, char * const restrict dst, const size_t sz, size_t * restrict z)
	__attribute__((nonnull(1, 2)));

/// transform_canon_pswrite
//
// SUMMARY
//  write a canonical transform-string for the specified transform to the specified pstring
//
// PARAMETERS
//  g  - transform
//  ps - pstring to write to
//
int transform_canon_pswrite(transform * const restrict g, struct pstring ** restrict ps)
	__attribute__((nonnull));

/// transform_canon_dump
//
// SUMMARY
//  write a canonical transform-string for the specified transform to stdout
//
// PARAMETERS
//  g    - transform
//  [ps] - pstring to write to
//
int transform_canon_dump(transform * const restrict g, struct pstring ** restrict ps)
	__attribute__((nonnull));

/// transform_canon_log
//
// SUMMARY
//  write a canonical transform-string for the specified transform to log_transform
//
// PARAMETERS
//  g       - transform
//  [ps]    - pstring to write to
//  [udata] - passthrough
//
int transform_canon_log(transform * const restrict g, struct pstring ** restrict ps, void * restrict udata)
	__attribute__((nonnull(1)));

/// transform_description_write
//
// SUMMARY
//  write a multiline description for the specified transform to the specified buffer
//
// PARAMETERS
//  g   - transform
//  dst - buffer
//  sz  - size of buffer
//  [z] - incremented by the amount written to dst
//
// RETURNS
//  the number of bytes written
//
int transform_description_write(transform * const restrict g, char * const restrict dst, const size_t sz, size_t * restrict z)
	__attribute__((nonnull(1, 2)));

/// transform_description_pswrite
//
// SUMMARY
//  write a multiline description for the specified transform to the specified pstring
//
// PARAMETERS
//  g  - transform
//  ps - pstring to write to
//
int transform_description_pswrite(transform * const restrict g, struct pstring ** restrict ps)
	__attribute__((nonnull));

/// transform_description_dump
//
// SUMMARY
//  write a multiline description for the specified transform to stdout
//
// PARAMETERS
//  g    - transform
//  [ps] - pstring to write to
//
int transform_description_dump(transform * const restrict g, struct pstring ** restrict ps)
	__attribute__((nonnull));

/// transform_description_log
//
// SUMMARY
//  write a multiline description for the specified transform to log_transform
//
// PARAMETERS
//  g       - transform
//  [ps]    - pstring to write to
//  [udata] - passthrough
//
int transform_description_log(transform * const restrict g, struct pstring ** restrict ps, void * restrict udata)
	__attribute__((nonnull(1)));

#undef restrict
#endif
