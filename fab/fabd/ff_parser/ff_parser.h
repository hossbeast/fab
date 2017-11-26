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

#ifndef _FF_PARSER_H
#define _FF_PARSER_H

#include "xapi.h"
#include "types.h"

struct ff_node;

typedef struct ff_parser ff_parser;

// cref bytes by representative character
extern const char * ff_cref_byte_table;
static inline uint8_t ff_cref_byte(char c)
{
  return ff_cref_byte_table[(uint8_t)c];
}

// cref representative character by byte
extern const char * ff_cref_char_table;
static inline char ff_cref_char(uint8_t b)
{
  return ff_cref_char_table[b];
}

/// ff_parser_create
//
// SUMMARY
//  create a config parser
//
xapi ff_parser_create(ff_parser ** const restrict p)
  __attribute__((nonnull));

/// ff_parser_free
//
// SUMMARY
//  free a config parser with free semantics
//
void ff_parser_free(ff_parser * restrict);

/// ff_parser_ifree
//
// SUMMARY
//  free a config parser with iwfree semantics
//
void ff_parser_ifree(ff_parser ** restrict)
  __attribute__((nonnull));

/// ff_parse
//
// SUMMARY
//  parse the fabfile at the specified path
//
// PARAMETERS
//  [parser]  - (returns) reusable parser
//  [stor]    - (returns) value storage for the config tree
//  text      - config text
//  len       - size of text
//  [root]    - (returns) root of the parse tree
//
// REMARKS
//  not passing stor means that the parsed config tree will have been freed before this function
//  returns, and is therefore only useful to log the parse tree
//
xapi ff_parser_parse(
    ff_parser ** restrict parser
  , const char * const restrict buf
  , size_t len
  , const char * restrict fname
  , struct ff_node ** restrict root
);

#endif
