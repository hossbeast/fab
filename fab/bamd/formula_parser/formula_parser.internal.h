/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

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
