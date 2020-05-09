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

#ifndef _FABD_PATTERN_PARSER_INTERNAL_H
#define _FABD_PATTERN_PARSER_INTERNAL_H

#include "yyutil/parser.h"

#include "pattern_parser.h"
#include "pattern.internal.h"

struct pattern_yystype {
  yyu_lval yyu;
  pattern *pattern;
}; 

struct pattern_parser {
  yyu_parser match_yyu;
  yyu_parser generate_yyu;
  yyu_parser reference_yyu;
  yyu_parser lookup_yyu;

  // state
  uint16_t group_counter;

  // under construction
  // struct pattern_formula_show_settings show_settings;
  // struct pattern_filesystem_entry * fse;
  // struct pattern_logging_section * logging_section;

  // (returns)
  pattern * pattern;
};

static int match_pattern_yylex(void*, void*, void*)
  __attribute__((weakref("pattern_yylex")));

static int match_pattern_yyerror(yyu_location* loc, void * scanner, yyu_parser * pp, char const * err)
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

#endif
