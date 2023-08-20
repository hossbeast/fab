/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "selector_parser.h"
#include "valyria/llist.h"
#include "yyutil/parser.h"

struct pattern_parser;

struct selector_parser {
  yyu_parser yyu;

  // stack of selectors under construction
  llist selector_stack;

  /* sub parsers */
  struct pattern_parser * pattern_parser;
};

/// selector_yyerror
//
// SUMMARY
//  flag the parse as failed (by setting pp->r = 0)
//  log the error under L_ERROR | L_FF
//  log the last token under L_ERROR | L_FF
//
// DETAILS
//  called from tab.o and lex.o
//
static void selector_yyerror(yyu_location* loc, void * scanner, yyu_parser * pp, char const * err)
  __attribute__((weakref("yyu_grammar_error")));
