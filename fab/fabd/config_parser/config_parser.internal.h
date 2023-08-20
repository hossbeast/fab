/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "yyutil/parser.h"

#include "config_parser.h"
#include "config.internal.h"

struct pattern_parser;

struct config_parser {
  yyu_parser yyu;

  // sub parsers
  struct pattern_parser * pattern_parser;

  // under construction
  struct config_filesystem_entry * fse;
  struct config_logging_section * logging_section;

  // (returns)
  configblob * cfg; // parsed config
};

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
static void config_yyerror(yyu_location * loc, void * scanner, yyu_parser * pp, char const * err)
  __attribute__((weakref("yyu_grammar_error")));
