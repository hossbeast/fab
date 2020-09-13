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

#ifndef _FABD_CONFIG_PARSER_INTERNAL_H
#define _FABD_CONFIG_PARSER_INTERNAL_H

#include "yyutil/parser.h"

#include "config_parser.h"
#include "config.internal.h"

struct box_store;
struct value_parser;

struct config_parser {
  yyu_parser yyu;

  // sub-parsers
  struct value_parser * value_parser;

  // under construction
  struct config_filesystem_entry * fse;
  struct config_logging_section * logging_section;

  // (returns)
  config * cfg; // parsed config
};

/// config_yyerror
//
// SUMMARY
//  flag the parse as failed (by setting pp->r = 0)
//  log the error under L_ERROR | L_FF
//  log the last token under L_ERROR | L_FF
//
// DETAILS
//  called from tab.o and lex.o
//
static void config_yyerror(yyu_location * loc, void * scanner, yyu_parser * pp, char const * err)
  __attribute__((weakref("yyu_grammar_error")));

#endif
