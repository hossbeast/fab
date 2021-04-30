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

#ifndef _FABD_PATTERN_PARSER_H
#define _FABD_PATTERN_PARSER_H

#include "xapi.h"
#include "types.h"

struct yyu_location;
struct pattern;

struct pattern_parser;
typedef struct pattern_parser pattern_parser;

/// pattern_parser_create
//
// SUMMARY
//  create a pattern parser
//
xapi pattern_parser_create(pattern_parser ** const restrict p)
  __attribute__((nonnull));

/// pattern_parser_xfree
//
// SUMMARY
//  free a pattern parser with free semantics
//
xapi pattern_parser_xfree(pattern_parser * restrict);

/// pattern_parser_ixfree
//
// SUMMARY
//  free a pattern parser with iwfree semantics
//
xapi pattern_parser_ixfree(pattern_parser ** restrict)
  __attribute__((nonnull));

/*
 * parse pattern text in place
 *
 * parser     - reusable parser
 * buf        - buffer containing pattern text
 * size       - size of buffer
 * [fname]    - filename, for error messages
 * [init_loc] - starting location
 * [used_loc] - location delta from init_loc
 * [rv]       - (returns) parsed pattern object
 */
xapi search_pattern_parse_partial(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , struct yyu_location * restrict init_loc
  , struct yyu_location * restrict used_loc
  , struct pattern ** restrict rv
)
  __attribute__((nonnull(1, 2)));

xapi generate_pattern_parse_partial(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , struct yyu_location * restrict init_loc
  , struct yyu_location * restrict used_loc
  , struct pattern ** restrict rv
)
  __attribute__((nonnull(1, 2)));

xapi reference_pattern_parse_partial(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , struct yyu_location * restrict init_loc
  , struct yyu_location * restrict used_loc
  , struct pattern ** restrict rv
)
  __attribute__((nonnull(1, 2)));

xapi lookup_pattern_parse_partial(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , struct yyu_location * restrict init_loc
  , struct yyu_location * restrict used_loc
  , struct pattern ** restrict rv
)
  __attribute__((nonnull(1, 2)));

xapi include_pattern_parse_partial(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , struct yyu_location * restrict init_loc
  , struct yyu_location * restrict used_loc
  , struct pattern ** restrict rv
)
  __attribute__((nonnull(1, 2)));

xapi match_pattern_parse_partial(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , struct yyu_location * restrict init_loc
  , struct yyu_location * restrict used_loc
  , struct pattern ** restrict rv
)
  __attribute__((nonnull(1, 2)));

#endif
