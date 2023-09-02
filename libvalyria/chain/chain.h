/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

SUMMARY
 circular doubly-linked embedded list

REMARKS
 there is no dedicated head item - the minimum chain size is 1

*/

#include "types.h"
#include "macros.h"

typedef struct chain {
  struct chain * next;
  struct chain * prev;
} chain;

chain * chain_init(chain * head)
  __attribute__((nonnull));

size_t chain_count(const chain * head)
  __attribute__((nonnull));

/*
 * add an item to a chain such that the new item will be last in an enumeration of the chain
 * starting from the specified head
 *
 * head - list head
 * node - item to add
 */
chain * chain_append(chain * dst, chain * node)
  __attribute__((nonnull));

#define chain_push(dst, node) chain_append(dst, node)

/*
 * add an item to a chain such that the new item will be first in an enumeration of the chain
 * starting from the specified head
 *
 * head - list head
 * node - item to add
 */
chain * chain_prepend(chain * dst, chain * node)
  __attribute__((nonnull));

#define chain_unshift(dst, node) chain_prepend(dst, node)

//
// cursor-based chain traversal
//

chain * chain_next_node(const chain * head, const chain * cursor[1])
  __attribute__((nonnull));

#define chain_next(_head, _cursorp, _member) ({                 \
  typeof(*_head) *__i = NULL;                                   \
  const chain * __T;                                            \
  if((__T = chain_next_node(&(_head)->_member, (_cursorp)))) {  \
    __i = containerof(__T, typeof(*_head), _member);            \
  }                                                             \
  __i;                                                          \
})

/* check whether any items remain in a cursor-based traversal */
bool chain_has_next(const chain * head, const chain cursor[1])
  __attribute__((nonnull(1)));

chain * chain_prev_node(const chain * head, const chain * cursor[1])
  __attribute__((nonnull));

#define chain_prev(_head, _cursorp, _member) ({                 \
  typeof(*_head) *__i = NULL;                                   \
  const chain * __T;                                            \
  if((__T = chain_prev_node(&(_head)->_member, (_cursorp)))) {  \
    __i = containerof(__T, typeof(*_head), _member);            \
  }                                                             \
  __i;                                                          \
})

#define chain_foreach(_cursor, _item, _member, _head)                 \
  for(                                                                \
    _cursor = NULL, _item = chain_next((_head), &(_cursor), _member); \
    _item != NULL;                                                    \
    _item = chain_next((_head), &(_cursor), _member)                  \
  )

chain * chain_next_safe_node(const chain * head, const chain * cursor[2])
  __attribute__((nonnull));

#define chain_next_safe(_head, _cursor, _member) ({                 \
  typeof(*_head) *__i = NULL;                                       \
  const chain * __T;                                                \
  if((__T = chain_next_safe_node(&(_head)->_member, (_cursor)))) {  \
    __i = containerof(__T, typeof(*_head), _member);                \
  }                                                                 \
  __i;                                                              \
})

#define chain_foreach_safe(_cursor, _item, _member, _head)                     \
  for(                                                                         \
    (_cursor)[0] = NULL, _item = chain_next_safe((_head), (_cursor), _member); \
    _item != NULL;                                                             \
    _item = chain_next_safe((_head), (_cursor), _member)                       \
  )

/*
 * merge two chains such that the items in the source chain will be last in an enumeration of the merged
 * chain which starts at the specified dst_head
 */
chain * chain_splice(chain * dst_head, chain * src_head)
  __attribute__((nonnull));

/* remove the last item in a chain, if any, and return it */
chain * chain_pop_node(chain * restrict head)
  __attribute__((nonnull));

#define chain_pop(_head, _member) ({                  \
  typeof(*_head) *__i = NULL;                         \
  const chain *__T;                                   \
  if(((_head)->_member.next != (&_head->_member))) {  \
    __T = chain_pop_node(&_head->_member);            \
    __i = containerof(__T, typeof(*_head), _member);  \
  }                                                   \
  __i;                                                \
})
