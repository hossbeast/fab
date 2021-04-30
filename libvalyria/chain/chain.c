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

#include "chain.h"

#define chain_foreach_node(node, T, head) \
  for(node = (head), T = NULL; T == NULL || node != (head); T = node, node = node->next)

chain * API chain_init_node(chain * head)
{
  head->next = head;
  head->prev = head;

  return head;
}

chain * API chain_add_tail_node(chain * head, chain * node)
{
  node->prev = head;
  node->next = head->next;
  head->next->prev = node;
  head->next = node;

  return head;
}

chain * API chain_add_head_node(chain * head, chain * node)
{
  node->prev = head->prev;
  node->next = head;
  head->prev->next = node;
  head->prev = node;

  return head;
}

size_t API chain_count_node(const chain * head)
{
  size_t size = 0;
  const chain *node, *T;

  chain_foreach_node(node, T, head) {
    size++;
  }

  return size;
}

chain * API chain_splice_tail_node(chain * dst_head, chain * src_head)
{
// src : D E F
// dst : A B C

  chain * T = dst_head->prev;

  dst_head->prev->next = src_head;        // C->next = D    A->next = D
  dst_head->prev       = src_head->prev;  // A->prev = F    A->prev = D
  src_head->prev->next = dst_head;        // F->next = A    D->next = A
  src_head->prev       = T;               // D->prev = C    D->prev = A

  return dst_head;
}

chain * API chain_next_node(const chain * head, const chain * cursor[1])
{
  if(cursor[0] == NULL) {
    cursor[0] = head->next;
    return (void*)head;
  }

  if(cursor[0] == head)
    return NULL;

  const chain * T = cursor[0];
  cursor[0] = (cursor[0])->next;
  return (chain*)T;
}

chain * API chain_prev_node(const chain * head, const chain * cursor[1])
{
  if(cursor[0] == NULL) {
    cursor[0] = head->prev;
    return (void*)head;
  }

  if(cursor[0] == head)
    return NULL;

  const chain * T = cursor[0];
  cursor[0] = (cursor[0])->prev;
  return (chain*)T;
}

bool API chain_has_next_node(const chain * head, const chain cursor[1])
{
  if(cursor == NULL)
    return true;

  return cursor != head;
}

chain * API chain_next_safe_node(const chain * head, const chain * cursor[2])
{
  if(cursor[0] == NULL) {
    cursor[0] = head->next;
    cursor[1] = cursor[0]->next;
    return (void*)head;
  }

  if(cursor[0] == head)
    return NULL;

  const chain * T = cursor[0];
  cursor[0] = cursor[1];
  if(cursor[1] != head)
    cursor[1] = cursor[1]->next;
  return (chain*)T;
}
