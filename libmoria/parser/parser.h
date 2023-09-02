/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "yyutil/parser.h"
#include "valyria/llist.h"

struct map;
struct attrs32;
struct moria_operations_vtable;
struct moria_edge;
struct moria_vertex;

#define MORIA_OPATTRS_INIT       0x1
#define MORIA_OPATTRS_INIT_SLASH 0x2

typedef struct moria_parser {
  yyu_parser graph_yyu;       // declarative graph parser
  yyu_parser operations_yyu;  // operations graph parser

  struct moria_graph *g;      // graph to parse onto
  struct moria_operations_vtable *dispatch;

  /* during parsing */
  const struct attrs32 *vertex_defs;  // definitions maps
  const struct attrs32 *edge_defs;
  llist *vertex_lists[8];             // vertex lookup lists
  uint16_t vertex_lists_len;
  struct map *vertex_by_id;           // vertex lookup by id in moria_parser_graph_parse

  /* scratch */
  llist lists;              // llist

  /* for vertices allocated by the parser, v->label is mallocd and owned by the vertex */
  llist vertices;           // active vertex list
  llist vertex_freelist;
  llist edges;              // active edge list
  llist edge_freelist;
} moria_parser;

void moria_parser_create(
    moria_parser ** const restrict p
  , struct moria_graph * restrict g
  , llist ** restrict vertex_lists            // optional
  , uint16_t vertex_lists_len
  , struct moria_operations_vtable *dispatch  // optional, operations vtable
  , const struct attrs32 * restrict vertex_defs
  , const struct attrs32 * restrict edge_defs
)
  __attribute__((nonnull(1, 2, 5)));

/* free a config parser with free semantics */
void moria_parser_free(moria_parser * const restrict);

/* free a config parser with ifree semantics */
void moria_parser_ifree(moria_parser ** const restrict)
  __attribute__((nonnull));

/* parse onto the graph from a description of its vertices and edges */
int moria_parser_graph_parse(
    moria_parser * restrict parser
  , char * restrict text
  , size_t len
)
  __attribute__((nonnull));

/* parse onto the graph from a description of operations */
int moria_parser_operations_parse(
    moria_parser * restrict parser
  , char * restrict text
  , size_t len
)
  __attribute__((nonnull));
