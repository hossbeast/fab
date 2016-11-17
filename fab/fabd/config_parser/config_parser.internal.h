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

#ifndef _CONFIG_PARSER_INTERNAL_H
#define _CONFIG_PARSER_INTERNAL_H

#ifndef YYU_EXTRA_TYPE
 struct config_extra;
 #define YYU_EXTRA_TYPE struct config_extra
#endif
#include "yyutil/parser.h"
#include "config_parser.h"

typedef struct config_xtra
{
  /* yyu-defined xtra fields */
  union {
    yyu_extra;
    yyu_extra yyu;
  };

  struct value_store * stor; // value store
  struct value *      root; // (returns) root of the parsed tree
} config_xtra;

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
static void config_yyerror(struct YYLTYPE * loc, void * scanner, config_xtra * pp, char const * err)
  __attribute__((weakref("yyu_grammar_error")));

#endif
