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

#ifndef _VALYRIA_STACK_H
#define _VALYRIA_STACK_H

/*

SUMMARY
 circular embedded stack - doubly-linked to support random access delete

REMARKS
 the stack is initialized with a dedicated head - the minimum size is 0

*/

#include "xapi.h"
#include "types.h"

#include "macros.h"

typedef struct stack {
  struct stack * next;
  struct stack * prev;
} stack;

#define STACK_INITIALIZER(name) (stack) { next : &(name), prev : &(name) }

/*
 * initialize a stack head
 */
stack * stack_init(stack * head)
  __attribute__((nonnull));

/*
 * get the number of items in the stack
 */
size_t stack_count(const stack * restrict head)
  __attribute__((nonnull));

/*
 * check whether a stack is empty
 */
bool stack_empty(const stack * restrict head)
  __attribute__((nonnull));

/*
 * check whether a stack node is a member of any stack
 */
bool stack_attached(const stack * restrict node)
  __attribute__((nonnull));

/*
 * add an item to the end of a stack - the item is last in an enumeration of the stack
 *
 * head - stack head
 * node - item to push
 */
stack * stack_push(stack * restrict head, stack * restrict node)
  __attribute__((nonnull));

/*
 * remove an item from a stack
 */
void stack_delete(stack * node)
  __attribute__((nonnull));

/*
 * remove the last item from a stack, if any
 */
#define stack_pop(head, type, member) ({        \
  stack * node = 0;                             \
  if(!stack_empty_node(head)) {                 \
    node = stack_prev_node(head, head);         \
    stack_delete_node(node);                    \
  }                                             \
  node ? containerof(node, type, member) : 0;   \
})

/*
 * iteration
 */

#define stack_foreach(head, item, member)                            \
  for(                                                               \
    item = containerof((head)->next, typeof(*item), member);         \
    item != containerof(head, typeof(*item), member);                \
    item = containerof((item)->member.next, typeof(*item), member)   \
  )

/*
 * concatenate a source stack onto the end of a destination stack
 *
 * the order of the items in the source stack is unchanged
 *
 * after this operation, the source stack is empty
 */
stack * stack_splice(stack * dst_head, stack * src_head)
  __attribute__((nonnull));

#endif
