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

#ifndef _MORIA_EDGE_H
#define _MORIA_EDGE_H

/*

MODULE
 edge

SUMMARY

REMARKS

*/

#include "xapi.h"
#include "types.h"

#include "valyria/llist.h"
#include "valyria/rbtree.h"

struct graph;
struct vertex;

#define MORIA_EDGE_HYPER    0x80000000  /* set for a many-to-many hyperedge */
#define MORIA_EDGE_IDENTITY 0x40000000  /* set for an identity edge */

/* all edges where Alen == Blen == 1 do NOT have the hyper-edge bit set */

typedef struct edge {
  uint32_t attrs;            // properties of the edge

  union {
    struct {
      rbnode rbn_up;
      struct vertex *A;      // A depends on B, A -> B
    };

    /* hyperedge */
    struct {
      struct {
        struct vertex *v;
        rbnode rbn;
      } *Alist;
      uint16_t Alen;
    };
  };

  union {
    struct {
      rbnode rbn_down;
      struct vertex *B;      // A depends on B, A -> B, A is up, B is down
    };

    /* hyperedge */
    struct {
      struct {
        struct vertex *v;
        rbnode rbn;
      } *Blist;
      uint16_t Blen;
    };
  };

  llist graph_lln;    // graph list/freelist
  llist lln;          // user use
} edge;

edge * edge_between(const struct vertex * restrict A, const struct vertex * restrict B)
  __attribute__((nonnull));

edge * edge_of(struct vertex ** restrict Alist, uint16_t Alen, struct vertex ** restrict Blist, uint16_t Blen)
  __attribute__((nonnull));

/// edge_value_set
//
// SUMMARY
//  set the value for the edge
//
// REMARKS
//  size must match that provided in graph_create
//
// PARAMETERS
//  v       - pointer to edge
//  [value] - pointer to value
//
void edge_value_set(edge * const restrict v, struct graph * restrict g, void * value)
  __attribute__((nonnull));

/// edge_value
//
// SUMMARY
//  get a pointer to the value for the edge
//
void * edge_value(const edge * const restrict v);

/// edge_containerof
//
// SUMMARY
//  get a pointer to the edge containing the user value
//
edge * edge_containerof(const void * value);

#endif
