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

/*

This should probably be internal only, because vertices are allocated with their value as a pointer
to their label, so not useful for e.g. fabd in which vertices are allocated with fabd-node as value.

*/

struct graph;
struct graph_parser;
typedef struct graph_parser graph_parser;

/// graph_parser_create
//
// SUMMARY
//  create a config parser
//
xapi graph_parser_create(graph_parser ** const restrict p)
  __attribute__((nonnull));

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

/// graph_parser_parse
//
// SUMMARY
//  parse the graph
//
// PARAMETERS
//  [parser]  - (returns) reusable parser
//  text      - config text
//  len       - size of text
//  identity  - edges identity mask
//  [g]       - (returns) parsed graph
//
xapi graph_parser_parse(
    graph_parser * restrict parser
  , struct graph * restrict g
  , char * restrict buf
  , size_t len
)
  __attribute__((nonnull));

#endif
