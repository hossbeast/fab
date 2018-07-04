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

#include <inttypes.h>
#include <stdio.h>

#include "types.h"
#include "xapi.h"

#include "xlinux/xstdlib.h"
#include "valyria/dictionary.h"
#include "valyria/list.h"
#include "valyria/map.h"
#include "valyria/multimap.h"
#include "narrator.h"

#include "internal.h"
#include "graph.internal.h"
#include "MORIA.errtab.h"
#include "edge.internal.h"
#include "vertex.internal.h"

#include "macros.h"

//
// static
//

struct edge_compare_context {
  graph * g;
  dictionary * vertex_id_map;
};

static int edge_compare(const void * _X, const void * _Y, void * _ctx)
{
  struct edge_compare_context * ctx = _ctx;

  const edge * X = list_get(ctx->g->edges, *(int*)_X);
  const edge * Y = list_get(ctx->g->edges, *(int*)_Y);

  uint32_t * xaid = dictionary_get(ctx->vertex_id_map, (void*)&X->A, sizeof(void*));
  uint32_t * xbid = dictionary_get(ctx->vertex_id_map, (void*)&X->B, sizeof(void*));
  uint32_t * yaid = dictionary_get(ctx->vertex_id_map, (void*)&Y->A, sizeof(void*));
  uint32_t * ybid = dictionary_get(ctx->vertex_id_map, (void*)&Y->B, sizeof(void*));

  return    INTCMP(*xaid, *yaid)
         ?: INTCMP(*xbid, *ybid)
         ?: INTCMP(X->A->attrs, Y->A->attrs)
         ?: INTCMP(X->B->attrs, Y->B->attrs)
         ?: INTCMP(X->attrs, Y->attrs)
         ;
}

static xapi edges_sorted(graph * const restrict g, dictionary * restrict vertex_id_map, int ** restrict xs)
{
  enter;

  fatal(xmalloc, xs, sizeof(**xs) * g->edges->l);

  int x;
  for(x = 0; x < g->edges->l; x++)
    (*xs)[x] = x;

  struct edge_compare_context ctx;
  ctx.g = g;
  ctx.vertex_id_map = vertex_id_map;

  qsort_r(*xs, g->edges->l, sizeof(**xs), edge_compare, &ctx);

  finally : coda;
}

static int vertex_compare(const void * _X, const void * _Y, void * ctx)
{
  graph * g = ctx;
  const vertex * X = list_get(g->vertices, *(int*)_X);
  const vertex * Y = list_get(g->vertices, *(int*)_Y);

  return   memncmp(X->label, X->label_len, Y->label, Y->label_len)
        ?: X->attrs - Y->attrs
        ;
}

static xapi vertices_sorted(graph * const restrict g, int ** restrict xs)
{
  enter;

  fatal(xmalloc, xs, sizeof(**xs) * g->vertices->l);

  int x;
  for(x = 0; x < g->vertices->l; x++)
    (*xs)[x] = x;

  qsort_r(*xs, g->vertices->l, sizeof(**xs), vertex_compare, g);

  finally : coda;
}

//
// public
//

xapi graph_vertex_push(graph * const restrict g, vertex * const restrict v)
{
  enter;

  fatal(list_push, g->vertices, v);

  finally : coda;
}

//
// api
//

API xapi graph_create(graph ** const restrict g, uint32_t identity)
{
  xproxy(graph_createx, g, identity, 0, 0, 0);
}

API xapi graph_createx(
    graph ** const restrict g
  , uint32_t identity
  , size_t vsz
  , void * vertex_value_destroy
  , void * vertex_value_xdestroy
)
{
  enter;

  fatal(xmalloc, g, sizeof(**g));
  (*g)->traversal_id = 1;
  fatal(list_createx, &(*g)->vertices, 0, vertex_xfree, 0);
  fatal(list_createx, &(*g)->edges, wfree, 0, 0);
  (*g)->identity = identity;
  (*g)->vsz = vsz;
  (*g)->vertex_value_destroy = vertex_value_destroy;
  (*g)->vertex_value_xdestroy = vertex_value_xdestroy;

  if((*g)->identity)
    fatal(multimap_create, &(*g)->mm);

  finally : coda;
}

API xapi graph_xfree(graph * const restrict g)
{
  enter;

  if(g)
  {
    fatal(graph_recycle, g);
    fatal(list_xfree, g->vertices);
    fatal(list_xfree, g->edges);
    fatal(multimap_xfree, g->mm);
  }

  wfree(g);

  finally : coda;
}

API xapi graph_recycle(graph * const restrict g)
{
  enter;

  if(g->vertex_value_destroy || g->vertex_value_xdestroy)
  {
    int x;
    for(x = 0; x < g->vertices->l; x++)
    {
      vertex * v = list_get(g->vertices, x);
      if(g->vertex_value_destroy)
        g->vertex_value_destroy(v->value);
      else
        fatal(g->vertex_value_xdestroy, v->value);
    }
  }

  fatal(list_recycle, g->vertices);
  fatal(list_recycle, g->edges);
  if(g->identity)
    fatal(multimap_recycle, g->mm);

  finally : coda;
}

