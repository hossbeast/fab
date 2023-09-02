/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "narrator/fixed.h"
#include "xlinux/xstdlib.h"
#include "moria/vertex.h"
#include "common/attrs.h"

#include "anchor.h"
#include "graph.h"
#include "pattern.h"

#include "macros.h"

llist anchor_list = LLIST_INITIALIZER(anchor_list);                 // active anchor
static llist anchor_freelist = LLIST_INITIALIZER(anchor_freelist);  // freelist

void anchor_mk(anchor ** restrict anchorp, struct pattern * lookup)
{
  anchor * r;

  if((r = llist_shift(&anchor_freelist, typeof(*r), owner)) == 0) {
    xmalloc(&r, sizeof(*r));
  }

  llist_append(&anchor_list, &r->owner);

  r->lookup = lookup;

  *anchorp = r;
}

void anchor_dispose(anchor * restrict anchor)
{
  pattern_free(anchor->lookup);

  llist_delete(&anchor->owner);
  llist_push(&anchor_freelist, &anchor->owner);
}

void anchor_release(anchor * restrict anchor)
{
  if(!anchor) {
    return;
  }
  anchor_dispose(anchor);
}

void anchor_release_bulk(llist * restrict list)
{
  anchor *anchor;

  while((anchor = llist_shift(list, typeof(*anchor), owner))) {
    anchor_release(anchor);
  }
}

void anchor_dispose_bulk(llist * restrict list)
{
  anchor *anchor;

  while((anchor = llist_shift(list, typeof(*anchor), owner))) {
    anchor_dispose(anchor);
  }
}

void anchor_say(const anchor * restrict b, narrator * restrict N)
{
  xsays("anchor");
  xsays(" ");

  pattern_say(b->lookup, N);
}
