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

#ifndef _MORIA_EDGE_INTERNAL_H
#define _MORIA_EDGE_INTERNAL_H

#include "types.h"
#include "valyria/rbtree.h"

#include "edge.h"

struct moria_vertex;

typedef struct edge_key {
  uint32_t attrs;
  struct moria_vertex ** Alist;   // sorted in ptr order
  uint16_t Alen;
  struct moria_vertex ** Blist;   // sorted in ptr order
  uint16_t Blen;
} edge_key;

int edge_cmp_up(const struct moria_edge * restrict a, const struct moria_edge * restrict b)
  __attribute__((nonnull));

int edge_cmp_rbn_up(const rbnode * restrict an, const rbnode * restrict bn)
  __attribute__((nonnull));

int edge_cmp_key_up(void * restrict key, const rbnode * restrict bn)
  __attribute__((nonnull));

int edge_cmp_down(const struct moria_edge * restrict a, const struct moria_edge * restrict b)
  __attribute__((nonnull));

int edge_cmp_key_down(void * restrict key, const rbnode * restrict rbn)
  __attribute__((nonnull));

int edge_cmp_rbn_down(const rbnode * restrict an, const rbnode * restrict bn)
  __attribute__((nonnull));

#endif
