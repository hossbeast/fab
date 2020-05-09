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

#ifndef _FABD_FORMULA_PARSER_H
#define _FABD_FORMULA_PARSER_H

#include "xapi.h"
#include "types.h"

struct set;
struct formula;

typedef struct formula_parser formula_parser;

/// formula_parser_create
//
// SUMMARY
//  create a config parser
//
xapi formula_parser_create(formula_parser ** const restrict p)
  __attribute__((nonnull));

/// formula_parser_free
//
// SUMMARY
//  free a config parser with free semantics
//
xapi formula_parser_xfree(formula_parser * restrict);

/// formula_parser_ifree
//
// SUMMARY
//  free a config parser with iwfree semantics
//
xapi formula_parser_ixfree(formula_parser ** restrict)
  __attribute__((nonnull));

/// config_parse
//
// SUMMARY
//  parse the fabfile at the specified path
//
// PARAMETERS
//  parser  - reusable parser
//  text    - config text
//  len     - size of text
//  vars    - (returns) set of parsed vars
//
// REMARKS
//  not passing stor means that the parsed config tree will have been freed before this function
//  returns, and is therefore only useful to log the parse tree
//
xapi formula_parser_parse(
    formula_parser * restrict parser
  , char * const restrict buf
  , size_t len
  , const char * restrict fname
  , struct formula * restrict fml
)
  __attribute__((nonnull(1, 2, 5)));

#endif
