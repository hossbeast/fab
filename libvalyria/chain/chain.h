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

#ifndef _VALYRIA_CHAIN_H
#define _VALYRIA_CHAIN_H

/*

SUMMARY
 circular doubly-linked embedded list

REMARKS
 there is no dedicated head item - the minimum chain size is 1

*/

#include "xapi.h"
#include "types.h"

#include "macros.h"

typedef struct chain {
  struct chain * next;
  struct chain * prev;
} chain;

chain * chain_init_node(chain * head)
  __attribute__((nonnull));

#define chain_init(_head, _member)        \
  containerof(                            \
      chain_init_node(&(_head)->_member)  \
    , typeof(*_head)                      \
    , _member                             \
  )

size_t chain_count_node(const chain * head)
  __attribute__((nonnull));

#define chain_count(_head, _member) chain_count_node(&(_head)->_member)

/// chain_add
//
// add an item to a chain such that the new item will be last in an enumeration of the chain
// starting from the specified head
//
// PARAMETERS
//  head - list head
//  node - item to add
//
chain * chain_add_tail_node(chain * dst, chain * node)
  __attribute__((nonnull));

chain * chain_add_head_node(chain * dst, chain * node)
  __attribute__((nonnull));

#define chain_add_head(_head, _node, _member)                    \
  containerof(                                                   \
      chain_add_head_node(&(_head)->_member, &(_node)->_member)  \
    , typeof(*_head)                                             \
    , _member                                                    \
  )

#define chain_add_tail(_head, _node, _member)                    \
  containerof(                                                   \
      chain_add_tail_node(&(_head)->_member, &(_node)->_member)  \
    , typeof(*_head)                                             \
    , _member                                                    \
  )

#define chain_add(_head, _node, _member) chain_add_head(_head, _node, _member)

/* chain traversal */

/// chain_next
//
// returns the next item in a cursor traversal
//
chain * chain_next_node(const chain * head, const chain * cursor[1])
  __attribute__((nonnull));

chain * chain_prev_node(const chain * head, const chain * cursor[1])
  __attribute__((nonnull));

#define chain_next(_head, _cursorp, _member) ({                 \
  typeof(*_head) *__i = NULL;                                   \
  const chain * __T;                                            \
  if((__T = chain_next_node(&(_head)->_member, (_cursorp)))) {  \
    __i = containerof(__T, typeof(*_head), _member);            \
  }                                                             \
  __i;                                                          \
})

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

/* chain safe traversal */

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

/// chain_has_next
//
// returns a boolean value indicating whether any items remain in a cursor traversal
//
bool chain_has_next_node(const chain * head, const chain cursor[1])
  __attribute__((nonnull(1)));

#define chain_has_next(_head, _cursor, _member) chain_has_next_node(&(_head)->_member, _cursor)

/// chain_splice
//
// merge two chains such that the items in the source chain will be last in an enumeration of the merged
// chain which starts at the specified dst_head
//
chain * chain_splice_tail_node(chain * dst_head, chain * src_head)
  __attribute__((nonnull));

#define chain_splice_tail(_dst_head, _src_head, _member)                   \
  containerof(                                                             \
      chain_splice_tail_node(&(_dst_head)->_member, &(_src_head)->_member) \
    , typeof(*_dst_head)                                                   \
    , _member                                                              \
  )

#endif
