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

#ifndef FABD_CONFIG_INTERNAL_H
#define FABD_CONFIG_INTERNAL_H

#include "types.h"
#include "xapi.h"

#include "config.h"

struct value;
struct value_parser;
struct value_store;

/// config_throw
//
// SUMMARY
//  fail with CONFIG_ILLEGAL
//
// PARAMETERS
//  error  - error, CONFIG_*
//  [val]  - throw config object
//  [path] - path from the root of the config tree
//
xapi config_throw(xapi error, struct value * restrict val, const char * restrict path);

///
//
//
//
xapi config_promote(struct value_store * store_staging, struct value * config_staging)
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
xapi config_parse(
    struct value_parser ** restrict parser
  , struct value_store ** restrict stor
  , const char * const restrict buf
  , size_t len
  , const char * restrict fname
  , struct value ** restrict root
)
  __attribute__((nonnull(3)));

#endif
