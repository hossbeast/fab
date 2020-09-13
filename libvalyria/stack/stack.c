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

#include "stack.h"

stack * API stack_init(stack * head)
{
  head->next = head;
  head->prev = head;

  return head;
}

stack * API stack_push(stack * head, stack * node)
{
  node->prev = head->prev;
  node->next = head;
  head->prev->next = node;
  head->prev = node;

  return head;
}

void API stack_delete(stack * node)
{
  node->next->prev = node->prev;
  node->prev->next = node->next;
  // node->next - not modified
  node->prev = node;
}

bool API stack_empty(const stack * head)
{
  return head->next == head;
}

bool API stack_attached(const stack *node)
{
  return node->next != node;
}

size_t API stack_count(const stack * head)
{
  size_t size = 0;
  stack * node;

  for(node = head->next; node != head; node = node->next) {
    size++;
  }

  return size;
}

stack * API stack_splice(stack * dst_head, stack * src_head)
{
  if(stack_empty(src_head)) {
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
