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

#include "xapi.h"
#include "types.h"

#include "edge.h"
#include "traverse.internal.h"

#include "macros.h"

struct vertex;

typedef struct edge_t {
  union {
    edge ex;
    struct {
      fieldof(edge, attrs);
      union {
        struct {
          fieldof(edge, rbn_up);
          fieldof(edge, A);
        };
        struct {
          fieldof(edge, Alist);
          fieldof(edge, Alen);
        };
      };
      union {
        struct {
          fieldof(edge, rbn_down);
          fieldof(edge, B);
        };
        struct {
          fieldof(edge, Blist);
          fieldof(edge, Blen);
        };
      };

      fieldof(edge, graph_lln);
      fieldof(edge, lln);
    };
  };

//  size_t up_index;
//  size_t down_index;
//  size_t edges_index;

  entity ent;   // traversable entity
  char value[];
} edge_t;

STATIC_ASSERT(offsetof(edge_t, value) % 8 == 0);

ALIGNEDOF(edge, edge_t, rbn_up);
ALIGNEDOF(edge, edge_t, A);
ALIGNEDOF(edge, edge_t, Alist);
ALIGNEDOF(edge, edge_t, rbn_down);
ALIGNEDOF(edge, edge_t, B);
ALIGNEDOF(edge, edge_t, Blist);
ALIGNEDOF(edge, edge_t, attrs);
ALIGNEDOF(edge, edge_t, graph_lln);

xapi edge_alloc(struct graph * restrict g, edge_t ** restrict re)
  __attribute__((nonnull));

void edge_free(edge_t * restrict e);


typedef struct edge_key {
  uint32_t attrs;
  struct vertex ** Alist;   // sorted in ptr order
  uint16_t Alen;
  struct vertex ** Blist;   // sorted in ptr order
  uint16_t Blen;

  uint32_t cmp_attrs;       // possibly contains NOFOLLOW
} edge_key;

int edge_cmp_up(const edge_t * restrict a, const edge_t * restrict b)
  __attribute__((nonnull));

int edge_cmp_rbn_up(const rbnode * restrict an, const rbnode * restrict bn)
  __attribute__((nonnull));

int edge_cmp_key_up(void * restrict key, const rbnode * restrict bn)
  __attribute__((nonnull));

int edge_cmp_down(const edge_t * restrict a, const edge_t * restrict b)
  __attribute__((nonnull));

int edge_cmp_key_down(void * restrict key, const rbnode * restrict rbn)
  __attribute__((nonnull));

int edge_cmp_rbn_down(const rbnode * restrict an, const rbnode * restrict bn)
  __attribute__((nonnull));

#endif
