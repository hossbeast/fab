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
#include <stddef.h>

#include "xapi.h"
#include "types.h"

#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"

#include "valyria/list.h"

#include "internal.h"
#include "graph.internal.h"
#include "vertex.internal.h"
#include "edge.internal.h"
#include "attr.internal.h"

#include "macros.h"

#define restrict __restrict

//
// static
//

static bool __attribute__((nonnull(1))) vertex_travel(
    /* 1 */ const vertex * restrict v
  , /* 2 */ const char * restrict label
  , /* 3 */ size_t label_len
  , /* 4 */ uint32_t vertex_visit
  , /* 5 */ uint32_t edge_visit
  , /* 6 */ uint32_t attrs
  , /* 7 */ vertex ** restrict rv
  , /* 8 */ edge ** restrict re
)
{
  edge * e = 0;
  vertex * nv;

  if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_UP)
  {
    struct vertex_cmp_context ctx = { A : label, len : label_len };
    e = list_search(v->up, &ctx, vertex_compare);
  }
  else if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_DOWN)
  {
    struct vertex_cmp_context ctx = { B : label, len : label_len };
    e = list_search(v->down, &ctx, vertex_compare);
  }

  if(!e)
    return false;

  if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_UP)
    nv = e->A;
  else if((attrs & DIRECTION_OPT) == MORIA_TRAVERSE_DOWN)
    nv = e->B;

  if(edge_visit && !(e->attrs & edge_visit))
    return false;

  if(vertex_visit && !(nv->attrs & vertex_visit))
    return false;

  if(rv)
    *rv = nv;
  if(re)
    *re = e;

  return true;
}

//
// public
//

xapi vertex_create(vertex ** const restrict v, graph * restrict g, size_t vsz, uint32_t attrs)
{
  enter;

  fatal(xmalloc, v, sizeof(**v) + vsz);
  (*v)->graph = g;
  fatal(list_create, &(*v)->up);
  fatal(list_create, &(*v)->down);
  (*v)->attrs = attrs;

  finally : coda;
}

xapi vertex_creates(vertex ** const restrict v, graph * restrict g, size_t vsz, uint32_t attrs, const char * const restrict label)
{
  enter;

  fatal(vertex_create, v, g, vsz, attrs);
  (*v)->label = label;
  (*v)->label_len = strlen(label);

  finally : coda;
}

xapi vertex_createw(vertex ** const restrict v, graph * restrict g, size_t vsz, uint32_t attrs, const char * const restrict label, size_t label_len)
{
  enter;

  fatal(vertex_create, v, g, vsz, attrs);
  (*v)->label = label;
  (*v)->label_len = label_len;

  finally : coda;
}

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

int vertex_compare(void * _ctx, const void * _e, size_t idx)
{
  struct vertex_cmp_context * ctx = _ctx;
  const edge * e = _e;

  if(ctx->A)
    ctx->lc = memncmp(ctx->A, ctx->len, e->A->label, e->A->label_len);
  else
    ctx->lc = memncmp(ctx->B, ctx->len, e->B->label, e->B->label_len);

  ctx->lx = idx;
  return ctx->lc;
};

//
// api
//

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

API vertex * vertex_travel_vertexs(
    const vertex * restrict v
  , const char * restrict label
  , uint32_t vertex_visit
  , uint32_t edge_visit
  , uint32_t attrs
)
{
  vertex * nv;
  if(vertex_travel(v, label, strlen(label), vertex_visit, edge_visit, attrs, &nv, 0))
    return nv;

  return 0;
}

API vertex * vertex_travel_vertexw(
    const vertex * restrict v
  , const char * restrict label
  , size_t label_len
  , uint32_t vertex_visit
  , uint32_t edge_visit
  , uint32_t attrs
)
{
  vertex * nv;
  if(vertex_travel(v, label, label_len, vertex_visit, edge_visit, attrs, &nv, 0))
    return nv;

  return 0;
}

API edge * vertex_travel_edges(
    const vertex * restrict v
  , const char * restrict label
  , uint32_t vertex_visit
  , uint32_t edge_visit
  , uint32_t attrs
)
{
  edge * e;
  if(vertex_travel(v, label, strlen(label), vertex_visit, edge_visit, attrs, 0, &e))
    return e;

  return 0;
}

API edge * vertex_travel_edgew(
    const vertex * restrict v
  , const char * restrict label
  , size_t label_len
  , uint32_t vertex_visit
  , uint32_t edge_visit
  , uint32_t attrs
)
{
  edge * e;
  if(vertex_travel(v, label, label_len, vertex_visit, edge_visit, attrs, 0, &e))
    return e;

  return 0;
}
