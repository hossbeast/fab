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

#ifndef _FABD_REQUEST_PARSER_H
#define _FABD_REQUEST_PARSER_H

#include "types.h"
#include "yyutil/parser.h"

struct request;
struct selector_parser;
struct config_parser;
struct command;

typedef struct request_parser {
  yyu_parser yyu;

  // sub-parsers
  struct selector_parser * selector_parser;
  struct config_parser * config_parser;

  // under construction
  struct command * command;

  // (returns)
  struct request * request;
} request_parser;

/// request_parser_create
//
// SUMMARY
//  create a config parser
//
void request_parser_create(request_parser ** const restrict p)
  __attribute__((nonnull));

/// request_parser_xfree
//
// SUMMARY
//  free a config parser with free semantics
//
void request_parser_xfree(request_parser * restrict);

/// request_parser_ixfree
//
// SUMMARY
//  free a config parser with iwfree semantics
//
void request_parser_ixfree(request_parser ** restrict)
  __attribute__((nonnull));

/*
 * parse a request
 *
 *  parser    - reusable parser
 *  text      - config text
 *  len       - size of text
 *  req       - request to return the result in
 */
int request_parser_parse(
    request_parser * restrict parser
  , char * const restrict buf
  , size_t len
  , const char * restrict fname
  , struct request * restrict req
)
  __attribute__((nonnull(1, 2)));

#endif