API xapi graph_ixfree(graph ** const restrict g)
{
  enter;

  fatal(graph_xfree, *g);
  *g = 0;

  finally : coda;
}

API const list * graph_vertices(graph * g)
{
  return g->vertices;
}

API const list * graph_edges(graph * g)
{
  return g->edges;
}

API xapi graph_say(graph * const restrict g, map * restrict definitions, struct narrator * const restrict N)
{
  enter;

  dictionary * vertex_id_map = 0;
  int * xs = 0;
  int x;

  uint32_t * id;
  fatal(dictionary_create, &vertex_id_map, sizeof(*id));

  fatal(vertices_sorted, g, &xs);
  for(x = 0; x < g->vertices->l; x++)
  {
    vertex * v = list_get(g->vertices, xs[x]);
    fatal(dictionary_set, vertex_id_map, (void*)&v, sizeof(v), &id);
    *id = x + 1;
  }

  for(x = 0; x < g->vertices->l; x++)
  {
    if(x)
      xsays(" ");

    vertex * v = list_get(g->vertices, xs[x]);
    id = dictionary_get(vertex_id_map, (void*)&v, sizeof(v));

    xsayf("%"PRIu32"-%.*s", *id, v->label_len, v->label);

    char * attrs_label = 0;
    if(definitions)
      attrs_label = map_get(definitions, MM(v->attrs));
    if(attrs_label)
    {
      xsayf("!%s", attrs_label);
    }
    else if(v->attrs)
    {
      xsayf("!0x%"PRIx32, v->attrs);
    }
  }

  wfree(xs); xs = 0;
  fatal(edges_sorted, g, vertex_id_map, &xs);
  for(x = 0; x < g->edges->l; x++)
  {
    edge * e = list_get(g->edges, xs[x]);

    id = dictionary_get(vertex_id_map, (void*)&e->A, sizeof(e->A));
    xsayf(" %"PRIu32, *id);

    char * attrs_label = 0;
    if(definitions)
      attrs_label = map_get(definitions, MM(e->attrs));
    if(attrs_label)
    {
      xsayf(":%s", attrs_label);
    }
    else if(e->attrs)
    {
      xsayf(":0x%"PRIx32, e->attrs);
    }

    id = dictionary_get(vertex_id_map, (void*)&e->B, sizeof(e->B));
    xsays(":");
    xsayf("%"PRIu32, *id);
  }

finally:
  wfree(xs);
  fatal(dictionary_xfree, vertex_id_map);
coda;
}

API xapi graph_lookup_sentinel(void * restrict _context, const char ** restrict label, uint16_t * restrict label_len)
{
  enter;

  graph_lookup_sentinel_context * context = _context;

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

  finally : coda;
}

API xapi graph_lookup(
    graph * restrict g
  , graph_lookup_identifier_callback identifier_callback
  , void * restrict context
  , void * mm_tmp
  , vertex * restrict V[2]   // (returns) located vertices
  , int * restrict r         // (returns) return code
)
{
  enter;

  vertex ** nodes;
  size_t nodesl;
  const char * label;
  uint16_t label_len;

  *r = 0;

  // get the starting vertex label
  fatal(identifier_callback, context, 0, 0);
  fatal(identifier_callback, context, &label, &label_len);
  if(!label)
    goto XAPI_FINALIZE;

  // get the set of starting vertices having the initial label
  fatal(multimap_get, g->mm, label, label_len, mm_tmp, &nodes, &nodesl);

  size_t x[2] = { [0] = SIZE_MAX };
  for(x[1] = 0; x[1] < nodesl; x[1]++)
  {
    const vertex * v = nodes[x[1]];

    while(1)
    {
      fatal(identifier_callback, context, &label, &label_len);
      if(!label)
        break;

      if((v = vertex_up(v)) == NULL)
        break;

      if(memncmp(label, label_len, v->label, v->label_len) != 0)
        break;
    }

    bool matched = !!label;

    // reset the cursor, discard the initial label
    fatal(identifier_callback, context, 0, 0);
    fatal(identifier_callback, context, &label, &label_len);

    if(matched)
    {
      // identifier was not fully matched
      continue;
    }
    else if(x[0] != SIZE_MAX)
    {
      // identifier matched on multiple nodes
      V[0] = nodes[x[0]];
      V[1] = nodes[x[1]];
      *r = 2;
      goto XAPI_FINALIZE;
    }

    x[0] = x[1];
  }

  if(x[0] != SIZE_MAX)
  {
    V[0] = nodes[x[0]];
    *r = 1;
  }

  finally : coda;
}

API xapi graph_identity_indexs(graph * const restrict g, vertex * const restrict v, const char * const restrict name)
{
  enter;

  fatal(multimap_set, g->mm, MMS(name), v);

  finally : coda;
}

API xapi graph_identity_indexw(graph * const restrict g, vertex * const restrict v, const char * const restrict name, uint16_t name_len)
{
  enter;

  fatal(multimap_set, g->mm, name, name_len, v);

  finally : coda;
}
