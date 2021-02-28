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

#ifndef _VALUE_PARSER_H
#define _VALUE_PARSER_H

#include "types.h"
#include "xapi.h"

#include "value.h"

struct value;       // value.h
struct yyu_location;

struct value_parser;
typedef struct value_parser value_parser;

/// value_parser_create
//
// SUMMARY
//  create a config parser
//
xapi value_parser_create(value_parser ** const restrict p)
  __attribute__((nonnull));

/// value_parser_xfree
//
// SUMMARY
//  free a config parser with free semantics
//
xapi value_parser_xfree(value_parser * const restrict);

/// value_parser_ixfree
//
// SUMMARY
//  free a config parser with iwfree semantics
//
xapi value_parser_ixfree(value_parser ** const restrict)
  __attribute__((nonnull));

/// value_parser_recycle
//
xapi value_parser_recycle(value_parser * const restrict)
  __attribute__((nonnull));

/// value_parse
//
// SUMMARY
//  parse the config at the specified path
//
// PARAMETERS
//  [parser]  - (returns) reusable parser
//  text      - config text
//  len       - size of text
//  [tree]    - (returns) root of the parsed config tree
//
// REMARKS
//  not passing stor means that the parsed config tree will have been freed before this function
//  returns, and is therefore only useful to log the parse tree
//
xapi value_parser_parse(
    value_parser * restrict parser
  , char * const restrict buf
  , size_t len
  , const char * restrict fname
  , enum value_type initial_state
  , struct value ** restrict root
)
  __attribute__((nonnull(1, 2)));

xapi value_parser_parse_partial(
    value_parser * restrict parser
  , char * const restrict buf
  , size_t len
  , const char * restrict fname
  , struct yyu_location * init_loc
  , struct yyu_location * used_loc
  , enum value_type initial_state
  , struct value ** restrict root
)
  __attribute__((nonnull(1, 2)));

#endif
