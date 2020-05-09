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

#include "attrs.h"
#include "macros.h"
#include "zbuffer.h"

//
// static
//

struct edge_compare_context {
  graph * g;
  dictionary * vertex_id_map;
};

static int edge_compare(const void * _X, const void * _Y, void * _ctx)
{
  dictionary * vertex_id_map = _ctx;

  const edge_t * X = *(edge_t **)_X;
  const edge_t * Y = *(edge_t **)_Y;
  uint32_t * xid;
  uint32_t * yid;
  int r;
  int x;

  if((r = INTCMP(X->attrs & ~MORIA_EDGE_IDENTITY, Y->attrs & ~MORIA_EDGE_IDENTITY)))
    return r;

  if(!(X->attrs & MORIA_EDGE_HYPER))
  {
    xid = dictionary_get(vertex_id_map, (void*)&X->A, sizeof(void*));
    yid = dictionary_get(vertex_id_map, (void*)&Y->A, sizeof(void*));
    if((r = INTCMP(*xid, *yid)))
      return r;

    xid = dictionary_get(vertex_id_map, (void*)&X->B, sizeof(void*));
    yid = dictionary_get(vertex_id_map, (void*)&Y->B, sizeof(void*));
    if((r = INTCMP(*xid, *yid)))
      return r;

    return 0;
  }

  if((r = INTCMP(X->Alen, Y->Alen)))
    return r;

  for(x = 0; x < X->Alen; x++)
  {
    xid = dictionary_get(vertex_id_map, (void*)&X->Alist[x].v, sizeof(void*));
    yid = dictionary_get(vertex_id_map, (void*)&Y->Alist[x].v, sizeof(void*));
    if((r = INTCMP(*xid, *yid)))
      return r;
  }

  if((r = INTCMP(X->Blen, Y->Blen)))
    return r;

  for(x = 0; x < X->Blen; x++)
  {
    xid = dictionary_get(vertex_id_map, (void*)&X->Blist[x].v, sizeof(void*));
    yid = dictionary_get(vertex_id_map, (void*)&Y->Blist[x].v, sizeof(void*));
    if((r = INTCMP(*xid, *yid)))
      return r;
  }

  return 0;
}

static xapi edges_sorted(graph * const restrict g, dictionary * restrict vertex_id_map, size_t count, edge_t *** restrict edges)
{
  enter;

  edge_t * e;
  int x;

  fatal(xmalloc, edges, count * sizeof(*edges));

  x = 0;
  llist_foreach(&g->edges, e, graph_lln) {
    (*edges)[x++] = e;
  }

  qsort_r(*edges, count, sizeof(**edges), edge_compare, vertex_id_map);

  finally : coda;
}

static int vertex_compare(const void * _A, const void * _B)
{
  const vertex_t * A = *(vertex_t **)_A;
  const vertex_t * B = *(vertex_t **)_B;

  int r;
  if((r = INTCMP(B->attrs & ~0x3f, A->attrs & ~0x3f)))
    return r;

  if(A->label == 0 || B->label == 0)
    return INTCMP(A->label, B->label);

  return memncmp(A->label, A->label_len, B->label, B->label_len);
}

static xapi vertices_sorted(graph * const restrict g, size_t count, vertex_t *** restrict vertices)
{
  enter;

  vertex_t * v;
  int x;

  fatal(xmalloc, vertices, count * sizeof(*vertices));

  x = 0;
  llist_foreach(&g->vertices, v, graph_lln) {
    (*vertices)[x++] = v;
  }

  qsort(*vertices, count, sizeof(**vertices), vertex_compare);

  finally : coda;
}

//
// internal
//

void graph_vertex_init(graph * const restrict g, vertex_t * const restrict v)
{
  v->ent.index = g->vertex_index;
  v->ent.mask = g->vertex_mask;

  g->vertex_mask <<= 3;
  if(g->vertex_mask == (UINT64_C(1) << 63))
  {
    g->vertex_mask = UINT64_C(1);
    g->vertex_index++;
  }
}

void graph_edge_init(graph * const restrict g, edge_t * const restrict e)
{
  e->ent.index = g->edge_index;
  e->ent.mask = g->edge_mask;

  g->edge_mask <<= 3;
  if(g->edge_mask == (UINT64_C(1) << 63))
  {
    g->edge_mask = UINT64_C(1);
    g->edge_index++;
  }
}

//
// api
//

API xapi graph_create(graph ** const restrict g, uint32_t identity)
{
  xproxy(graph_createx, g, identity, 0, 0, 0, 0);
}

API xapi graph_createx(
    graph ** const restrict g
  , uint32_t identity
  , size_t vsz
  , size_t esz
  , void * vertex_value_destroy
  , void * vertex_value_xdestroy
)
{
  enter;

  fatal(xmalloc, g, sizeof(**g));
  (*g)->identity = identity;
  (*g)->vsz = vsz;
  (*g)->esz = esz;
  (*g)->vertex_value_destroy = vertex_value_destroy;
  (*g)->vertex_value_xdestroy = vertex_value_xdestroy;

  (*g)->vertex_mask = UINT64_C(1);
  (*g)->edge_mask = UINT64_C(1);
  llist_init_node(&(*g)->states);

  fatal(multimap_create, &(*g)->mm);

  llist_init_node(&(*g)->vertices);
  llist_init_node(&(*g)->vertex_freelist);
  llist_init_node(&(*g)->edges);
  llist_init_node(&(*g)->edge_freelist);

  finally : coda;
}

