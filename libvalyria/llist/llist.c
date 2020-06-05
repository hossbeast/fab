/* Copyright (c) 2012-2019 Todd Freed <todd.freed@gmail.com>

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

#include "llist.h"

llist * API llist_init_node(llist * head)
{
  head->next = head;
  head->prev = head;

  return head;
}

llist * API llist_prepend_node(llist * head, llist * node)
{
  node->prev = head;
  node->next = head->next;
  head->next->prev = node;
  head->next = node;

  return head;
}

llist * API llist_append_node(llist * head, llist * node)
{
  node->prev = head->prev;
  node->next = head;
  head->prev->next = node;
  head->prev = node;

  return head;
}

void API llist_delete_node(llist * node)
{
  node->next->prev = node->prev;
  node->prev->next = node->next;
  node->next = node;
  node->prev = node;
}

/* cursor traversal */

llist * API llist_next_node(const llist * head, const llist * node)
{
  if(node == NULL)
    return NULL;

  if(node->next == head)
    return NULL;

  return node->next;
}

llist * API llist_prev_node(const llist * head, const llist * node)
{
  if(node == NULL)
    return NULL;

  if(node->prev == head)
    return NULL;

  return node->prev;
}

bool API llist_empty_node(const llist * head)
{
  return head->next == head;
}

bool API llist_attached_node(const llist *node)
{
  return node->next != node;
}

size_t API llist_count_node(const llist * head)
{
  size_t size = 0;
  llist * node;

  llist_foreach_node(head, node) {
    size++;
  }

  return size;
}

llist * API llist_splice_head(llist * dst_head, llist * src_head)
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

llist * API llist_splice_tail(llist * dst_head, llist * src_head)
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
