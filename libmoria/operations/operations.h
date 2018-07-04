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

struct operations_parser;
typedef struct operations_parser operations_parser;

#define GRAPH_OPERATION_TABLE                               \
  GRAPH_OPERATION_DEF(GRAPH_OPERATION_VERTEX)       /*   */ \
  GRAPH_OPERATION_DEF(GRAPH_OPERATION_CONNECT)      /* + */ \
  GRAPH_OPERATION_DEF(GRAPH_OPERATION_DISCONNECT)   /* - */ \
  GRAPH_OPERATION_DEF(GRAPH_OPERATION_DISINTEGRATE) /* ~ */ \

typedef enum operation_type {
  GRAPH_OPERATION_RANGE_BEFORE = 0,

#define GRAPH_OPERATION_DEF(x) x,
GRAPH_OPERATION_TABLE
#undef GRAPH_OPERATION_DEF

  GRAPH_OPERATION_RANGE_AFTER
} operation_type;

/// identifier
//
// represents a vertex
//
typedef struct identifier {
  struct identifier * next;   // next in the sequence
  uint32_t attrs;
  uint16_t label_len;
  char label[];
} identifier;

/// operation
//
// represents an operation on a graph
//
typedef struct operation {
  operation_type type;
  uint32_t attrs;
  identifier * A;
  identifier * B;
} operation;

typedef struct operations_dispatch {
  xapi (*connect)(struct graph * restrict g, struct vertex * restrict A, struct vertex * restrict B, uint32_t attrs)
    __attribute__((nonnull));
  xapi (*disconnect)(struct graph * restrict g, struct vertex * restrict A, struct vertex * restrict B)
    __attribute__((nonnull));
  xapi (*vertex_create)(struct vertex ** restrict v, struct graph * restrict g, uint32_t attrs, const char * restrict label, uint16_t label_len)
    __attribute__((nonnull));
} operations_dispatch;

operations_dispatch * graph_operations_dispatch;

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

/// operations_parser_parse
//
// SUMMARY
//  parse a specification of a series of operations
//
// PARAMETERS
//  [parser]     - (returns) reusable parser
//  text         - config text
//  len          - size of text
//  [operations] - (returns) sequence of operations
//
// REMARKS
//  not passing stor means that the parsed config tree will have been freed before this function
//  returns, and is therefore only useful to log the parse tree
//
xapi operations_parser_parse(
    operations_parser ** restrict parser
  , const char * restrict buf
  , size_t len
  , struct list ** restrict operations
)
  __attribute__((nonnull(2)));

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
  , const operations_dispatch * restrict dispatch
  , struct list * restrict operations
)
  __attribute__((nonnull));

/// graph_connect
//
// SUMMARY
//  create the edge A : B with the specified attributes
//
// REMARKS
//  A : B, e.g. A needs B, e.g. A depends on B, e.g. A is up from B, B is down from A
//
// PARAMETERS
//  g     - graph
//  A     - vertex
//  B     - vertex
//  attrs - properties of the edge
//
// THROWS
//  EXISTS
//
xapi graph_connect(struct graph * const restrict g, struct vertex * A, struct vertex * B, uint32_t attrs)
  __attribute__((nonnull));

xapi graph_disconnect(struct graph * const restrict g, struct vertex * A, struct vertex * B)
  __attribute__((nonnull));

typedef struct graph_disintegrate_context {
  void * uctx;
} graph_disintegrate_context;

xapi operations_disintegrate_visitor(struct edge * restrict e, int distance, void * restrict context)
  __attribute__((nonnull));

/// graph_disintegrate
//
// SUMMARY
//  remove all edges in a subgraph from the graph
//
// PARAMETERS
//  g              - 
//  e              - 
//  [criteria]     - 
//  attrs          - 
//  [traversal_id] - 
//  [visitor]      - 
//  [ctx]          - optional user context
//
xapi graph_disintegrate(
    /* 1 */ struct graph * restrict g
  , /* 2 */ struct edge * restrict e
  , /* 3 */ xapi (* visitor)(struct edge * restrict e, int distance, void * restrict ctx) __attribute__((nonnull))
  , /* 4 */ int traversal_id
  , /* 5 */ const traversal_criteria * restrict criteria
  , /* 6 */ uint32_t attrs
  , /* 7 */ void * ctx
)
  __attribute__((nonnull(1, 2)));

#endif
