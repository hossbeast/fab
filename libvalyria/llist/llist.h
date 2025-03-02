/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

SUMMARY
 circular doubly-linked embedded list

REMARKS
 the list is initialized with a dedicated head - the minimum size is 0

*/

#include "types.h"
#include "macros.h"

typedef struct llist {
  struct llist *next;
  struct llist *prev;
} llist;

#define LLIST_INITIALIZER(name) (llist) { .next = &(name), .prev = &(name) }

/// llist_init
//
// SUMMARY
//  initialize an llist head
//
static inline void __attribute__((nonnull)) llist_init(llist * head)
{
  head->next = head;
  head->prev = head;
}

/* check whether a list is empty */
static inline bool __attribute__((nonnull)) llist_empty(const llist * head)
{
  return head->next == head;
}

static inline bool __attribute__((nonnull)) llist_attached(const llist *node)
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
static inline void __attribute__((nonnull)) llist_prepend(llist * head, llist * node)
{
//  RUNTIME_ASSERT(!llist_attached(node));

  node->prev = head;
  node->next = head->next;
  head->next->prev = node;
  head->next = node;
}

#define llist_unshift(...) llist_prepend(__VA_ARGS__)

/// list_append (push)
//
// add to a list such that the item will be last in an enumeration of the list
//
// PARAMETERS
//  head - list head
//  node - item to append
//
static inline void __attribute__((nonnull)) llist_append(llist * head, llist * node)
{
//  RUNTIME_ASSERT(!llist_attached(node));
  node->prev = head->prev;
  node->next = head;
  head->prev->next = node;
  head->prev = node;
}

#define llist_push(...) llist_append(__VA_ARGS__)

static inline void __attribute__((nonnull)) llist_insert(llist * restrict prev, llist * restrict node, llist * restrict next)
{
//  RUNTIME_ASSERT(!llist_attached(node));

  prev->next = node;
  node->prev = prev;

  node->next = next;
  next->prev = node;
}

/* remove an item from a list */
static inline void  __attribute__((nonnull)) llist_delete(llist * node)
{
//  RUNTIME_ASSERT(llist_attached(node));
  node->next->prev = node->prev;
  node->prev->next = node->next;
  node->next = node;
  node->prev = node;

//#if CONFIG_DEBUG || CONFIG_DEVEL
//  node->next = (void*)(uintptr_t)0x42;
//  node->prev = (void*)(uintptr_t)0x42;
//#endif
}

/* remove the first item from a list, if any, and return it */
#define llist_shift(head, type, member) ({      \
  llist * node = 0;                             \
  if(!llist_empty(head)) {                      \
    node = llist_next_node(head, head);         \
    llist_delete(node);                         \
  }                                             \
  node ? containerof(node, type, member) : 0;   \
})

/* remove the last item from a list, if any, and return it */
#define llist_pop(head, type, member) ({        \
  llist * node = 0;                             \
  if(!llist_empty(head)) {                      \
    node = llist_prev_node(head, head);         \
    llist_delete(node);                         \
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
    item = containerof((head)->next, typeof(*item), member), cursor = (head)->next->next;   \
    item != containerof(head, typeof(*item), member);                                       \
    item = containerof(cursor, typeof(*item), member), cursor = cursor->next                \
  )

#define llist_foreach_reverse(head, item, member)                    \
  for(                                                               \
    item = containerof((head)->prev, typeof(*item), member);         \
    item != containerof(head, typeof(*item), member);                \
    item = containerof((item)->member.prev, typeof(*item), member)   \
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

/*
 * merge two lists such that the items in the source list are now first in an enumeration of the
 * destination list
 *
 * the internal order of items is unchanged
 *
 * after this operation, src_head is empty
 *
 * before
 *  dst d e f
 *  src a b c
 *
 * after
 *  dst a b c d e f
 *  src
 */
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

/*
 * merge two lists such that the items in the source list are now last in an enumeration of the
 * destination list
 *
 * the internal order of the two lists is unchanged
 *
 * after this operation, src_head is empty
 *
 * before
 *  src a b c
 *  dst d e f
 *
 * after
 *  dst d e f a b c
 *  src
 */
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

/* traverse the list and return the number of items visited */
static inline size_t __attribute__((nonnull)) llist_count(const llist * head)
{
  size_t size;
  llist * node;

  size = 0;
  llist_foreach_node(head, node) {
    size++;
  }

  return size;
}

void llist_sort(llist * restrict list, int (*cmp)(void *ctx, llist * restrict A, llist * restrict B), void *ctx)
  __attribute__((nonnull(1, 2)));
