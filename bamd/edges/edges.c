/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "valyria/llist.h"
#include "xlinux/xstdlib.h"
#include "moria/traverse.h"
#include "moria.h"

#include "edges.h"
#include "vertices.h"
#include "locks.h"

static llist freelist = LLIST_INITIALIZER(freelist);
static struct futexlock freelist_lock;

// static

static edges *edges_alloc(void)
{
  edges *ent = 0;

  if(!llist_empty(&freelist)) {
    futexlock_acquire(&freelist_lock);
    ent = llist_shift(&freelist, typeof(*ent), lln);
    futexlock_release(&freelist_lock);
  }

  if(!ent) {
    xmalloc(&ent, sizeof(*ent));
  }

  edges_init(ent);

  return ent;
}

void edges_setup(void)
{
}

void edges_cleanup(void)
{
  edges *ent;

  while((ent = llist_shift(&freelist, typeof(*ent), lln))) {
    free(ent);
  }
}

void edges_init(edges * restrict list)
{
  llist_init(&list->lln);
  llist_init(&list->lln_sorted);
}

void edges_destroy(edges * restrict list)
{
  futexlock_acquire(&freelist_lock);
  llist_splice_head(&freelist, &list->lln);
  futexlock_release(&freelist_lock);

  edges_init(list);
}

void edges_free(edges * restrict eln)
{
  futexlock_acquire(&freelist_lock);
  llist_push(&freelist, &eln->lln);
  futexlock_release(&freelist_lock);
}

void edges_push(edges * restrict list, const moria_edge * restrict p)
{
  edges *ent;
  edges *eln;
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

  ent = edges_alloc();
  ent->p = (void*)p;
  llist_insert(lln, &ent->lln_sorted, lln->next);

  /* insertion order maintained */
  llist_push(&list->lln, &ent->lln);
}

void edges_unshift(edges * restrict list, const moria_edge * restrict p)
{
  edges *ent;
  edges *eln;
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

  ent = edges_alloc();
  ent->p = (void*)p;
  llist_insert(lln, &ent->lln_sorted, lln->next);

  /* insertion order maintained */
  llist_unshift(&list->lln, &ent->lln);
}

edges *edges_shift(edges * restrict list)
{
  edges *eln;

  if(llist_empty(&list->lln)) {
    return 0;
  }

  eln = containerof(list->lln.next, typeof(*eln), lln);
  llist_delete(&eln->lln);
  llist_delete(&eln->lln_sorted);

  return eln;
}

void edges_visitor_edge(
    struct moria_edge * restrict e
  , void * ctx
  , moria_traversal_mode mode
  , int distance
  , moria_traverse_result * restrict result
)
{
  edges *list = ctx;
  edges_push(list, e);
}

size_t edges_count(const edges * restrict list)
{
  return llist_count(&list->lln);
}

bool edges_empty(const edges * restrict list)
{
  return llist_empty(&list->lln);
}
