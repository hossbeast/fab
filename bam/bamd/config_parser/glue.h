/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

struct yyu_location;
struct yyu_parser;

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
static void config_yyerror(struct yyu_location * loc, void * scanner, struct yyu_parser * pp, char const * err)
  __attribute__((weakref("yyu_grammar_error")));
