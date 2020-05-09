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

#ifndef GRAPH_PARSER_INTERNAL_H
#define GRAPH_PARSER_INTERNAL_H

#include "xapi.h"
#include "types.h"

#include "yyutil/parser.h"
#include "parser.h"

struct graph;
struct vertex;
struct attrs32;

struct graph_yystype {
  yyu_lval yyu;
  uint32_t u32;
  struct vertex * vertex;
  char * label;
};

struct graph_parser {
  yyu_parser yyu;

  // definitions map during parsing
  const struct attrs32 * vertex_defs;
  const struct attrs32 * edge_defs;

  // vertex lookup during parsing
  struct map * vertex_map;

  struct graph * g;  // (returns) parsed graph
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
static void graph_yyerror(yyu_location * loc, void * scanner, graph_parser * pp, char const * err)
  __attribute__((weakref("yyu_grammar_error")));

/// graph_parser_graph_create
//
// SUMMARY
//  g - (returns) graph suitable for use with graph_parser_parse
//
xapi graph_parser_graph_create(struct graph ** restrict g, uint32_t identity)
  __attribute__((nonnull));

#endif
