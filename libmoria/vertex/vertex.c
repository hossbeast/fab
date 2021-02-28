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

#include "vertex.internal.h"
#include "attr.internal.h"
#include "edge.internal.h"
#include "graph.internal.h"

#include "macros.h"

//
// static
//

static bool __attribute__((nonnull)) vertex_down(
    const moria_vertex * restrict v
  , const char * restrict label
  , uint16_t label_len
  , moria_edge ** restrict re
)
{
  rbnode *rbn;
  edge_key key;

  key = (typeof(key)) {
      attrs : MORIA_EDGE_IDENTITY
    , Blist : (moria_vertex*[]) { (moria_vertex[]) {{ label : label, label_len : label_len }} }
    , Blen : 1
  };

  if((rbn = rbtree_lookup_node(&v->down, &key, edge_cmp_key_down)))
  {
    *re = containerof(rbn, moria_edge, rbn_down);
    return true;
  }

  return false;
}

//
// api
//

void API moria_vertex_init(moria_vertex * const restrict v, moria_graph * const restrict g, uint32_t attrs)
{
  graph_vertex_init(g, v);
  rbtree_init(&v->up);
  rbtree_init(&v->down);
  llist_init_node(&v->lln);

  v->attrs = attrs;

  //*rv = &v->vx;
  //v = 0;
}

void API moria_vertex_initw(moria_vertex * const restrict v, moria_graph * const restrict g, uint32_t attrs, const char * const restrict label, uint16_t label_len)
{
  moria_vertex_init(v, g, attrs);

  v->label = label;
  v->label_len = label_len;
}

void API moria_vertex_inits(moria_vertex * const restrict rv, moria_graph * const restrict g, uint32_t attrs, const char * const restrict label)
{
  moria_vertex_initw(rv, g, attrs, label, strlen(label));
}

moria_vertex * API moria_vertex_downs(const moria_vertex * restrict v, const char * restrict label)
{
  moria_edge * e;

  if(vertex_down(v, label, strlen(label), &e))
    return e->B;

  return 0;
}

moria_vertex * API moria_vertex_downw(const moria_vertex * restrict v, const char * restrict label, uint16_t label_len)
{
  moria_edge * e;

  if(vertex_down(v, label, label_len, &e))
    return e->B;

  return 0;
}

moria_edge * API moria_vertex_edge_downs(const moria_vertex * restrict v, const char * restrict label)
{
  moria_edge * e;

  if(vertex_down(v, label, strlen(label), &e))
    return e;

  return 0;
}

moria_edge * API moria_vertex_edge_downw(const moria_vertex * restrict v, const char * restrict label, uint16_t label_len)
{
  moria_edge * e;

  if(vertex_down(v, label, label_len, &e))
    return e;

  return 0;
}

moria_vertex * API moria_vertex_up(const moria_vertex * restrict v)
{
 if(v->up_identity)
    return v->up_identity->A;

  return 0;
}

moria_edge * API moria_vertex_edge_up(const moria_vertex * restrict v)
{
  if(v->up_identity)
    return v->up_identity;

  return 0;
}

moria_vertex * API moria_vertex_descendw(const moria_vertex * restrict v, const char * restrict path, uint16_t path_len)
{
  const char *name, *next;
  uint16_t name_len, len;

  name = path;
  len = path_len;
  while(v && len)
  {
    if((next = strchr(name, '/')))
    {
      name_len = (next - name);
    }
    else
    {
      name_len = len;
    }

    v = moria_vertex_downw(v, name, name_len);
    len -= name_len;
  }

  if(len)
  {
    v = 0;
  }

  return (moria_vertex*)v;
}

moria_vertex * API moria_vertex_descends(const moria_vertex * restrict vx, const char * restrict path)
{
  return moria_vertex_descendw(vx, path, strlen(path));
}

#if 0
void API vertex_value_set(moria_vertex * const restrict vx, graph * restrict g, void * value)
{
  moria_vertex * v = containerof(vx, moria_vertex, vx);
  memcpy(v->value, value, g->vsz);
}

void * API vertex_value(const moria_vertex * const restrict vx)
{
  if(!vx)
    return 0;

  const moria_vertex * v = containerof(vx, moria_vertex, vx);

  return (void*)v->value;
}

moria_vertex * API vertex_containerof(const void * value)
{
  if(!value)
    return 0;

  return (vertex*)(value - offsetof(moria_vertex, value));
}


xapi vertex_xfree(moria_vertex * const restrict v)
{
  enter;

  wfree(v);

  finally : coda;
}

xapi vertex_ixfree(moria_vertex ** const restrict v)
{
  enter;

  fatal(vertex_xfree, *v);
  *v = 0;

  finally : coda;
}

xapi API moria_vertex_delete(moria_vertex * restrict v, graph * restrict g)
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

#if 0
  if((v = llist_shift(&g->vertex_freelist, moria_vertex, graph_lln)) == 0)
  {
    fatal(xmalloc, &v, sizeof(*v) + g->vsz);
    graph_vertex_init(g, v);
  }
  else
  {
    memset(v->value, 0, g->vsz);
    /* Normally, the application sets the label after allocating the vertex, but there are use cases for
     * vertices with no label (rules in bamd). */
    v->label = 0;
    v->label_len = 0;
  }
  llist_append(&g->vertices, v, graph_lln);
#endif
#endif
