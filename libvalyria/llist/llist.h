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

#ifndef _VALYRIA_LLIST_H
#define _VALYRIA_LLIST_H

/*

SUMMARY
 circular doubly-linked embedded list

REMARKS
 the list is initialized with a dedicated head - the minimum size is 0

*/

#include "types.h"
#include "macros.h"

typedef struct llist {
  struct llist * next;
  struct llist * prev;
} llist;

#define LLIST_INITIALIZER(name) (llist) { next : &(name), prev : &(name) }

/// llist_init
//
// SUMMARY
//  initialize an llist head
//
static inline llist * __attribute__((nonnull)) llist_init_node(llist * head)
{
  head->next = head;
  head->prev = head;

  return head;
}

/// llist_count
//
// SUMMARY
//  get the number of items in the list
//
#define llist_count(head) \
  llist_count_node(head)

#define llist_size(head) llist_count(head)

/// llist_empty
//
// SUMMARY
//  check whether a list is empty
//
#define llist_empty(head) \
  llist_empty_node(head)

static inline bool __attribute__((nonnull)) llist_empty_node(const llist * head)
{
  return head->next == head;
}

#define llist_attached(item, member)  \
  llist_attached_node(&(item)->member)

static inline bool __attribute__((nonnull)) llist_attached_node(const llist *node)
{
  return node->next != node;
}

/// list_prepend (unshift)
//
// add to a list such that the item will be first in an enumeration of the list
//
// PARAMETERS
//  head - list head
//  node - item to prepend
//
#define llist_prepend(head, item, member) \
  llist_prepend_node(head, &(item)->member)

static inline llist * __attribute__((nonnull)) llist_prepend_node(llist * head, llist * node)
{
  node->prev = head;
  node->next = head->next;
  head->next->prev = node;
  head->next = node;

  return head;
}

/// list_append (push)
//
// add to a list such that the item will be last in an enumeration of the list
//
// PARAMETERS
//  head - list head
//  node - item to append
//
#define llist_append(head, item, member) \
  llist_append_node(head, &(item)->member)

#define llist_push(head, item, member) llist_append(head, item, member)
#define llist_push_node(head, node) llist_append_node(head, node)

static inline llist * __attribute__((nonnull)) llist_append_node(llist * head, llist * node)
{
  node->prev = head->prev;
  node->next = head;
  head->prev->next = node;
  head->prev = node;

  return head;
}

/// llist_delete
//
// remove an item from a list
//
#define llist_delete(item, member) \
  llist_delete_node(&(item)->member)

static inline void  __attribute__((nonnull)) llist_delete_node(llist * node)
{
  node->next->prev = node->prev;
  node->prev->next = node->next;
  node->next = node;
  node->prev = node;
}

/// llist_shift
//
// remove the first item from a list, if any
//
#define llist_shift(head, type, member) ({      \
  llist * node = 0;                             \
  if(!llist_empty_node(head)) {                 \
    node = llist_next_node(head, head);         \
    llist_delete_node(node);                    \
  }                                             \
  node ? containerof(node, type, member) : 0;   \
})

/// llist_pop
//
// remove the last item from a list, if any
//
#define llist_pop(head, type, member) ({        \
  llist * node = 0;                             \
  if(!llist_empty_node(head)) {                 \
    node = llist_prev_node(head, head);         \
    llist_delete_node(node);                    \
  }                                             \
  node ? containerof(node, type, member) : 0;   \
})

/*
* iteration
*/

#define llist_foreach_node(head, node) \
  for(node = (head)->next; node != (head); node = node->next)

#define llist_foreach(head, item, member)                            \
  for(                                                               \
    item = containerof((head)->next, typeof(*item), member);         \
    item != containerof(head, typeof(*item), member);                \
    item = containerof((item)->member.next, typeof(*item), member)   \
  )

// traversal safe against node removal
#define llist_foreach_safe(head, item, member, cursor)                                      \
  for(                                                                                      \
    item = containerof((head)->next, typeof(*item), member), cursor = (item)->member.next;  \
    item != containerof(head, typeof(*item), member);                                       \
    item = containerof(cursor, typeof(*item), member), cursor = cursor->next                \
  )


/*
* cursor traversal - returns NULL once the head node is reached
*/

#define llist_first(head, type, member) \
  ({ llist * node = llist_next_node(head, head); node ? containerof(node, type, member) : NULL; })

#define llist_last(head, type, member) \
  ({ llist * node = llist_prev_node(head, head); node ? containerof(node, type, member) : NULL; })

#define llist_next(head, item, member) \
  ({ llist * node = llist_next_node(head, refas(item, member)); node ? containerof(node, typeof(*item), member) : NULL; })

static inline llist * __attribute__((nonnull(1))) llist_next_node(const llist * head, const llist * node)
{
  if(node == NULL)
    return NULL;

  if(node->next == head)
    return NULL;

  return node->next;
}

#undef refas
#define refas(expr, as) ({    \
  typeof(expr->as) * ptr = 0; \
  if(expr) {                  \
    ptr = &expr->as;          \
  }                           \
  ptr;                        \
})

#define llist_prev(head, item, member) \
  ({ llist * node = llist_prev_node(head, refas(item, member)); node ? containerof(node, typeof(*item), member) : NULL; })

static inline llist * __attribute__((nonnull(1))) llist_prev_node(const llist * head, const llist * node)
{
  if(node == NULL)
    return NULL;

  if(node->prev == head)
    return NULL;

  return node->prev;
}

/// llist_splice_head
//
// concatenate a source list onto a destination list such that items in the source list will be
// first in an enumeration of the destination list
//
// the order of the items in the source list is unchanged
//
// after this operation, the source list is empty
//
static inline llist * __attribute__((nonnull)) llist_splice_head(llist * dst_head, llist * src_head)
{
  if(llist_empty(src_head)) {
    return dst_head;
  }

  dst_head->next->prev = src_head->prev;
  src_head->prev->next = dst_head->next;
  dst_head->next = src_head->next;
  src_head->next->prev = dst_head;

  src_head->prev = src_head;
  src_head->next = src_head;

  return dst_head;
}

/// llist_splice_head
//
// concatenate a source list onto a destination list such that items in the source list will be
// last in an enumeration of the destination list
//
// the order of the items in the source list is unchanged
//
// after this operation, the source list is empty
//
static inline llist * __attribute__((nonnull)) llist_splice_tail(llist * dst_head, llist * src_head)
{
  if(llist_empty(src_head)) {
    return dst_head;
  }

  src_head->prev->next = dst_head;
  src_head->next->prev = dst_head->prev;
  dst_head->prev->next = src_head->next;
  dst_head->prev = src_head->prev;

  src_head->prev = src_head;
  src_head->next = src_head;

  return dst_head;
}

static inline size_t __attribute__((nonnull)) llist_count_node(const llist * head)
{
  size_t size = 0;
  llist * node;

  llist_foreach_node(head, node) {
    size++;
  }

  return size;
}

#endif
