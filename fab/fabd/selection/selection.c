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

#include "common/hash.h"
#include "moria.h"
#include "valyria/set.h"
#include "xlinux/xstdlib.h"

#include "selection.h"
#include "dependency.h"

static llist selected_entity_freelist = LLIST_INITIALIZER(selected_entity_freelist);

static uint32_t selected_entity_hash(uint32_t h, const void * restrict _val, size_t sz)
{
  selected *sn = (void*)_val;
  return hash32(h, &sn->p, sizeof(sn->p));
}

static int selected_entity_cmp(const void * _A, size_t Az, const void *_B, size_t Bz)
{
  selected *sa = (void*)_A;
  selected *sb = (void*)_B;

  return INTCMP(sa->p, sb->p);
}

static xapi __attribute__((nonnull)) selection_add(selection * restrict sel, void *v, uint16_t distance)
{
  enter;

  selected snk = { p : v };
  selected *sn = 0;
  size_t newa;
  int x;

  if((sn = set_get(sel->selected_entities, &snk, 0)) == 0)
  {
    if((sn = llist_shift(&selected_entity_freelist, typeof(*sn), lln)) == 0)
    {
      fatal(xmalloc, &sn, sizeof(*sn));
      llist_init_node(&sn->lln);
    }
    sn->p = v;
    fatal(set_put, sel->selected_entities, sn, 0);

    if(sel->iteration_type == SELECTION_ITERATION_TYPE_ORDER)
    {
      llist_append(&sel->list, sn, lln);
      sn->distance = distance;
    }
    else
    {
      sn->rank = -1;
    }
  }

  if(sel->iteration_type == SELECTION_ITERATION_TYPE_ORDER) {
    goto end;
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

      fatal(xrealloc, &sel->ranks, sizeof(*sel->ranks), newa, sel->ranks_alloc);
      for(x = sel->ranks_alloc; x < newa; x++) {
        fatal(xmalloc, &sel->ranks[x], sizeof(*sel->ranks[x]));
        llist_init_node(sel->ranks[x]);
      }
      sel->ranks_alloc = newa;
    }
    sel->ranks_len = MAX(sel->ranks_len, distance + 1);
    llist_delete(sn, lln);
    sn->rank = distance;
    llist_append(sel->ranks[sn->rank], sn, lln);
  }

end:
  sn = 0;

finally:
  free(sn);
coda;
}

//
// public
//

xapi selection_cleanup()
{
  enter;

  selected *sel;
  llist *tmp;

  llist_foreach_safe(&selected_entity_freelist, sel, lln, tmp) {
    free(sel);
  }

  finally : coda;
}

xapi selection_xinit(selection * restrict sel)
{
  enter;

  memset(sel, 0, sizeof(*sel));
  fatal(set_createx, &sel->selected_entities, 100, selected_entity_hash, selected_entity_cmp, 0, 0);
  llist_init_node(&sel->list);
  sel->initialized = true;

  finally : coda;
}

xapi selection_xdestroy(selection * restrict sel)
{
  enter;

  int x;

  if(!sel->initialized) {
    goto XAPI_FINALIZE;
  }

  fatal(selection_reset, sel, 0);

  fatal(set_xfree, sel->selected_entities);

  for(x = 0; x < sel->ranks_alloc; x++) {
    free(sel->ranks[x]);
  }

  free(sel->ranks);

  finally : coda;
}

xapi selection_create(selection ** restrict selp, selection_iteration_type iteration_type)
{
  enter;

  fatal(xmalloc, selp, sizeof(**selp));
  fatal(selection_xinit, *selp);
  (*selp)->iteration_type = iteration_type;

  finally : coda;
}

xapi selection_xfree(selection * restrict sel)
{
  enter;

  if(sel) {
    fatal(selection_xdestroy, sel);
  }
  free(sel);

  finally : coda;
}

xapi selection_reset(selection * restrict sel, selection_iteration_type iteration_type)
{
  enter;

  int x;

  fatal(set_recycle, sel->selected_entities);

  for(x = 0; x < sel->ranks_len; x++) {
    llist_splice_tail(&selected_entity_freelist, sel->ranks[x]);
  }

  sel->ranks_len = 0;
  llist_splice_tail(&selected_entity_freelist, &sel->list);

  sel->iteration_type = iteration_type;

  finally : coda;
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

xapi selection_add_vertex(selection * restrict sel, moria_vertex * restrict v, uint16_t rank)
{
  enter;

  fatal(selection_add, sel, v, rank);

  finally : coda;
}

xapi selection_add_dependency(selection * restrict sel, dependency * restrict bpe, uint16_t rank)
{
  enter;

  fatal(selection_add, sel, bpe, rank);

  finally : coda;
}

xapi selection_replicate(selection * restrict dst, selection * restrict src)
{
  enter;

  selected snk;
  selected *sn;
  selected *ssn;

  llist_foreach(&src->list, ssn, lln) {
    snk.p = ssn->p;
    if((sn = set_get(dst->selected_entities, &snk, 0))) {
      continue;
    }

    if((sn = llist_shift(&selected_entity_freelist, typeof(*sn), lln)) == 0)
    {
      fatal(xmalloc, &sn, sizeof(*sn));
      llist_init_node(&sn->lln);
    }
/* this is wrong */
    sn->p = ssn->p;
    fatal(set_put, dst->selected_entities, sn, 0);
    llist_append(&dst->list, sn, lln);
  }

  finally : coda;
}
