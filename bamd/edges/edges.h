/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "valyria/llist.h"

struct moria_edge;
enum moria_traversal_mode;
struct moria_traverse_result;

typedef struct edges {
  llist lln;          // insertion order maintained
  llist lln_sorted;   // sorted for uniqueness

  struct moria_edge *p;
} edges;

void edges_setup(void);
void edges_cleanup(void);

void edges_init(edges * restrict);
void edges_destroy(edges * restrict)
  __attribute__((nonnull));

void edges_push(edges * restrict list, const struct moria_edge * restrict p)
  __attribute__((nonnull));

void edges_unshift(edges * restrict list, const struct moria_edge * restrict p)
  __attribute__((nonnull));

#define edges_foreach(head, item, node)                         \
  for(                                                          \
    node = containerof((head)->lln.next, typeof(*node), lln);   \
    item = node->p, node != head;                               \
    node = containerof(node->lln.next, typeof(*node), lln)      \
  )

edges *edges_shift(edges * restrict list)
  __attribute__((nonnull));

void edges_free(edges * restrict eln);

void edges_visitor_edge(struct moria_edge * restrict e, void * ctx, enum moria_traversal_mode mode, int distance, struct moria_traverse_result * restrict result)
  __attribute__((nonnull));

#define edges_first(list) \
  ({ llist *node = (list)->lln.next; node == &(list)->lln ? NULL : containerof(node, edges, lln); })

#define edges_next(list, eln) \
  ({ llist *node = (eln)->lln.next; node == &(list)->lln ? NULL : containerof(node, edges, lln); })

size_t edges_count(const edges * restrict list)
  __attribute__((nonnull));

bool edges_empty(const edges * restrict list)
  __attribute__((nonnull));
