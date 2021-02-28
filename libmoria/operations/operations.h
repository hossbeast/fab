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

struct moria_graph;
struct moria_vertex;
struct moria_edge;
struct list;
struct multimap;

struct operations_parser;
typedef struct operations_parser operations_parser;

struct graph_parser;

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

/// operation_type_name
//
// SUMMARY
//  get the name of an operation as a string
//
const char * operation_type_name(operation_type type);

typedef struct moria_operations_dispatch {
  xapi (*connect)(
      struct graph_parser * restrict parser
    , struct moria_vertex * restrict A
    , struct moria_vertex * restrict B
    , uint32_t attrs
    , struct moria_edge ** restrict e
  )
    __attribute__((nonnull(1, 2, 3)));

  xapi (*hyperconnect)(
      struct graph_parser * restrict parser
    , struct moria_vertex ** restrict Alist
    , uint16_t Alen
    , struct moria_vertex ** restrict Blist
    , uint16_t Blen
    , uint32_t attrs
    , struct moria_edge ** restrict e
  )
    __attribute__((nonnull(1)));

  xapi (*disconnect)(
      struct graph_parser * restrict parser
    , struct moria_edge * restrict e
  )
    __attribute__((nonnull));

  xapi (*create_vertex)(
      struct graph_parser * restrict parser
    , struct moria_vertex ** restrict v
    , uint32_t attrs
    , uint8_t opattrs
    , const char * restrict label
    , uint16_t label_len
  )
    __attribute__((nonnull));

  xapi (*invalidate_vertex)(
      struct graph_parser * restrict parser
    , struct moria_vertex * restrict v
  )
    __attribute__((nonnull));

  xapi (*refresh_vertex)(
      struct graph_parser * restrict parser
    , struct moria_vertex * restrict v
  )
    __attribute__((nonnull));

  /* called at the start of operations_perform */
  xapi (*setup)(struct graph_parser * restrict)
    __attribute__((nonnull));

  /* called at the end of operations_perform */
  xapi (*cleanup)(struct graph_parser * restrict)
    __attribute__((nonnull));
} moria_operations_dispatch;


#if 0
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
  , struct moria_graph * restrict g
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
    struct moria_graph * restrict g
  , struct multimap * restrict mm
  , struct operations_dispatch * restrict dispatch
  , struct list * restrict operations
)
  __attribute__((nonnull));
#endif

#endif

#if 0

xapi graph_vertex_delete(struct moria_graph * restrict g, struct moria_vertex * restrict v)
  __attribute__((nonnull));

#endif
