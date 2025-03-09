/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "common/hash.h"
#include "moria.h"
#include "valyria/hashtable.h"
#include "xlinux/xstdlib.h"

#include "selection.h"
#include "dependency.h"
#include "bpe.h"
#include "locks.h"

static llist freelist = LLIST_INITIALIZER(freelist);
static struct futexlock freelist_lock;

static uint32_t selected_entity_hash(uint32_t h, const void * restrict _val)
{
  const selected *sn = _val;
  return hash32(h, &sn->p, sizeof(sn->p));
}

static int selected_entity_cmp(const void * _A, const void *_B)
{
  const selected *sa = _A;
  const selected *sb = _B;

  return INTCMP(sa->p, sb->p);
}

static void __attribute__((nonnull)) selection_add(selection * restrict sel, enum selected_type type, const void *v, uint16_t distance)
{
  selected snk = { p : v };
  selected *sn = 0;
  size_t newa;
  int x;
  hashtable_search_context ctx;

  ctx = (typeof(ctx)) {
    entry : &snk
  };
  if(hashtable_search(&sel->selected_entities, &ctx))
  {
    sn = hashtable_fetch(&sel->selected_entities, &ctx);
  }
  else
  {
    if(!llist_empty(&freelist)) {
      futexlock_acquire(&freelist_lock);
      sn = llist_shift(&freelist, typeof(*sn), lln);
      futexlock_release(&freelist_lock);
    }

    if(!sn) {
      xmalloc(&sn, sizeof(*sn));
      llist_init(&sn->lln);
    }
    sn->type = type;
    sn->p = v;
    ctx.entry = sn;
    hashtable_set(&sel->selected_entities, &ctx);//sn, 0);

    if(sel->iteration_type == SELECTION_ITERATION_TYPE_ORDER)
    {
      llist_append(&sel->list, &sn->lln);
      sn->distance = distance;
    }
    else
    {
      sn->rank = -1;
    }
  }

  if(sel->iteration_type == SELECTION_ITERATION_TYPE_ORDER) {
    return;
  }

  // max rank ever seen for this node
  if(distance > sn->rank)
  {
    if(distance >= sel->ranks_alloc)
    {
      newa = sel->ranks_alloc ?: 5;
      while(newa <= distance) {
        newa += newa * 2 + newa / 2;
      }

      xrealloc(&sel->ranks, sizeof(*sel->ranks), newa, sel->ranks_alloc);
      for(x = sel->ranks_alloc; x < newa; x++) {
        xmalloc(&sel->ranks[x], sizeof(*sel->ranks[x]));
        llist_init(sel->ranks[x]);
      }
      sel->ranks_alloc = newa;
    }
    sel->ranks_len = MAX(sel->ranks_len, distance + 1);
    llist_delete(&sn->lln);
    sn->rank = distance;
    llist_append(sel->ranks[sn->rank], &sn->lln);
  }
}

//
// public
//

void selection_cleanup()
{
  selected *sel;

  while((sel = llist_shift(&freelist, typeof(*sel), lln))) {
    free(sel);
  }
}

void selection_init(selection * restrict sel)
{
  memset(sel, 0, sizeof(*sel));
  hashtable_initx(&sel->selected_entities, 100, selected_entity_hash, selected_entity_cmp, 0);
  llist_init(&sel->list);
  sel->initialized = true;
}

void selection_destroy(selection * restrict sel)
{
  int x;

  if(!sel->initialized) {
    return;
  }

  selection_reset(sel, 0);

  hashtable_destroy(&sel->selected_entities);

  for(x = 0; x < sel->ranks_alloc; x++) {
    free(sel->ranks[x]);
  }

  free(sel->ranks);
}

void selection_create(selection ** restrict selp, selection_iteration_type iteration_type)
{
  xmalloc(selp, sizeof(**selp));
  selection_init(*selp);
  (*selp)->iteration_type = iteration_type;
}

void selection_free(selection * restrict sel)
{
  if(sel) {
    selection_destroy(sel);
  }
  free(sel);
}

void selection_reset(selection * restrict sel, selection_iteration_type iteration_type)
{
  int x;
  llist list;

  hashtable_recycle(&sel->selected_entities);

  llist_init(&list);
  for(x = 0; x < sel->ranks_len; x++) {
    llist_splice_tail(&list, sel->ranks[x]);
  }

  sel->ranks_len = 0;
  llist_splice_tail(&list, &sel->list);

  futexlock_acquire(&freelist_lock);
  llist_splice_tail(&freelist, &list);
  futexlock_release(&freelist_lock);

  sel->iteration_type = iteration_type;
  sel->numranks = 0;
}

void selection_finalize(selection * restrict sel)
{
  int x;
  int32_t rank;
  selected *sn;

  if(sel->iteration_type == SELECTION_ITERATION_TYPE_ORDER) {
    return;
  }

  /* concatenate the ranks into one list */
  for(x = 0; x < sel->ranks_len; x++) {
    llist_splice_head(&sel->list, sel->ranks[x]);
  }
  sel->ranks_len = 0;

  if(llist_empty(&sel->list)) {
    return;
  }

  /* renumber from zero */
  rank = -1;
  llist_foreach(&sel->list, sn, lln) {
    if(sn->rank != rank) {
      rank = sn->rank;
      sel->numranks++;
    }

    sn->rank = sel->numranks - 1;
  }
}

void selection_add_vertex(selection * restrict sel, const moria_vertex * restrict v, uint16_t rank)
{
  selection_add(sel, SELECTED_VERTEX, v, rank);
}

void selection_add_bpe(selection * restrict sel, const buildplan_entity * restrict bpe, uint16_t rank)
{
  selection_add(sel, SELECTED_BPE, bpe, rank);
}

void selection_replicate(selection * restrict dst, selection * restrict src)
{
  selected *ssn;
  selected *dsn;
  hashtable_search_context ctx = { };

  llist_foreach(&src->list, ssn, lln) {
    ctx.entry = ssn;
    if(hashtable_search(&dst->selected_entities, &ctx)) {
      continue;
    }

    if((dsn = llist_shift(&freelist, typeof(*dsn), lln)) == 0)
    {
      xmalloc(&dsn, sizeof(*dsn));
      llist_init(&dsn->lln);
    }
    dsn->p = ssn->p;

    ctx.entry = dsn;
    hashtable_set(&dst->selected_entities, &ctx);
    llist_append(&dst->list, &dsn->lln);
  }
}
