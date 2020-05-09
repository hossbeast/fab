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

#ifndef MORIA_OPERATIONS_INTERNAL_H
#define MORIA_OPERATIONS_INTERNAL_H

#include "yyutil/parser.h"
#include "operations.h"

struct dictionary;
struct list;
struct attrs32;

struct operations_parser {
  yyu_parser yyu;

  const struct attrs32 * vertex_defs;
  const struct attrs32 * edge_defs;
  struct list * li;  // sequence of operations
};

/// operations_yyerror
//
// SUMMARY
//  flag the parse as failed (by setting pp->r = 0)
//  log the error under L_ERROR
//  log the last token under L_ERROR
//
// DETAILS
//  called from tab.o and lex.o
//
static void operations_yyerror(yyu_location * loc, void * scanner, yyu_parser * pp, char const * err)
  __attribute__((weakref("yyu_grammar_error")));

void identifier_list_free(struct identifier_list * id);
void identifier_free(struct identifier * id);
void operation_free(struct operation * op);

#endif
