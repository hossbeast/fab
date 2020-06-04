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

#ifndef MORIA_OPERATIONS_H
#define MORIA_OPERATIONS_H

/*

MODULE
 graph/operations

SUMMARY
 mutating graph operations

*/

#include "types.h"
#include "xapi.h"

#include "traverse.h"

struct graph;
struct list;
struct vertex;
struct edge;
enum traversal_mode;

struct operations_parser;
typedef struct operations_parser operations_parser;

#define GRAPH_OPERATION_TABLE                               \
  DEF(GRAPH_OPERATION_VERTEX)       /*   */ \
  DEF(GRAPH_OPERATION_CONNECT)      /* + */ \
  DEF(GRAPH_OPERATION_DISCONNECT)   /* - */ \
  DEF(GRAPH_OPERATION_REFRESH)      /* @ */ \
  DEF(GRAPH_OPERATION_INVALIDATE)   /* ~ */ \

typedef enum operation_type {
  GRAPH_OPERATION_RANGE_BEFORE = 0,

#undef DEF
#define DEF(x) x,
GRAPH_OPERATION_TABLE

  GRAPH_OPERATION_RANGE_AFTER
} operation_type;

#define MORIA_OPATTRS_INIT       0x1
#define MORIA_OPATTRS_INIT_SLASH 0x2

/// identifier
//
// represents a vertex
//
typedef struct identifier {
  struct identifier * next;   // next in the sequence
  uint8_t op_attrs;
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
  operation_type type;
  uint32_t attrs;
  identifier_list * A;
  identifier_list * B;
} operation;

typedef struct operations_dispatch {
  xapi (*connect)(struct graph * restrict g, struct vertex * restrict A, struct vertex * restrict B, uint32_t attrs, struct edge ** restrict e, bool * restrict r)
    __attribute__((nonnull(1, 2, 3)));
  xapi (*hyperconnect)(struct graph * restrict g, struct vertex ** restrict Alist, uint16_t Alen, struct vertex ** restrict B, uint16_t Blen, uint32_t attrs, struct edge ** restrict e, bool * restrict r)
    __attribute__((nonnull(1)));
  xapi (*disconnect)(struct graph * restrict g, struct edge * restrict e)
    __attribute__((nonnull));
  xapi (*create_vertex)(struct vertex ** restrict v, struct graph * restrict g, uint32_t attrs, uint8_t op_attrs, const char * restrict label, uint16_t label_len)
    __attribute__((nonnull));
  xapi (*invalidate_vertex)(struct graph * restrict g, struct vertex * restrict v)
    __attribute__((nonnull));
  xapi (*refresh_vertex)(struct graph * restrict g, struct vertex * restrict v)
    __attribute__((nonnull));

  /* called at the start of operations_perform */
  xapi (*setup)(struct operations_dispatch * restrict dispatch, struct graph * restrict g);

  /* called at the end of operations_perform */
  xapi (*cleanup)(struct operations_dispatch * restrict dispatch, struct graph * restrict g);
} operations_dispatch;

extern operations_dispatch * graph_operations_dispatch;

/// operation_type_name
//
// SUMMARY
//  get the name of an operation as a string
//
const char * operation_type_name(operation_type type);

/// operations_parser_create
//
// SUMMARY
//  create an operations parser
//
xapi operations_parser_create(operations_parser ** const restrict p)
  __attribute__((nonnull));

/// operations_parser_xfree
//
// SUMMARY
//  free an operations parser with free semantics
//
xapi operations_parser_xfree(operations_parser * const restrict);

/// operations_parser_ixfree
//
// SUMMARY
//  free an operations parser with iwfree semantics
//
xapi operations_parser_ixfree(operations_parser ** const restrict)
  __attribute__((nonnull));

xapi operations_parser_operations_create(struct list ** restrict operations)
  __attribute__((nonnull));

/// operations_parser_parse
//
// SUMMARY
//  parse a specification of a series of operations
//
// PARAMETERS
//  parser     - (returns) reusable parser
//  operations - (returns) sequence of operations
//  text       - config text
//  len        - size of text
//
// REMARKS
//  not passing stor means that the parsed config tree will have been freed before this function
//  returns, and is therefore only useful to log the parse tree
//
xapi operations_parser_parse(
    operations_parser * restrict parser
  , struct graph * restrict g
  , char * restrict text
  , size_t len
  , struct list * restrict operations
)
  __attribute__((nonnull));

/// operations_perform
//
// SUMMARY
//  Mutate a graph by carrying out a series of operations, in order.
//
// REMARKS
//  If operations contain any CONNECT operations, and if any of the identifiers in the vertex
//  specifications contain multiple labels, then it is expected that the graph contains an identity
//  subgraph whose edges contain the bits in the identity mask.
//
// PARAMETERS
//  operations - sequence of operations to perform, in order
//  g          - graph to operate on
//
xapi operations_perform(
    struct graph * restrict g
  , operations_dispatch * restrict dispatch
  , struct list * restrict operations
)
  __attribute__((nonnull));

/// graph_connect
//
// SUMMARY
//  create the edge A : B with the specified attributes (idempotent)
//
// REMARKS
//  A : B, e.g. A needs B, e.g. A depends on B, e.g. A is up from B, B is down from A
//
// PARAMETERS
//  g     - graph
//  A     - vertex
//  B     - vertex
//  attrs - properties of the edge
//  [e]   - (returns) the edge
//  [r]   - (returns) true if the edge is newly created
//
// THROWS
//  EXISTS
//
xapi graph_connect(
    struct graph * const restrict g
  , struct vertex * A
  , struct vertex * B
  , uint32_t attrs
  , struct edge ** restrict e
  , bool * restrict r
)
  __attribute__((nonnull(1, 2, 3)));

/*
 * as for graph_connect, except that if the edge already exists, replace the existing B
 */
xapi graph_connect_replace(
    struct graph * const restrict g
  , struct vertex * A
  , struct vertex * B
  , uint32_t attrs
  , struct edge ** restrict e
  , bool * restrict r
  , struct vertex ** oldB
)
  __attribute__((nonnull(1, 2, 3)));

/// graph_hyperconnect
//
// create the edge [ Alist ] : [ Blist ] with the specified attributes (idempotent)
//
xapi graph_hyperconnect(
    struct graph * const restrict g
  , struct vertex ** Alist
  , uint16_t Alen
  , struct vertex ** Blist
  , uint16_t Blen
  , uint32_t attrs
  , struct edge ** restrict e
  , bool * restrict r
)
  __attribute__((nonnull(1)));

/// edge_disconnect
//
// SUMMARY
//  remove the edge A : B
//
// PARAMETERS
//  g - graph
//  e - edge to remove
//
xapi graph_disconnect(struct graph * const restrict g, struct vertex * A, struct vertex * B)
  __attribute__((nonnull));

xapi graph_hyperdisconnect(struct graph * restrict g, struct vertex ** Alist, uint16_t Alen, struct vertex ** Blist, uint16_t Blen)
  __attribute__((nonnull));

xapi graph_edge_disconnect(struct graph * const restrict g, struct edge * restrict e)
  __attribute__((nonnull));

xapi graph_vertex_delete(struct graph * restrict g, struct vertex * restrict v)
  __attribute__((nonnull));

#endif
