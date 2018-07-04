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

static bool __attribute__((nonnull(1, 2))) vertex_down(
    /* 1 */ const vertex * restrict v
  , /* 2 */ const char * restrict label
  , /* 3 */ uint16_t label_len
  , /* 4 */ vertex ** restrict rv
  , /* 5 */ edge ** restrict re
)
{
  struct edge_key_compare_label_context ctx = { B : label, len : label_len };
  edge * e = list_search_range(v->down, 0, v->down_partition, &ctx, edge_key_compare_label);

  if(!e)
    return false;

  if(rv)
    *rv = e->B;
  if(re)
    *re = e;

  return true;
}

static xapi __attribute__((nonnull)) vertex_alloc(
    vertex ** const restrict v
  , graph * restrict g
  , size_t vsz
  , uint32_t attrs
)
{
  enter;

  fatal(xmalloc, v, sizeof(**v) + vsz);
  (*v)->graph = g;
  fatal(list_create, &(*v)->up);
  fatal(list_create, &(*v)->down);
  (*v)->attrs = attrs;

  finally : coda;
}

//
// public
//

xapi vertex_xfree(vertex * const restrict v)
{
  enter;

  if(v)
  {
    fatal(list_xfree, v->up);
    fatal(list_xfree, v->down);
  }

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

//
// api
//

API xapi vertex_creates(struct vertex ** const restrict v, graph * const restrict g, uint32_t attrs, const char * const restrict label)
{
  enter;

  vertex * lv = 0;
  fatal(vertex_alloc, &lv, g, g->vsz, attrs);
  lv->label = label;
  lv->label_len = strlen(label);

  fatal(graph_vertex_push, g, lv);
  *v = lv;
  lv = 0;

  if(g->identity)
    fatal(graph_identity_indexs, g, *v, label);

finally:
  fatal(vertex_xfree, lv);
coda;
}

API xapi vertex_createw(struct vertex ** const restrict v, graph * const restrict g, uint32_t attrs, const char * const restrict label, uint16_t label_len)
{
  enter;

  vertex * lv = 0;
  fatal(vertex_alloc, &lv, g, g->vsz, attrs);
  lv->label = label;
  lv->label_len = label_len;

  fatal(graph_vertex_push, g, lv);
  *v = lv;
  lv = 0;

  if(g->identity)
    fatal(graph_identity_indexw, g, *v, label, label_len);

finally:
  fatal(vertex_xfree, lv);
coda;
}

API xapi vertex_create(struct vertex ** const restrict v, graph * const restrict g, uint32_t attrs)
{
  enter;

  vertex * lv = 0;
  fatal(vertex_alloc, &lv, g, g->vsz, attrs);

  fatal(graph_vertex_push, g, lv);
  *v = lv;
  lv = 0;

finally:
  fatal(vertex_xfree, lv);
coda;
}

API void vertex_value_set(vertex * const restrict v, void * value, size_t vsz)
{
  memcpy(v->value, value, vsz);
}

API void * vertex_value(const vertex * const restrict v)
{
  return (void*)v->value;
}

API vertex * vertex_containerof(const void * value)
{
  return (vertex*)(value - offsetof(vertex, value));
}

API vertex * vertex_downs(const vertex * restrict v, const char * restrict label)
{
  vertex * nv;
  if(vertex_down(v, label, strlen(label), &nv, 0))
    return nv;

  return 0;
}

API vertex * vertex_downw(const vertex * restrict v, const char * restrict label, uint16_t label_len)
{
  vertex * nv;
  if(vertex_down(v, label, label_len, &nv, 0))
    return nv;

  return 0;
}

API edge * vertex_edge_downs(const vertex * restrict v, const char * restrict label)
{
  edge * e;
  if(vertex_down(v, label, strlen(label), 0, &e))
    return e;

  return 0;
}

API edge * vertex_edge_downw(const vertex * restrict v, const char * restrict label, uint16_t label_len)
{
  edge * e;
  if(vertex_down(v, label, label_len, 0, &e))
    return e;

  return 0;
}

API vertex * vertex_up(const vertex * restrict v)
{
  if(v->up_partition)
    return ((edge*)list_get(v->up, 0))->A;

  return NULL;
}

API edge * vertex_edge_up(const vertex * restrict v)
{
  if(v->up_partition)
    return list_get(v->up, 0);

  return NULL;
}
