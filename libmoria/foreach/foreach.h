/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct moria_vertex;
struct moria_edge;
struct rbnode;

typedef struct moria_foreach_state {
  struct moria_edge *next;
  struct rbnode *rbn;
  bool down;
} moria_foreach_state;

void moria_foreach_edge_up_init(moria_foreach_state * restrict s, const struct moria_vertex * restrict v)
  __attribute__((nonnull));

void moria_foreach_edge_up_next(moria_foreach_state * restrict s,  const struct moria_vertex * restrict v)
  __attribute__((nonnull));

#define moria_foreach_edge_up(s, v, e)  \
  for(                                  \
    moria_foreach_edge_up_init(s, v);   \
    (e = (s)->next);                    \
    moria_foreach_edge_up_next(s, v)    \
  )

void moria_foreach_edge_down_init(moria_foreach_state * restrict s, const struct moria_vertex * restrict v)
  __attribute__((nonnull));

void moria_foreach_edge_down_next(moria_foreach_state * restrict s, const struct moria_vertex * restrict v)
  __attribute__((nonnull));

#define moria_foreach_edge_down(s, v, e) \
  for (                                  \
    moria_foreach_edge_down_init(s, v);  \
    (e = (s)->next);                     \
    moria_foreach_edge_down_next(s, v)   \
  )

void moria_foreach_edge_init(moria_foreach_state * restrict s, const struct moria_vertex * restrict v)
  __attribute__((nonnull));

void moria_foreach_edge_next(moria_foreach_state * restrict s, const struct moria_vertex * restrict v)
  __attribute__((nonnull));

#define moria_foreach_edge(s, v, e)     \
  for (                                 \
    moria_foreach_edge_init(s, v);      \
    (e = (s)->next);                    \
    moria_foreach_edge_next(s, v)       \
  )
