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

#include "types.h"

#include "valyria/llist.h"

struct dependency;
struct moria_vertex;
struct set;

/*
 * A selection is a set of nodes grouped by rank. For two nodes X and Y, the rank of X denoted
 * Xr < the rank of Y denoted Yr, iff Xd < Yd, where Xd is the depth at which X was visited and
 * Yd is the depth at which Y was visited. A selection supports rank-based traversal.
 */

typedef struct selected {
  union {
    void * p;
    struct moria_vertex * v;
    struct dependency * bpe;
  };
  union {
    int32_t rank;
    uint16_t distance;
  };
  llist lln;
} selected;

typedef enum selection_iteration_type {
  SELECTION_ITERATION_TYPE_RANK,
  SELECTION_ITERATION_TYPE_ORDER,
} selection_iteration_type;

typedef struct selection {
  struct set * selected_entities;

  selection_iteration_type iteration_type;
  llist ** ranks;
  uint16_t ranks_len;
  size_t ranks_alloc;
  bool initialized;

  // after finalize ; this is the list to iterate in rank order
  llist list;           // struct selected
  uint16_t numranks;    // ranks are numbered 0 - (numranks - 1)
} selection;

void selection_cleanup(void);

void selection_create(selection ** restrict selp, selection_iteration_type type)
  __attribute__((nonnull));

void selection_xfree(selection * restrict sel);

void selection_xinit(selection * restrict selp)
  __attribute__((nonnull));

void selection_xdestroy(selection * restrict sel)
  __attribute__((nonnull));

void selection_add_vertex(selection * restrict sel, struct moria_vertex* restrict v, uint16_t distance)
  __attribute__((nonnull));

void selection_add_dependency(selection * restrict sel, struct dependency * restrict bpe, uint16_t distance)
  __attribute__((nonnull));

void selection_finalize(selection * restrict sel)
  __attribute__((nonnull));

void selection_reset(selection * restrict sel, selection_iteration_type type)
  __attribute__((nonnull));

void selection_replicate(selection * restrict dst, selection * restrict src)
  __attribute__((nonnull));

#endif
