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

#include "xapi.h"

struct pstring;             // valyria/pstring.h
struct narrator;            // narrator.h
struct arg;                 // operator.h
struct operation;           // operator.h
struct transform_parser;    // transform.c

typedef struct transform
{
  struct arg **       args;   // initial list contents
  int                 argsl;

  struct operation ** ops;    // operator/args pairs to be executed sequentially
  int                 opsl;
} transform;

#define restrict __restrict

///
/// [[ TRANSFORM API ]]
///

/// transform_parser_create
//
// SUMMARY
//  allocate a transform-string parser
//
xapi transform_parser_create(struct transform_parser ** restrict)
  __attribute__((nonnull));

/// transform_parser_free
//
// SUMMARY
//  free a transform-string parser with free semantics
//
void transform_parser_free(struct transform_parser * restrict);

/// transform_parser_ifree
//
// SUMMARY
//  free a transform-string parser with iwfree semantics
//
void transform_parser_ifree(struct transform_parser ** const restrict)
  __attribute__((nonnull));

/// transform_parse
//
// SUMMARY
//  parse a transform-string
//
// PARAMETERS
//  p      - (returns) reusable parser instance
//  s      - transform string
//  [sl]   - length of transform string, or 0 for strlen
//  [name] - also specify a name for this input (that is only used in informational and error messages)
//  r      - receives parsed transform
//
xapi transform_parse(struct transform_parser ** restrict p, const char * const restrict s, size_t sl, transform ** const restrict r)
  __attribute__((nonnull(2,4)));

xapi transform_parsex(struct transform_parser ** restrict p, const char * const restrict s, size_t sl, char * const restrict name, transform ** r)
  __attribute__((nonnull(2,5)));

/// transform_free
//
// SUMMARY
//  frees a transform with free semantics
//
void transform_free(transform * restrict);

/// transform_ifree
//
// SUMMARY
//  free a transform with iwfree semantics
//
void transform_ifree(transform ** const restrict)
  __attribute__((nonnull));

/// transform_canon_say
//
// SUMMARY
//  write the canonical representation of a transform to a narrator
//
// PARAMETERS
//  g - transform
//  N - narrator
//
xapi transform_canon_say(transform * const restrict g, struct narrator * const restrict N)
  __attribute__((nonnull));

/// transform_description_say
//
// SUMMARY
//  write a descriptive / verbose / human readable / representation of a transform to a narrator
//
// PARAMETERS
//  g - transform
//  N - narrator
//
xapi transform_description_say(transform * const restrict g, struct narrator * const restrict N)
  __attribute__((nonnull));

#undef restrict
#endif
