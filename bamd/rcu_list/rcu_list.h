/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*
 * circular, doubly-linked, embedded, rcu-synchronized list.
 *
 * Traversal may only be used on threads which have called rcu_register.
 */

#include "types.h"
#include "rcu.h"

typedef struct rcu_list {
  struct rcu_list * next;
  struct rcu_list * prev;
} rcu_list;

#define RCU_LIST_INITIALIZER(name) (rcu_list) { next : &(name), prev : &(name) }
#define RCU_LIST_DELETED (void*)0x42

static inline void rcu_list_init(rcu_list * restrict list)
{
  list->next = list;
  list->prev = list;
}

/*
 * add an item to the end of an rcu list
 *
 * head - list head
 * node - item to push
 */
static inline rcu_list * __attribute__((nonnull)) rcu_list_push(rcu_list * restrict head, rcu_list * restrict node)
{
  node->prev = head->prev;
  rcu_update(node->next, head);
  rcu_update(head->prev->next, node);
  head->prev = node;

  return head;
}

/* remove an item from an rcu-list, leaving node->next unmodified, so that concurrent traversal is permitted. */
static inline void __attribute__((nonnull)) rcu_list_delete(rcu_list * restrict node)
{
  /* excise */
  node->next->prev = node->prev;
  rcu_update(node->prev->next, node->next);

  /* induce crash on double-delete */
  node->prev = RCU_LIST_DELETED;
}

/* only forward traversal is supported */
#define rcu_list_foreach(head, item, member)                                    \
  for(                                                                          \
    item = containerof(rcu_read((head)->next), typeof(*item), member);          \
    item != containerof(head, typeof(*item), member);                           \
    item = containerof(rcu_read((item)->member.next), typeof(*item), member)    \
  )
