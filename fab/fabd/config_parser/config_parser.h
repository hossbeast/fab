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

#ifndef _FABD_CONFIG_PARSER_H
#define _FABD_CONFIG_PARSER_H

#include "xapi.h"
#include "types.h"

struct yyu_location;
struct configblob;
struct config_parser;
typedef struct config_parser config_parser;

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
xapi config_parser_xfree(config_parser * restrict);

/// config_parser_ixfree
//
// SUMMARY
//  free a config parser with iwfree semantics
//
xapi config_parser_ixfree(config_parser ** restrict)
  __attribute__((nonnull));

/// config_parse
//
// SUMMARY
//  parse config text in place
//
// PARAMETERS
//  parser    - reusable parser
//  buf       - buffer containing config text
//  size      - size of buffer
//  [fname]   - filename, for error messages
//  [used]    - (returns) number of bytes parsed
//  [rv]      - (returns) parsed config object
//
// REMARKS
//  this parser uses YYU_INPLACE, so the buffer must have two terminating null bytes
//
xapi config_parser_parse(
    config_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , struct yyu_location * restrict init_loc
  , struct configblob ** restrict rv
)
  __attribute__((nonnull(1, 2)));

#endif
