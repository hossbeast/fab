/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "yyutil/parser.h"

#include "pattern_parser.h"
#include "pattern.internal.h"

struct pattern_yystype {
  yyu_lval yyu;
  pattern *pattern;
};

struct pattern_parser {
  yyu_parser search_yyu;
  yyu_parser generate_yyu;
  yyu_parser reference_yyu;
  yyu_parser lookup_yyu;
  yyu_parser include_yyu;
  yyu_parser match_yyu;

  // state
  uint16_t group_counter;

  // (returns)
  pattern * pattern;
};

static int search_pattern_yylex(void*, void*, void*)
  __attribute__((weakref("pattern_yylex")));

static int search_pattern_yyerror(yyu_location* loc, void * scanner, yyu_parser * pp, char const * err)
  __attribute__((weakref("yyu_grammar_error")));

static int generate_pattern_yylex(void*, void*, void*)
  __attribute__((weakref("pattern_yylex")));

static int generate_pattern_yyerror(yyu_location* loc, void * scanner, yyu_parser * pp, char const * err)
  __attribute__((weakref("yyu_grammar_error")));

static int reference_pattern_yylex(void*, void*, void*)
  __attribute__((weakref("pattern_yylex")));

static int reference_pattern_yyerror(yyu_location* loc, void * scanner, yyu_parser * pp, char const * err)
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
