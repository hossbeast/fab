/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

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

#ifndef _FABD_SELECTION_H
#define _FABD_SELECTION_H

#include "xapi.h"
#include "types.h"

#include "valyria/llist.h"

struct node;
struct set;
struct buildplan_entity;

/*
 * stores a set of nodes and provides rank-based traversal, in which the nodes are divided
 * into groups by rank, having the property that for node N in rank X, all of its
 * dependencies are in rank Y, where Y > X.
 *
 */

typedef struct selected_node {
  union {
    void * v;
    struct node * n;
    struct buildplan_entity * bpe;
  };
  int32_t rank;
  llist lln;
} selected_node;

typedef struct selection {
  struct set * selected_nodes;

  llist ** ranks;
  uint16_t ranks_len;
  size_t ranks_alloc;
  bool initialized;

  // after finalize ; this is the list to iterate in rank order
  llist list;           // struct selected_node
  uint16_t numranks;    // ranks are numbered 0 - (numranks - 1)
} selection;

xapi selection_setup(void);
xapi selection_cleanup(void);

xapi selection_create(selection ** restrict selp)
  __attribute__((nonnull));

xapi selection_xfree(selection * restrict sel);

xapi selection_xinit(selection * restrict selp)
  __attribute__((nonnull));

xapi selection_xdestroy(selection * restrict sel)
  __attribute__((nonnull));

xapi selection_add_node(selection * restrict sel, struct node * restrict n, uint16_t rank)
  __attribute__((nonnull));

xapi selection_add_bpe(selection * restrict sel, struct buildplan_entity * restrict bpe, uint16_t rank)
  __attribute__((nonnull));

void selection_finalize(selection * restrict sel)
  __attribute__((nonnull));

xapi selection_reset(selection * restrict sel)
  __attribute__((nonnull));

xapi selection_replicate(selection * restrict dst, selection * restrict src)
  __attribute__((nonnull));

#endif
