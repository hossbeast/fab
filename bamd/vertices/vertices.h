/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "valyria/llist.h"

struct moria_vertex;

typedef struct vertices {
  llist lln;          // insertion order maintained
  llist lln_sorted;   // sorted for uniqueness

  struct moria_vertex *p;
} vertices;

void vertices_setup(void);
void vertices_cleanup(void);

void vertices_init(vertices * restrict);
void vertices_destroy(vertices * restrict)
  __attribute__((nonnull));

void vertices_push(vertices * restrict list, const struct moria_vertex * restrict p)
  __attribute__((nonnull));

void vertices_unshift(vertices * restrict list, const struct moria_vertex * restrict p)
  __attribute__((nonnull));

#define vertices_foreach(head, item, node)                       \
  for(                                                          \
    node = containerof((head)->lln.next, typeof(*node), lln);   \
    item = node->p, node != head;                               \
    node = containerof(node->lln.next, typeof(*node), lln)      \
  )

vertices *vertices_shift(vertices * restrict list)
  __attribute__((nonnull));

void vertices_free(vertices * restrict eln);

#define vertices_first(list) \
  ({ llist *node = (list)->lln.next; node == &(list)->lln ? NULL : containerof(node, vertices, lln); })

#define vertices_next(list, eln) \
  ({ llist *node = (eln)->lln.next; node == &(list)->lln ? NULL : containerof(node, vertices, lln); })

size_t vertices_count(const vertices * restrict list)
  __attribute__((nonnull));

bool vertices_empty(const vertices * restrict list)
  __attribute__((nonnull));
