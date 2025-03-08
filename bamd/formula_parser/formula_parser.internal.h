/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "yyutil/parser.h"
#include "formula_parser.h"

struct selector_parser;
struct pattern_parser;
struct bacon_value;

typedef struct formula_parser {
  yyu_parser yyu;

  // sub-parsers
  struct selector_parser *selector_parser;
  struct pattern_parser *pattern_parser;
  struct bacon_parser *bacon_parser;

  // under construction
  struct formula_operation *set;

  // (returns)
  struct bacon_value *file;  // file to execute
  struct bacon_value *args;  // list
  struct bacon_value *envs;  // set of mappings

  const char *error;
  uint16_t error_len;
} formula_parser;

/* called from tab.o and lex.o */
static void formula_yyerror(yyu_location * loc, void * scanner, yyu_parser * pp, char const * err)
  __attribute__((weakref("yyu_grammar_error")));

/* locate the embedded bacon block in formula text, if any */
void formula_parser_extract(
    const char * restrict text
  , size_t text_len
  , bool * restrict shebang
  , int * restrict bacon_line
  , char ** restrict baconp
  , size_t * restrict bacon_lenp
)
  __attribute__((nonnull));

int formula_parser_bacon_parse(
    /* 1 */ formula_parser * restrict parser
  , /* 2 */ char * const restrict vars_text
  , /* 3 */ size_t vars_text_len
  , /* 4 */ const char * restrict fname
  , /* 5 */ int line
  , /* 6 */ struct bacon_value ** restrict path
  , /* 7 */ struct bacon_value ** restrict args
  , /* 8 */ struct bacon_value ** restrict env
)
  __attribute__((nonnull(1, 2, 6, 7, 8)));
