/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "yyutil/parser.h"

#include "pattern_parser.h"
#include "pattern.internal.h"

struct pattern_yystype {
  yyu_lval yyu;
  struct pattern *pattern;
};

struct pattern_parser {
  yyu_parser search_yyu;
  yyu_parser generate_yyu;
  yyu_parser attachment_yyu;
  yyu_parser lookup_yyu;
  yyu_parser include_yyu;
  yyu_parser match_yyu;
  yyu_parser path_yyu;

  // state
  yyu_parser *yyu;
  uint16_t group_counter;
  uint16_t capture_group_counter;
  uint32_t section_type_qualifiers_mask;  // for the current section
  uint32_t group_type_qualifiers_mask;    // for the current group

  // (returns)
  struct pattern *pattern;
  const char *error;
  uint16_t error_len;
};

static int search_pattern_yylex(void*, void*, void*)
  __attribute__((weakref("pattern_yylex")));

static int search_pattern_yyerror(yyu_location* loc, void * scanner, yyu_parser * pp, char const * err)
  __attribute__((weakref("yyu_grammar_error")));

static int generate_pattern_yylex(void*, void*, void*)
  __attribute__((weakref("pattern_yylex")));

static int generate_pattern_yyerror(yyu_location* loc, void * scanner, yyu_parser * pp, char const * err)
  __attribute__((weakref("yyu_grammar_error")));

static int attachment_pattern_yylex(void*, void*, void*)
  __attribute__((weakref("pattern_yylex")));

static int attachment_pattern_yyerror(yyu_location* loc, void * scanner, yyu_parser * pp, char const * err)
  __attribute__((weakref("yyu_grammar_error")));

static int lookup_pattern_yylex(void*, void*, void*)
  __attribute__((weakref("pattern_yylex")));

static int lookup_pattern_yyerror(yyu_location* loc, void * scanner, yyu_parser * pp, char const * err)
  __attribute__((weakref("yyu_grammar_error")));

static int include_pattern_yylex(void*, void*, void*)
  __attribute__((weakref("pattern_yylex")));

static int include_pattern_yyerror(yyu_location* loc, void * scanner, yyu_parser * pp, char const * err)
  __attribute__((weakref("yyu_grammar_error")));

static int match_pattern_yylex(void*, void*, void*)
  __attribute__((weakref("pattern_yylex")));

static int match_pattern_yyerror(yyu_location* loc, void * scanner, yyu_parser * pp, char const * err)
  __attribute__((weakref("yyu_grammar_error")));

static int path_pattern_yylex(void*, void*, void*)
  __attribute__((weakref("pattern_yylex")));

static int path_pattern_yyerror(yyu_location* loc, void * scanner, yyu_parser * pp, char const * err)
  __attribute__((weakref("yyu_grammar_error")));