API xapi graph_xfree(graph * const restrict g)
{
  enter;

  llist *T;
  traversal_state * state;
  vertex_t *v;
  edge_t *e;

  if(g)
  {
    fatal(graph_recycle, g);
    fatal(multimap_xfree, g->mm);

    llist_foreach_safe(&g->vertices, v, graph_lln, T) {
      wfree(v);
    }

    llist_foreach_safe(&g->vertex_freelist, v, graph_lln, T) {
      wfree(v);
    }

    llist_foreach_safe(&g->edges, e, graph_lln, T) {
      edge_free(e);
    }

    llist_foreach_safe(&g->edge_freelist, e, graph_lln, T) {
      edge_free(e);
    }

    llist_foreach_safe(&g->states, state, lln, T) {
      wfree(state);
    }
  }

  wfree(g);

  finally : coda;
}

API xapi graph_recycle(graph * const restrict g)
{
  enter;

  vertex_t * v;

  // free all the values
  if(g->vertex_value_destroy || g->vertex_value_xdestroy)
  {
    llist_foreach(&g->vertices, v, graph_lln) {
      if(g->vertex_value_destroy)
        g->vertex_value_destroy(v->value);
      else
        fatal(g->vertex_value_xdestroy, v->value);
    }
  }

  // move to the freelist
  llist_splice_tail(&g->vertex_freelist, &g->vertices);
  llist_splice_tail(&g->edge_freelist, &g->edges);

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

API struct llist * graph_vertices(graph * restrict g)
{
  return &g->vertices;
}

API struct llist * graph_edges(graph * restrict g)
{
  return &g->edges;
}

API void graph_vertex_definitions_set(graph * restrict g, const struct attrs32 * restrict defs)
{
  g->vertex_defs = defs;
}

API void graph_edge_definitions_set(graph * restrict g, const struct attrs32 * restrict defs)
{
  g->edge_defs = defs;
}

API xapi graph_say(graph * const restrict g, struct narrator * const restrict N)
{
  enter;

  dictionary * vertex_id_map = 0;
  int x;
  int y;
  uint32_t * id;
  vertex_t ** vertices = 0;
  edge_t ** edges = 0;
  edge_t * e;
  vertex_t * v;
  size_t vertices_count;
  size_t edges_count;
  uint32_t masked_attrs;
  char label[64];
  size_t label_len;

  vertices_count = llist_count(&g->vertices);
  edges_count = llist_count(&g->edges);

  fatal(dictionary_create, &vertex_id_map, sizeof(*id));

  fatal(vertices_sorted, g, vertices_count, &vertices);
  for(x = 0; x < vertices_count; x++)
  {
    v = vertices[x];
    fatal(dictionary_store, vertex_id_map, (void*)&v, sizeof(v), &id);
    *id = x + 1;
  }

  for(x = 0; x < vertices_count; x++)
  {
    if(x)
      xsays(" ");

    v = vertices[x];
    id = dictionary_get(vertex_id_map, (void*)&v, sizeof(v));

    xsayf("%"PRIu32"-%.*s", *id, v->label_len, v->label);

    if(g->vertex_defs && (label_len = znload_attrs32(label, sizeof(label), g->vertex_defs, v->attrs)))
    {
      xsayf("!%.*s", (int)label_len, label);
    }
    else if(v->attrs)
    {
      xsayf("!0x%"PRIx32, v->attrs);
    }
  }

  fatal(edges_sorted, g, vertex_id_map, edges_count, &edges);
  for(x = 0; x < llist_count(&g->edges); x++)
  {
    xsays(" ");
    e = edges[x];
    if(e->attrs & MORIA_EDGE_HYPER)
    {
      qsort(e->Alist, e->Alen, sizeof(*e->Alist), vertex_compare);

      for(y = 0; y < e->Alen; y++)
      {
        if(y)
          xsays(",");
        id = dictionary_get(vertex_id_map, &e->Alist[y].v, sizeof(e->Alist[y].v));
        xsayf("%"PRIu32, *id);
      }
    }
    else
    {
      id = dictionary_get(vertex_id_map, (void*)&e->A, sizeof(e->A));
      xsayf("%"PRIu32, *id);
    }

    /* mask off internal bits */
    masked_attrs = e->attrs & ~(MORIA_EDGE_IDENTITY | MORIA_EDGE_HYPER);

    if(g->edge_defs && (label_len = znload_attrs32(label, sizeof(label), g->edge_defs, masked_attrs)))
    {
      xsayf(":%.*s", (int)label_len, label);
    }
    else if(masked_attrs)
    {
      xsayf(":0x%"PRIx32, masked_attrs);
    }

    xsays(":");

    if(e->attrs & MORIA_EDGE_HYPER)
    {
      qsort(e->Blist, e->Blen, sizeof(*e->Blist), vertex_compare);

      for(y = 0; y < e->Blen; y++)
      {
        if(y)
          xsays(",");
        id = dictionary_get(vertex_id_map, &e->Blist[y].v, sizeof(e->Blist[y].v));
        xsayf("%"PRIu32, *id);
      }
    }
    else
    {
      id = dictionary_get(vertex_id_map, (void*)&e->B, sizeof(e->B));
      xsayf("%"PRIu32, *id);
    }
  }

finally:
  fatal(dictionary_xfree, vertex_id_map);
  wfree(edges);
  wfree(vertices);
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
  , bool (*candidate_callback)(void * context, const struct vertex * const restrict v)
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

    if(candidate_callback && !candidate_callback(context, v))
      continue;

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

    bool matched = !label;

    // reset the cursor, discard the initial label
    fatal(identifier_callback, context, 0, 0);
    fatal(identifier_callback, context, &label, &label_len);

    if(!matched)
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

API xapi graph_identity_deindex(graph * const restrict g, vertex * const restrict v)
{
  enter;

  fatal(multimap_delete, g->mm, v->label, v->label_len);

  finally : coda;
}
