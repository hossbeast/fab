/* Copyright (c) 2012-2018 Todd Freed <todd.freed@gmail.com>

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

#include <inttypes.h>

#include "valyria/set.h"
#include "xlinux/xstdlib.h"

#include "selection.h"
#include "node.h"

#include "common/hash.h"
#include "common/assure.h"

static llist selected_node_freelist = LLIST_INITIALIZER(selected_node_freelist);

static uint32_t selected_node_hash(uint32_t h, const void * restrict _val, size_t sz)
{
  selected_node *sn = (void*)_val;
  return hash32(h, &sn->v, sizeof(sn->v));
}

static int selected_node_cmp(const void * _A, size_t Az, const void *_B, size_t Bz)
{
  selected_node *sa = (void*)_A;
  selected_node *sb = (void*)_B;

  return INTCMP(sa->v, sb->v);
}

static xapi __attribute__((nonnull)) selection_add(selection * restrict sel, void *v, uint16_t rank)
{
  enter;

  selected_node snk = { v : v };
  selected_node *sn = 0;
  size_t newa;
  int x;

  if((sn = set_get(sel->selected_nodes, &snk, 0)) == 0)
  {
    if((sn = llist_shift(&selected_node_freelist, typeof(*sn), lln)) == 0)
    {
      fatal(xmalloc, &sn, sizeof(*sn));
      llist_init_node(&sn->lln);
    }
    sn->v = v;
    sn->rank = -1;
    fatal(set_put, sel->selected_nodes, sn, 0);
  }

  // max rank ever seen for this node
  if(rank > sn->rank)
  {
    if(rank >= sel->ranks_alloc)
    {
      newa = sel->ranks_alloc ?: 5;
      while(newa <= rank) {
        newa += newa * 2 + newa / 2;
      }

      fatal(xrealloc, &sel->ranks, sizeof(*sel->ranks), newa, sel->ranks_alloc);
      for(x = sel->ranks_alloc; x < newa; x++) {
        fatal(xmalloc, &sel->ranks[x], sizeof(*sel->ranks[x]));
        llist_init_node(sel->ranks[x]);
      }
      sel->ranks_alloc = newa;
    }
    sel->ranks_len = MAX(sel->ranks_len, rank + 1);
    llist_delete(sn, lln);
    sn->rank = rank;
    llist_append(sel->ranks[sn->rank], sn, lln);
  }
  sn = 0;

finally:
  free(sn);
coda;
}

//
//
//

xapi selection_setup()
{
  enter;

  finally : coda;
}

xapi selection_cleanup()
{
  enter;

  selected_node *sel;
  llist *tmp;

  llist_foreach_safe(&selected_node_freelist, sel, lln, tmp) {
    free(sel);
  }

  finally : coda;
}

//
//
//

xapi selection_xinit(selection * restrict sel)
{
  enter;

  fatal(set_createx, &sel->selected_nodes, 100, selected_node_hash, selected_node_cmp, 0, 0);
  llist_init_node(&sel->list);
  sel->initialized = true;

  finally : coda;
}

xapi selection_xdestroy(selection * restrict sel)
{
  enter;

  int x;

  if(!sel->initialized)
    goto XAPI_FINALIZE;

  fatal(selection_reset, sel);

  fatal(set_xfree, sel->selected_nodes);

  for(x = 0; x < sel->ranks_alloc; x++) {
    free(sel->ranks[x]);
  }

  free(sel->ranks);

  finally : coda;
}

xapi selection_create(selection ** restrict selp)
{
  enter;

  fatal(xmalloc, selp, sizeof(**selp));
  fatal(selection_xinit, *selp);

  finally : coda;
}

xapi selection_xfree(selection * restrict sel)
{
  enter;

  if(sel)
    fatal(selection_xdestroy, sel);
  free(sel);

  finally : coda;
}

xapi selection_reset(selection * restrict sel)
{
  enter;

  int x;

  fatal(set_recycle, sel->selected_nodes);

  for(x = 0; x < sel->ranks_len; x++) {
    llist_splice_tail(&selected_node_freelist, sel->ranks[x]);
  }

  sel->ranks_len = 0;
  llist_splice_tail(&selected_node_freelist, &sel->list);

  finally : coda;
}

void selection_finalize(selection * restrict sel)
{
  int x;
  int32_t rank;
  selected_node *sn;

  /* concatenate the ranks into one list */
  for(x = 0; x < sel->ranks_len; x++)
  {
    llist_splice_head(&sel->list, sel->ranks[x]);
  }
  sel->ranks_len = 0;

  if(llist_empty(&sel->list)) {
    return;
  }

  /* renumber from zero */
  sel->numranks = 0;
  rank = -1;
  llist_foreach(&sel->list, sn, lln) {
    if(sn->rank != rank) {
      rank = sn->rank;
      sel->numranks++;
    }

    sn->rank = sel->numranks - 1;
  }
}

xapi selection_add_node(selection * restrict sel, node * restrict n, uint16_t rank)
{
  enter;

  fatal(selection_add, sel, n, rank);

  finally : coda;
}

xapi selection_add_bpe(selection * restrict sel, buildplan_entity * restrict bpe, uint16_t rank)
{
  enter;

  fatal(selection_add, sel, bpe, rank);

  finally : coda;
}

xapi selection_replicate(selection * restrict dst, selection * restrict src)
{
  enter;

  selected_node snk;
  selected_node *sn;
  selected_node *ssn;

  llist_foreach(&src->list, ssn, lln) {
    snk.v = ssn->v;
    if((sn = set_get(dst->selected_nodes, &snk, 0)))
      continue;

    if((sn = llist_shift(&selected_node_freelist, typeof(*sn), lln)) == 0)
    {
      fatal(xmalloc, &sn, sizeof(*sn));
      llist_init_node(&sn->lln);
    }
    sn->v = ssn->v;
    fatal(set_put, dst->selected_nodes, sn, 0);
    llist_append(&dst->list, sn, lln);
  }

  finally : coda;
}
