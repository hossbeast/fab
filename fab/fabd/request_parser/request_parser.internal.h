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

#ifndef _FABD_REQUEST_PARSER_INTERNAL_H
#define _FABD_REQUEST_PARSER_INTERNAL_H

#include "yyutil/parser.h"
#include "request_parser.h"

struct selector_parser;
struct config_parser;
struct command;
struct request;

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

/// request_yyerror
//
// SUMMARY
//  flag the parse as failed (by setting pp->r = 0)
//  log the error under L_ERROR | L_FF
//  log the last token under L_ERROR | L_FF
//
// DETAILS
//  called from tab.o and lex.o
//
static void request_yyerror(yyu_location* loc, void * scanner, yyu_parser * pp, char const * err)
  __attribute__((weakref("yyu_grammar_error")));

#endif
