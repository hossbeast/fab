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

#ifndef _FABD_RCU_LIST_H
#define _FABD_RCU_LIST_H

/*
 * circular, doubly-linked, embedded, rcu-synchronized list.
 *
 * Traversal may only be used on threads which have called rcu_register.
 */

#include "xapi.h"
#include "types.h"

#include "rcu.h"

typedef struct rcu_list {
  struct rcu_list * next;
  struct rcu_list * prev;
} rcu_list;

#define RCU_LIST_INITIALIZER(name) (rcu_list) { next : &(name), prev : &(name) }
#define RCU_LIST_DELETED (void*)0x42

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

#endif
