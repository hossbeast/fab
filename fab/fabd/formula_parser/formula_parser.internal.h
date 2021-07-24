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

#ifndef _FABD_FORMULA_PARSER_INTERNAL_H
#define _FABD_FORMULA_PARSER_INTERNAL_H

#include "types.h"

#include "yyutil/parser.h"
#include "formula_parser.h"

struct selector_parser;
struct formula_value;

typedef struct formula_parser {
  yyu_parser yyu;

  // sub-parsers
  struct selector_parser * selector_parser;

  // under construction
  struct formula_operation * set;

  // (returns)
  struct formula_value * file;  // primitive
  struct formula_value * args;  // list
  struct formula_value * envs;  // set of mappings
} formula_parser;

/* called from tab.o and lex.o */
static void formula_yyerror(yyu_location * loc, void * scanner, yyu_parser * pp, char const * err)
  __attribute__((weakref("yyu_grammar_error")));

/* locate the embedded bacon block in formula text, if any */
void formula_parser_extract(
    const char * restrict text
  , size_t text_len
  , bool * restrict shebang
  , char ** restrict baconp
  , size_t * restrict bacon_lenp
)
  __attribute__((nonnull));

int formula_parser_bacon_parse(
    formula_parser * restrict parser
  , char * const restrict vars_text
  , size_t vars_text_len
  , const char * restrict fname
  , struct formula_value ** restrict path
  , struct formula_value ** restrict args
  , struct formula_value ** restrict env
)
  __attribute__((nonnull(1, 2, 5, 6, 7)));

#endif
