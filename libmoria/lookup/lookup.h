/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

struct moria_vertex;
struct moria_graph;
struct llist;

/// graph_lookup_identifier_callback
//
// SUMMARY
//  returns the next label in the lookup sequence, or resets the lookup sequence if label == NULL
//
typedef void (*moria_graph_lookup_identifier_callback)(
    void * context                  // user context
  , const char ** restrict label    // (returns) next label
  , uint16_t * restrict label_len   // (returns) next label length
);

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
void moria_graph_lookup(
    /* 1 */ struct moria_graph * restrict g
  , /* 2 */ const struct moria_vertex * restrict index
  , /* 3 */ moria_graph_lookup_identifier_callback identifier_callback
  , /* 4 */ void (*index_lookup_callback)(void * context, const char * restrict label, uint16_t label_len, struct moria_vertex * const restrict v)
  , /* 5 */ void * context
  , /* 6 */ const struct moria_vertex * restrict V[2]
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
void moria_graph_lookup_sentinel(void * restrict context, const char ** restrict label, uint16_t * restrict label_len)
  __attribute__((nonnull(1)));

void moria_graph_linear_search(
    struct llist * restrict vertex_list
  , const char * restrict label
  , uint16_t label_len
  , struct moria_vertex ** restrict rv
)
  __attribute__((nonnull));
