/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "llist.h"

void llist_sort(llist * restrict list, int (*cmp)(void *ctx, llist * restrict A, llist * restrict B), void *ctx)
{
  llist *p;
  llist *q;
  llist *e;
  llist *head;
  llist *tail;
  uint32_t stride;
  uint32_t plen;
  uint32_t qlen;
  uint32_t x;
  uint32_t merges;

  if(llist_empty(list)) {
    return;
  }

  /* unzip the list */
  list->prev->next = 0;
  head = list->next;
  tail = list->prev;

  stride = 1;
  tail = 0;
  do
  {
    p = head;
    tail = 0;
    merges = 0;

    while(p)
    {
      merges++;
      q = p;
      for(x = 0; x < stride; x++) {
        if((q = q->next) == 0) {
          x++;
          break;
        }
      }

      plen = x;
      qlen = stride;
      while(plen > 0 || (qlen > 0 && q))
      {
        if(plen == 0) {
          e = q;
          q = q->next;
          qlen--;
        } else if(qlen == 0 || !q) {
          e = p;
          p = p->next;
          plen--;
        } else if(cmp(ctx, p, q) <= 0) {
          e = p;
          p = p->next;
          plen--;
        } else {
          e = q;
          q = q->next;
          qlen--;
        }

        if(tail) {
          tail->next = e;
        } else {
          head = e;
        }
        e->prev = tail;
        e->next = 0;
        tail = e;
      }

      p = q;
    }

    stride <<= 1;
  } while(merges > 1);

  /* reassemble */
  list->next = head;
  list->prev = tail;
  head->prev = list;
  tail->next = list;
}
