/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "parser.h"

struct value_store;
struct YYLTYPE;

static void value_yyerror(struct YYLTYPE * loc, void * scanner, value_parser * parser, char const * err)
  __attribute__((weakref("yyu_grammar_error")));

static int value_set_yylex(void *, void *, void *)
  __attribute__((weakref("value_yylex")));

static void value_set_yyerror(struct YYLTYPE * loc, void * scanner, value_parser * parser, char const * err)
  __attribute__((weakref("yyu_grammar_error")));

static int value_list_yylex(void *, void *, void *)
  __attribute__((weakref("value_yylex")));

static void value_list_yyerror(struct YYLTYPE * loc, void * scanner, value_parser * parser, char const * err)
  __attribute__((weakref("yyu_grammar_error")));
