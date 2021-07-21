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

#include "types.h"

#include "parser.h"

struct moria_graph;
struct moria_vertex;
struct moria_edge;
struct attrs32;
struct moria_operations_dispatch;

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
static void graph_yyerror(yyu_location * loc, void * scanner, graph_parser * pp, char const * err)
  __attribute__((weakref("yyu_grammar_error")));

static void operations_yyerror(yyu_location * loc, void * scanner, graph_parser * pp, char const * err)
  __attribute__((weakref("yyu_grammar_error")));

/// identifier
//
// represents a vertex
//
typedef struct identifier {
  struct identifier * next;   // next in the sequence
  uint8_t opattrs;
  uint32_t attrs;
  uint16_t label_len;
  char label[];
} identifier;

typedef struct identifier_list {
  identifier *v0;
  identifier **v;
  uint16_t len;
} identifier_list;

/// operation
//
// represents an operation on a graph
//
typedef struct operation {
//  operation_type type;
  uint32_t attrs;
  identifier_list * A;
  identifier_list * B;
} operation;

void identifier_list_free(struct identifier_list * id);
void identifier_free(struct identifier * id);
void operation_free(struct operation * op);

/* operations parser */
void operation_vertex(graph_parser * restrict parser, identifier_list *A);
void operation_refresh(graph_parser * restrict parser, identifier_list * restrict A);
void operation_invalidate(graph_parser * restrict parser, identifier_list * restrict A);
void operation_connect(graph_parser * restrict parser, identifier_list * restrict A, identifier_list * restrict B, uint32_t attrs);
void operation_disconnect(graph_parser * restrict parser, identifier_list * restrict A, identifier_list * restrict B);

/* graph parser */
void graph_parser_create_vertex(
    graph_parser * restrict p
  , struct moria_vertex ** const restrict rv
  , uint32_t attrs
  , uint8_t opattrs
  , const char * const restrict label
  , uint16_t label_len
)
  __attribute__((nonnull));

void graph_parser_create_edge(
    graph_parser * restrict p
  , struct moria_edge ** const restrict er
  , uint16_t Alen
  , uint16_t Blen
)
  __attribute__((nonnull));

void graph_parser_connect(
    graph_parser * restrict p
  , struct moria_vertex * restrict A
  , struct moria_vertex * restrict B
  , uint32_t attrs
  , struct moria_edge ** restrict e
)
  __attribute__((nonnull(1, 2, 3)));

void graph_parser_hyperconnect(
    graph_parser * restrict p
  , struct moria_vertex ** restrict Alist
  , uint16_t Alen
  , struct moria_vertex ** restrict Blist
  , uint16_t Blen
  , uint32_t attrs
  , struct moria_edge ** restrict e
)
  __attribute__((nonnull(1)));

extern struct moria_operations_dispatch * graph_operations_dispatch;

#endif
