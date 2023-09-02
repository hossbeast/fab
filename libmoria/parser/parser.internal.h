/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "parser.h"

struct moria_vertex;

struct graph_yystype {
  yyu_lval yyu;
  uint32_t u32;
  struct moria_vertex * vertex;
  char * label;
};

/// graph_yyerror
//
// SUMMARY
//  flag the parse as failed (by setting pp->r = 0)
//  log the error under L_ERROR | L_FF
//  log the last token under L_ERROR | L_FF
//
// DETAILS
//  called from tab.o and lex.o
//
static void graph_yyerror(yyu_location * loc, void * scanner, moria_parser * pp, char const * err)
  __attribute__((weakref("yyu_grammar_error")));

static void operations_yyerror(yyu_location * loc, void * scanner, moria_parser * pp, char const * err)
  __attribute__((weakref("yyu_grammar_error")));
