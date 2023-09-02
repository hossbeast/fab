/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "chain.h"

#define chain_foreach_node(node, T, head) \
  for(node = (head), T = NULL; T == NULL || node != (head); T = node, node = node->next)

chain * API chain_init(chain * head)
{
  head->next = head;
  head->prev = head;

  return head;
}

chain * API chain_append(chain * head, chain * node)
{
  node->prev = head;
  node->next = head->next;
  head->next->prev = node;
  head->next = node;

  return head;
}

chain * API chain_prepend(chain * head, chain * node)
{
  node->prev = head->prev;
  node->next = head;
  head->prev->next = node;
  head->prev = node;

  return head;
}

chain * chain_pop_node(chain * restrict head)
{
  chain *prev;

  prev = head->prev;

  prev->next->prev = prev->prev;
  prev->prev->next = prev->next;

  prev->prev = prev;
  prev->next = prev;

  return prev;
}

size_t API chain_count(const chain * head)
{
  size_t size = 0;
  const chain *node, *T;

  chain_foreach_node(node, T, head) {
    size++;
  }

  return size;
}

chain * API chain_splice(chain * dst_head, chain * src_head)
{
// src : D E F
// dst : A B C
//  ->
// dst : A B C D E F

  chain * T = dst_head->prev;

  dst_head->prev->next = src_head;        // C->next = D
  dst_head->prev       = src_head->prev;  // A->prev = F
  src_head->prev->next = dst_head;        // F->next = A
  src_head->prev       = T;               // D->prev = C

  return dst_head;
}

chain * API chain_next_node(const chain * head, const chain * cursor[1])
{
  const chain *T;

  if(cursor[0] == NULL) {
    cursor[0] = head->next;
    return (void*)head;
  }

  if(cursor[0] == head) {
    return NULL;
  }

  T = cursor[0];
  cursor[0] = (cursor[0])->next;

  return (chain*)T;
}

chain * API chain_prev_node(const chain * head, const chain * cursor[1])
{
  const chain *T;

  if(cursor[0] == NULL) {
    cursor[0] = head->prev->prev;
    return (void*)head->prev;
  }

  if(cursor[0] == head->prev) {
    return NULL;
  }

  T = cursor[0];
  cursor[0] = (cursor[0])->prev;

  return (chain*)T;
}

bool API chain_has_next(const chain * head, const chain cursor[1])
{
  if(cursor == NULL) {
    return true;
  }

  return cursor != head;
}

chain * API chain_next_safe_node(const chain * head, const chain * cursor[2])
{
  const chain *T;

  if(cursor[0] == NULL) {
    cursor[0] = head->next;
    cursor[1] = cursor[0]->next;
    return (void*)head;
  }

  if(cursor[0] == head) {
    return NULL;
  }

  T = cursor[0];
  cursor[0] = cursor[1];
  if(cursor[1] != head) {
    cursor[1] = cursor[1]->next;
  }
  return (chain*)T;
}
