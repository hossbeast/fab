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

#include "vertex.h"
#include "edge.internal.h"
#include "graph.internal.h"
#include "moria.h"

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
