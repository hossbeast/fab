/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"
#include "moria.h"

#include "vertices.h"
#include "locks.h"

static llist freelist = LLIST_INITIALIZER(freelist);
static struct futexlock freelist_lock;

// static

static vertices *vertices_alloc(void)
{
  vertices *ent = 0;

  if(!llist_empty(&freelist)) {
    futexlock_acquire(&freelist_lock);
    ent = llist_shift(&freelist, typeof(*ent), lln);
    futexlock_release(&freelist_lock);
  }

  if(!ent) {
    xmalloc(&ent, sizeof(*ent));
  }

  vertices_init(ent);

  return ent;
}

void vertices_setup(void)
{
}

void vertices_cleanup(void)
{
  vertices *ent;

  while((ent = llist_shift(&freelist, typeof(*ent), lln))) {
    free(ent);
  }
}

void vertices_init(vertices * restrict list)
{
  llist_init(&list->lln);
  llist_init(&list->lln_sorted);
}

void vertices_destroy(vertices * restrict list)
{
  futexlock_acquire(&freelist_lock);
  llist_splice_head(&freelist, &list->lln);
  futexlock_release(&freelist_lock);

  vertices_init(list);
}

void vertices_free(vertices * restrict eln)
{
  futexlock_acquire(&freelist_lock);
  llist_push(&freelist, &eln->lln);
  futexlock_release(&freelist_lock);
}

void vertices_push(vertices * restrict list, const moria_vertex * restrict p)
{
  vertices *ent;
  vertices *eln;
  llist *lln;
  ptrdiff_t d;

  for(lln = list->lln_sorted.next; lln != &list->lln_sorted; lln = lln->next) {
    eln = containerof(lln, typeof(*eln), lln_sorted);
    if((d = INTCMP(p, eln->p)) < 0) {
      break;
    } else if(d == 0) {
      return;
    }
  }

  lln = lln->prev;

  ent = vertices_alloc();
  ent->p = (void*)p;
  llist_insert(lln, &ent->lln_sorted, lln->next);

  /* insertion order maintained */
  llist_push(&list->lln, &ent->lln);
}

void vertices_unshift(vertices * restrict list, const moria_vertex * restrict p)
{
  vertices *ent;
  vertices *eln;
  llist *lln;
  ptrdiff_t d;

  for(lln = list->lln_sorted.next; lln != &list->lln_sorted; lln = lln->next) {
    eln = containerof(lln, typeof(*eln), lln_sorted);
    if((d = INTCMP(p, eln->p)) < 0) {
      break;
    } else if(d == 0) {
      return;
    }
  }

  lln = lln->prev;

  ent = vertices_alloc();
  ent->p = (void*)p;
  llist_insert(lln, &ent->lln_sorted, lln->next);

  /* insertion order maintained */
  llist_unshift(&list->lln, &ent->lln);
}

vertices *vertices_shift(vertices * restrict list)
{
  vertices *eln;

  if(llist_empty(&list->lln)) {
    return 0;
  }

  eln = containerof(list->lln.next, typeof(*eln), lln);
  llist_delete(&eln->lln);
  llist_delete(&eln->lln_sorted);

  return eln;
}

size_t vertices_count(const vertices * restrict list)
{
  return llist_count(&list->lln);
}

bool vertices_empty(const vertices * restrict list)
{
  return llist_empty(&list->lln);
}
