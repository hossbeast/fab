/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "valyria/llist.h"
#include "valyria/hashtable.h"

struct buildplan_entity;
struct moria_vertex;

/*
 * A selection is a set of nodes grouped by rank. For two nodes X and Y, the rank of X denoted
 * Xr < the rank of Y denoted Yr, iff Xd < Yd, where Xd is the depth at which X was visited and
 * Yd is the depth at which Y was visited. A selection supports rank-based traversal.
 */

enum selected_type {
  SELECTED_VERTEX,
  SELECTED_BPE,
};

typedef struct selected {
  enum selected_type type;
  union {
    const void *p;
    struct moria_vertex *v;
    struct buildplan_entity *bpe;
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
  hashtable selected_entities;

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

void selection_free(selection * restrict sel);

void selection_init(selection * restrict selp)
  __attribute__((nonnull));

void selection_destroy(selection * restrict sel)
  __attribute__((nonnull));

void selection_add_vertex(selection * restrict sel, const struct moria_vertex* restrict v, uint16_t distance)
  __attribute__((nonnull));

void selection_add_bpe(selection * restrict sel, const struct buildplan_entity * restrict bpe, uint16_t distance)
  __attribute__((nonnull));

void selection_finalize(selection * restrict sel)
  __attribute__((nonnull));

void selection_reset(selection * restrict sel, selection_iteration_type type)
  __attribute__((nonnull));

void selection_replicate(selection * restrict dst, selection * restrict src)
  __attribute__((nonnull));
