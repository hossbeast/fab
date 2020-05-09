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

#include <string.h>
#include <stdio.h>

#include "xapi.h"
#include "types.h"

#include "xlinux/xstdlib.h"
#include "valyria/list.h"

#include "internal.h"
#include "vertex.internal.h"
#include "attr.internal.h"
#include "edge.internal.h"
#include "graph.internal.h"

#include "macros.h"

//
// static
//

static bool __attribute__((nonnull)) vertex_down(
    const vertex_t * restrict v
  , const char * restrict label
  , uint16_t label_len
  , edge ** restrict re
)
{
  rbnode *rbn;
  edge_key key;

  key = (typeof(key)) {
      attrs : MORIA_EDGE_IDENTITY
    , Blist : (vertex*[]) { (vertex[]) { { label : label, label_len : label_len }} }
    , Blen : 1
  };

  if((rbn = rbtree_lookup_node(&v->down, &key, edge_cmp_key_down)))
  {
    *re = containerof(rbn, edge, rbn_down);
    return true;
  }

  return false;
}

//
// public
//

xapi vertex_xfree(vertex * const restrict v)
{
  enter;

  wfree(v);

  finally : coda;
}

xapi vertex_ixfree(vertex ** const restrict v)
{
  enter;

  fatal(vertex_xfree, *v);
  *v = 0;

  finally : coda;
}

API xapi vertex_delete(vertex_t * restrict v, graph * restrict g)
{
  enter;

RUNTIME_ASSERT(rbtree_empty(&v->up));
RUNTIME_ASSERT(rbtree_empty(&v->down));
RUNTIME_ASSERT(v->up_identity == NULL);

  // free all the values
  if(g->vertex_value_destroy || g->vertex_value_xdestroy)
  {
    if(g->vertex_value_destroy)
      g->vertex_value_destroy(v->value);
    else
      fatal(g->vertex_value_xdestroy, v->value);
  }

  // push onto the freelist
  llist_delete(v, graph_lln);
  llist_append(&g->vertex_freelist, v, graph_lln);

  finally : coda;
}

//
// api
//

API xapi vertex_create(vertex ** const restrict rv, graph * const restrict g, uint32_t attrs)
{
  enter;

  vertex_t * v = 0;

  if((v = llist_shift(&g->vertex_freelist, vertex_t, graph_lln)) == 0)
  {
    fatal(xmalloc, &v, sizeof(*v) + g->vsz);
    graph_vertex_init(g, v);
  }
  else
  {
    memset(v->value, 0, g->vsz);
  }
  llist_append(&g->vertices, v, graph_lln);

  rbtree_init(&v->up);
  rbtree_init(&v->down);
  llist_init_node(&v->lln);

  v->attrs = attrs;

  *rv = &v->vx;
  v = 0;

  finally : coda;
}

API xapi vertex_createw(vertex ** const restrict rv, graph * const restrict g, uint32_t attrs, const char * const restrict label, uint16_t label_len)
{
  enter;

  fatal(vertex_create, rv, g, attrs);

  (*rv)->label = label;
  (*rv)->label_len = label_len;

  if(g->identity)
    fatal(graph_identity_indexs, g, *rv, label);

  finally : coda;
}

API xapi vertex_creates(vertex ** const restrict rv, graph * const restrict g, uint32_t attrs, const char * const restrict label)
{
  enter;

  fatal(vertex_createw, rv, g, attrs, label, strlen(label));

  finally : coda;
}

API void vertex_value_set(vertex * const restrict vx, graph * restrict g, void * value)
{
  vertex_t * v = containerof(vx, vertex_t, vx);
  memcpy(v->value, value, g->vsz);
}

API void * vertex_value(const vertex * const restrict vx)
{
  if(!vx)
    return 0;

  const vertex_t * v = containerof(vx, vertex_t, vx);

  return (void*)v->value;
}

API vertex * vertex_containerof(const void * value)
{
  if(!value)
    return 0;

  return (vertex*)(value - offsetof(vertex_t, value));
}

API vertex * vertex_downs(const vertex * restrict vx, const char * restrict label)
{
  const vertex_t * v;
  edge * e;

  v = containerof(vx, vertex_t, vx);
  if(vertex_down(v, label, strlen(label), &e))
    return e->B;

  return 0;
}

API vertex * vertex_downw(const vertex * restrict vx, const char * restrict label, uint16_t label_len)
{
  const vertex_t * v;
  edge * e;

  v = containerof(vx, vertex_t, vx);
  if(vertex_down(v, label, label_len, &e))
    return e->B;

  return 0;
}

API edge * vertex_edge_downs(const vertex * restrict vx, const char * restrict label)
{
  const vertex_t * v;
  edge * e;

  v = containerof(vx, vertex_t, vx);
  if(vertex_down(v, label, strlen(label), &e))
    return e;

  return 0;
}

API edge * vertex_edge_downw(const vertex * restrict vx, const char * restrict label, uint16_t label_len)
{
  const vertex_t * v = containerof(vx, vertex_t, vx);

  edge * e;
  if(vertex_down(v, label, label_len, &e))
    return e;

  return 0;
}

API vertex * vertex_up(const vertex * restrict vx)
{
  const vertex_t * v = containerof(vx, vertex_t, vx);

 if(v->up_identity)
    return v->up_identity->A;

  return 0;
}

API edge * vertex_edge_up(const vertex * restrict vx)
{
  const vertex_t * v = containerof(vx, vertex_t, vx);

  if(v->up_identity)
    return v->up_identity;

  return 0;
}
