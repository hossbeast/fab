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

#ifndef GRAPH_PARSER_H
#define GRAPH_PARSER_H

#include "types.h"
#include "xapi.h"

#include "yyutil/parser.h"
#include "valyria/llist.h"

struct map;
struct hashtable;
struct attrs32;
struct moria_operations_dispatch;

#define MORIA_OPATTRS_INIT       0x1
#define MORIA_OPATTRS_INIT_SLASH 0x2

typedef struct graph_parser {
  yyu_parser graph_yyu;       // declarative graph parser
  yyu_parser operations_yyu;  // operations graph parser

  struct moria_graph * g;     // graph to parse onto
  struct moria_operations_dispatch *dispatch;

  /* during parsing */
  const struct attrs32 * vertex_defs; // definitions maps
  const struct attrs32 * edge_defs;
  llist *vertex_list;                 // vertex lookup

  struct map * vertex_map;  // vertex lookup during parsing
  llist vertices;           // active vertex list
  llist vertex_freelist;
  llist edges;              // active edge list
  llist edge_freelist;
} graph_parser;

xapi graph_parser_create(
    graph_parser ** const restrict p
  , struct moria_graph * restrict g
  , llist * restrict vertex_list
  , struct moria_operations_dispatch *dispatch
  , const struct attrs32 * restrict vertex_defs
  , const struct attrs32 * restrict edge_defs
)
  __attribute__((nonnull(1, 2, 4)));

/// graph_parser_xfree
//
// SUMMARY
//  free a config parser with free semantics
//
xapi graph_parser_xfree(graph_parser * const restrict);

/// graph_parser_ixfree
//
// SUMMARY
//  free a config parser with iwfree semantics
//
xapi graph_parser_ixfree(graph_parser ** const restrict)
  __attribute__((nonnull));

/*
 * parse a graph
 *
 * parser    - reusable parser
 * g         - graph
 * vertices  - vertex list
 * edges     - edge list
 * text      - text to parse
 * len       - size of text
 */
xapi graph_parser_parse(
    graph_parser * restrict parser
  , char * restrict text
  , size_t len
)
  __attribute__((nonnull));

xapi graph_parser_operations_parse(
    graph_parser * restrict parser
  , char * restrict text
  , size_t len
)
  __attribute__((nonnull));

#endif
