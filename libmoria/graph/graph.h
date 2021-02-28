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

#ifndef _MORIA_GRAPH_H
#define _MORIA_GRAPH_H

/*
An identity graph is a tree where each vertex is uniquely identified by its path in the upward
direction to another vertex called the root. For example, a filesystem tree is an identity graph
with a root vertex at '/'.

hyperedges cannot be part of the identity graph
*/

#include "xapi.h"
#include "types.h"

#include "valyria/rbtree.h"

struct attrs32;
struct hashtable;    // valyria/hashtable
struct narrator;
struct moria_vertex;
struct llist;
struct moria_graph;

/*
 * create a graph
 *
 * g           - (returns) graph
 * [identity]  - nonzero to enable identity operations
 * [vsz]       - size of vertex udata
 * [vertex_value_destroy] - invoked on vertex user data before releasing
 * [edge_value_destroy] - invoked on vertex user data before releasing
 */
xapi moria_graph_create(struct moria_graph ** const restrict g)
  __attribute__((nonnull));

void moria_graph_init(struct moria_graph * const restrict g)
  __attribute__((nonnull));

/*
 * [defs] - name/value mappings for parse and say
 */
void moria_graph_vertex_definitions_set(struct moria_graph * restrict g, const struct attrs32 * restrict defs)
  __attribute__((nonnull));

/*
 * [defs] - name/value mappings for parse and say
 */
void moria_graph_edge_definitions_set(struct moria_graph * restrict g, const struct attrs32 * restrict defs)
  __attribute__((nonnull));

void moria_graph_destroy(struct moria_graph * const restrict g)
  __attribute__((nonnull));

/// graph_xfree
//
// SUMMARY
//  free a graph
//
xapi moria_graph_xfree(struct moria_graph * const restrict g);

/// graph_ifree
//
// SUMMARY
//  free a graph, zero its reference
//
xapi moria_graph_ixfree(struct moria_graph ** const restrict g)
  __attribute__((nonnull));

/// graph_say
//
// SUMMARY
//  write the normal representation of a graph to a narrator
//
// PARAMETERS
//  g   - graph
//  [d] - attrs name lookup
//  N   - narrator
//
xapi moria_graph_say(
  /* 1 */   struct moria_graph * const restrict g
  /* 2 */ , struct llist ** restrict vertex_lists
  /* 3 */ , uint16_t vertex_lists_len
  /* 4 */ , struct llist ** restrict edge_lists
  /* 5 */ , uint16_t edge_lists_len
  /* 6 */ , const struct attrs32 * vertex_defs
  /* 7 */ , const struct attrs32 * edge_defs
  /* 8 */ , struct narrator * const restrict N
)
  __attribute__((nonnull(1, 2, 4, 8)));

/// graph_lookup_identifier_callback
//
// SUMMARY
//  returns the next label in the lookup sequence, or resets the lookup sequence if label == NULL
//
typedef xapi (*moria_graph_lookup_identifier_callback)(
    void * context                  // user context
  , const char ** restrict label    // (returns) next label
  , uint16_t * restrict label_len   // (returns) next label length
);

typedef struct moria_vertex_entry {
  const char *label;
  uint16_t label_len;
  rbtree rbt;
} moria_vertex_entry;

int moria_vertex_entry_key_cmp(const void *A, const void *key, size_t sz)
  __attribute__((nonnull));

uint32_t moria_vertex_entry_hash(uint32_t h, const void *v, size_t sz)
  __attribute__((nonnull));

int moria_vertex_entry_cmp(const void *A, size_t Asz, const void *B, size_t Bsz)
  __attribute__((nonnull));

/// graph_lookup
//
// SUMMARY
//  find a vertex in the identity tree
//
// PARAMETERS
//  g                   - graph with nonzero identity
//  mm                  - scope
//  identifier_callback - user callback to get vertex labels in the lookup sequence
//  candidate_callback  - user callback to qualify a vertex
//  context             - opaque user context for identifier_callback
//  *V                  - (returns) one, or two vertices (based on *r)
//  *r                  - (returns) return code
//
// REMARKS
//  return code,
//   0 - no matching vertices
//   1 - one matching vertex (*V)
//   2 - two or more matching vertices (*V[0], *V[1])
//
xapi moria_graph_lookup(
    /* 1 */ struct moria_graph * restrict g
  , /* 2 */ const struct hashtable * restrict mm
  , /* 3 */ moria_graph_lookup_identifier_callback identifier_callback
  , /* 4 */ bool (*candidate_callback)(void * context, const struct moria_vertex * const restrict v)
  , /* 5 */ void * context
  , /* 6 */ struct moria_vertex * restrict V[2]
  , /* 7 */ int * restrict r
)
  __attribute__((nonnull(1, 2, 3, 6, 7)));

typedef struct moria_graph_lookup_sentinel_context {
  char ** labels;     // sentinel-terminated sequence of labels
  uint16_t index;     // iteration state
} moria_graph_lookup_sentinel_context;

/// graph_lookup_sentinel
//
// lookup callback for resolving an identifier comprised of a sentinel-terminated array of labels
//
xapi moria_graph_lookup_sentinel(void * restrict context, const char ** restrict label, uint16_t * restrict label_len)
  __attribute__((nonnull(1)));

xapi moria_graph_linear_search(
    struct llist * restrict vertex_list
  , const char * restrict label
  , uint16_t label_len
  , struct moria_vertex ** restrict rv
)
  __attribute__((nonnull));

#endif
