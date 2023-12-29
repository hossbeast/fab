/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "valyria/llist.h"
#include "xlinux/xstdlib.h"
#include "moria/traverse.h"

#include "entlist.h"
#include "locks.h"

static llist freelist = LLIST_INITIALIZER(freelist);
static struct futexlock freelist_lock;

// static

static entlist *entlist_alloc(void)
{
  entlist *ent = 0;

  if(!llist_empty(&freelist)) {
    futexlock_acquire(&freelist_lock);
    ent = llist_shift(&freelist, typeof(*ent), lln);
    futexlock_release(&freelist_lock);
  }

  if(!ent) {
    xmalloc(&ent, sizeof(*ent));
  }

  entlist_init(ent);

  return ent;
}

void entlist_setup(void)
{
}

void entlist_cleanup(void)
{
  entlist *ent;

  while((ent = llist_shift(&freelist, typeof(*ent), lln))) {
    free(ent);
  }
}

void entlist_init(entlist * restrict list)
{
  llist_init(&list->lln);
  llist_init(&list->lln_sorted);
}

void entlist_destroy(entlist * restrict list)
{
  futexlock_acquire(&freelist_lock);
  llist_splice_head(&freelist, &list->lln);
  futexlock_release(&freelist_lock);

  entlist_init(list);
}

void entlist_free(entlist * restrict eln)
{
  futexlock_acquire(&freelist_lock);
  llist_push(&freelist, &eln->lln);
  futexlock_release(&freelist_lock);
}

void entlist_push(entlist * restrict list, const void * restrict p)
{
  entlist *ent;
  entlist *eln;
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

  ent = entlist_alloc();
  ent->p = (void*)p;
  llist_insert(lln, &ent->lln_sorted, lln->next);

  /* insertion order maintained */
  llist_push(&list->lln, &ent->lln);
}

void entlist_unshift(entlist * restrict list, const void * restrict p)
{
  entlist *ent;
  entlist *eln;
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

  ent = entlist_alloc();
  ent->p = (void*)p;
  llist_insert(lln, &ent->lln_sorted, lln->next);

  /* insertion order maintained */
  llist_unshift(&list->lln, &ent->lln);
}

entlist *entlist_shift(entlist * restrict list)
{
  entlist *eln;

  if(llist_empty(&list->lln)) {
    return 0;
  }

  eln = containerof(list->lln.next, typeof(*eln), lln);
  llist_delete(&eln->lln);
  llist_delete(&eln->lln_sorted);

  return eln;
}

void entlist_visitor_edge(
    struct moria_edge * restrict e
  , void * ctx
  , moria_traversal_mode mode
  , int distance
  , moria_traverse_result * restrict result
)
{
  entlist *list = ctx;
  entlist_push(list, e);
}

size_t entlist_count(const entlist * restrict list)
{
  return llist_count(&list->lln);
}

bool entlist_empty(const entlist * restrict list)
{
  return llist_empty(&list->lln);
}
