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

#ifndef _CONFIG_PARSER_H
#define _CONFIG_PARSER_H

#include "xapi.h"

struct value; // libvalue
struct value_store; // libavlue

struct config_parser;
typedef struct config_parser config_parser;

#define restrict __restrict

/// config_parser_create
//
// SUMMARY
//  create a config parser
//
xapi config_parser_create(config_parser ** const restrict p)
  __attribute__((nonnull));

/// config_parser_xfree
//
// SUMMARY
//  free a config parser with free semantics
//
xapi config_parser_xfree(config_parser * const restrict);

/// config_parser_ixfree
//
// SUMMARY
//  free a config parser with iwfree semantics
//
xapi config_parser_ixfree(config_parser ** const restrict)
  __attribute__((nonnull));

/// config_parse
//
// SUMMARY
//  parse the config at the specified path
//
// PARAMETERS
//  [parser]  - (returns) reusable parser
//  [stor]    - (returns) value storage for the config tree
//  text      - config text
//  len       - size of text
//  [tree]    - (returns) root of the parsed config tree
//
// REMARKS
//  not passing stor means that the parsed config tree will have been freed before this function
//  returns, and is therefore only useful to log the parse tree
//
xapi config_parser_parse(
    config_parser ** restrict parser
  , struct value_store ** restrict stor
  , const char * const restrict buf
  , size_t len
  , struct value ** restrict root
)
  __attribute__((nonnull(3)));

#undef restrict
#endif
