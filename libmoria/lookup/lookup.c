/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <inttypes.h>
#include <stdlib.h>

#include "common/attrs.h"
#include "common/hash.h"
#include "narrator.h"
#include "valyria/llist.h"
#include "valyria/map.h"
#include "valyria/rbtree.h"
#include "xlinux/xstdlib.h"

#include "graph.internal.h"
#include "edge.internal.h"
#include "moria.h"
#include "foreach.h"
#include "traverse.internal.h"
#include "vertex.h"
#include "lookup.h"

#include "zcat.h"

void API moria_graph_lookup_sentinel(
    void * restrict _context
  , const char ** restrict label
  , uint16_t * restrict label_len
)
{
  moria_graph_lookup_sentinel_context * context = _context;

  if(label)
  {
    if((*label = context->labels[context->index]))
    {
      *label_len = strlen(*label);
      context->index++;
    }
  }
  else
  {
    // rewind
    context->index = 0;
  }
}

void API moria_graph_lookup(
    moria_graph * restrict g
  , const moria_vertex * restrict index
  , moria_graph_lookup_identifier_callback identifier_callback
  , void (*index_lookup_callback)(
        void * context
      , const char * restrict label
      , uint16_t label_len
      , struct moria_vertex * const restrict v
    )
  , void * restrict context
  , const moria_vertex * restrict V[2]   // (returns) located vertices
  , int * restrict r                     // (returns) return code
)
{
  const char * label;
  uint16_t label_len;
  moria_vertex *v, *p, *i;
  bool matched;
  moria_foreach_state st;
  moria_edge *e;

  *r = 0;

  // get the starting vertex label
  identifier_callback(context, 0, 0);
  identifier_callback(context, &label, &label_len);
  if(!label) {
    return;
  }

  // get the set of starting vertices having the initial label
  i = moria_vertex_downw(index, label, label_len);
  if(index_lookup_callback) {
    index_lookup_callback(context, label, label_len, i);
  }

  if(!i) {
    return;
  }

  moria_foreach_edge_down(&st, i, e) {
    v = e->B;

    /* check whether the sequence of ancestors matches */
    p = v;
    while(1)
    {
      identifier_callback(context, &label, &label_len);
      if(!label) {
        break;
      }

      if((p = moria_vertex_up(p)) == NULL) {
        break;
      }

      if(memncmp(label, label_len, ".", 1) == 0) {
        continue;
      }

      if(memncmp(label, label_len, "..", 2) == 0) {
        if((p = moria_vertex_up(p)) == NULL) {
          break;
        }
        continue;
      }

      if(memncmp(label, label_len, p->label, p->label_len) != 0) {
        break;
      }
    }

    matched = !label;

    // reset the cursor, discard the initial label
    identifier_callback(context, 0, 0);
    identifier_callback(context, &label, &label_len);

    if(!matched) {
      continue;
    }

    V[*r] = v;

    /* identifier matched on multiple nodes */
    if((++(*r)) == 2) {
      break;
    }
  }
}

void API moria_graph_linear_search(
    llist * restrict vertex_list
  , const char * restrict label
  , uint16_t label_len
  , moria_vertex ** restrict rv
)
{
  moria_vertex *v;

  *rv = 0;
  llist_foreach(vertex_list, v, owner) {
    if(memncmp(v->label, v->label_len, label, label_len) == 0) {
      if(*rv) {
        RTA;
      }
      *rv = v;
    }
  }
}
