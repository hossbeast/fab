/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "valyria/llist.h"

struct moria_edge;
enum moria_traversal_mode;

typedef struct entlist {
  llist lln;          // insertion order maintained
  llist lln_sorted;   // sorted for uniqueness

  void *p;
} entlist;

void entlist_setup(void);
void entlist_cleanup(void);

void entlist_init(entlist * restrict);
void entlist_destroy(entlist * restrict)
  __attribute__((nonnull));

void entlist_push(entlist * restrict list, const void * restrict p)
  __attribute__((nonnull));

void entlist_unshift(entlist * restrict list, const void * restrict p)
  __attribute__((nonnull));

#define entlist_foreach_node(head, node)                        \
  for(                                                          \
    node = containerof((head)->lln.next, typeof(*node), lln);   \
    node != head;                                               \
    node = containerof(node->lln.next, typeof(*node), lln)      \
  )

#define entlist_foreach(head, item, node)                       \
  for(                                                          \
    node = containerof((head)->lln.next, typeof(*node), lln);   \
    item = node->p, node != head;                               \
    node = containerof(node->lln.next, typeof(*node), lln)      \
  )

entlist *entlist_shift(entlist * restrict list)
  __attribute__((nonnull));

void entlist_free(entlist * restrict eln);

void entlist_visitor_edge(struct moria_edge * restrict e , void * ctx , enum moria_traversal_mode mode , int distance , moria_traverse_result * restrict result)
  __attribute__((nonnull));

#define entlist_first(list) \
  ({ llist *node = (list)->lln.next; node == &(list)->lln ? NULL : containerof(node, entlist, lln); })

#define entlist_next(list, eln) \
  ({ llist *node = (eln)->lln.next; node == &(list)->lln ? NULL : containerof(node, entlist, lln); })

size_t entlist_count(const entlist * restrict list)
  __attribute__((nonnull));

bool entlist_empty(const entlist * restrict list)
  __attribute__((nonnull));
