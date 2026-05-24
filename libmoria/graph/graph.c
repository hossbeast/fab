/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <inttypes.h>

#include "common/attrs.h"
#include "narrator.h"
#include "valyria/map.h"
#include "xlinux/xstdlib.h"

#include "graph.internal.h"
#include "moria.h"
#include "traverse.internal.h"
#include "membership.h"


//
// static
//

static uint32_t vertex_id_map_get(const map * restrict idmap, moria_vertex * restrict v)
{
  void *r;
  uint32_t id;

  r = map_get(idmap, (void*)&v, sizeof(v));
  id = (uint32_t)(uintptr_t)r;

  /* if this assert fires, it means there are vertices in the graph which were not
   * passed to moria_graph_say in the vertex_lists parameter */
  if(id == 0) {
    printf("unknown vertex %p\n", v);
    RTA;
  }

  return id;
}

struct edge_compare_context {
  moria_graph * g;
  map * vertex_id_map;
};

static int edge_compare(const void * _X, const void * _Y, void * _ctx)
{
  map * vertex_id_map = _ctx;

  const moria_edge * X = *(moria_edge **)_X;
  const moria_edge * Y = *(moria_edge **)_Y;
  uint32_t xid;
  uint32_t yid;
  int r;
  int x;

  if((r = INTCMP(X->attrs, Y->attrs)))
    return r;

  if(!(X->attrs & MORIA_EDGE_HYPER))
  {
    xid = vertex_id_map_get(vertex_id_map, X->A);
    yid = vertex_id_map_get(vertex_id_map, Y->A);
    if((r = INTCMP(xid, yid))) {
      return r;
    }

    xid = vertex_id_map_get(vertex_id_map, X->B);
    yid = vertex_id_map_get(vertex_id_map, Y->B);
    if((r = INTCMP(xid, yid))) {
      return r;
    }

    return 0;
  }

  if((r = INTCMP(X->Alen, Y->Alen)))
    return r;

  for(x = 0; x < X->Alen; x++)
  {
    xid = vertex_id_map_get(vertex_id_map, X->Alist[x].v);
    yid = vertex_id_map_get(vertex_id_map, Y->Alist[x].v);
    if((r = INTCMP(xid, yid))) {
      return r;
    }
  }

  if((r = INTCMP(X->Blen, Y->Blen)))
    return r;

  for(x = 0; x < X->Blen; x++)
  {
    xid = vertex_id_map_get(vertex_id_map, X->Blist[x].v);
    yid = vertex_id_map_get(vertex_id_map, Y->Blist[x].v);
    if((r = INTCMP(xid, yid))) {
      return r;
    }
  }

  return 0;
}

static void edges_sorted(
    llist ** restrict edge_lists
  , uint16_t edge_lists_len
  , map * restrict vertex_id_map
  , const uint32_t * restrict mask_list
  , const uint32_t * restrict bits_list
  , uint16_t mask_list_len
  , moria_edge *** restrict edges
  , size_t * restrict count
)
{
  moria_edge * e;
  int x;
  int y;

  *count = 0;
  for(x = 0; x < edge_lists_len; x++) {
    llist_foreach(edge_lists[x], e, owner) {
      if(!membership_edge(e, mask_list, bits_list, mask_list_len)) {
        continue;
      }
      *count += 1;
    }
  }

  xmalloc(edges, (*count) * sizeof(*edges));

  y = 0;
  for(x = 0; x < edge_lists_len; x++)
  {
    llist_foreach(edge_lists[x], e, owner) {
      if(!membership_edge(e, mask_list, bits_list, mask_list_len)) {
        continue;
      }
      (*edges)[y++] = e;
    }
  }

  qsort_r(*edges, *count, sizeof(**edges), edge_compare, vertex_id_map);
}

static int vertex_compare(const void * _A, const void * _B)
{
  const moria_vertex * A = *(moria_vertex **)_A;
  const moria_vertex * B = *(moria_vertex **)_B;

  int r;
  if((r = INTCMP(B->attrs & ~0x3f, A->attrs & ~0x3f)))
    return r;

  if(A->label == 0 || B->label == 0)
    return INTCMP(A->label, B->label);

  return memncmp(A->label, A->label_len, B->label, B->label_len);
}

static void vertices_sorted(
    llist ** restrict vertex_lists
  , uint16_t vertex_lists_len
  , const uint32_t * restrict mask_list
  , const uint32_t * restrict bits_list
  , uint16_t mask_list_len
  , moria_vertex *** restrict vertices
  , size_t * restrict count
)
{
  moria_vertex * v;
  int x;
  int y;

  *count = 0;
  for(x = 0; x < vertex_lists_len; x++) {
    llist_foreach(vertex_lists[x], v, owner) {
      if(!membership_vertex(v, mask_list, bits_list, mask_list_len)) {
        continue;
      }
      *count += 1;
    }
  }

  xmalloc(vertices, sizeof(*vertices) * (*count));

  y = 0;
  for(x = 0; x < vertex_lists_len; x++)
  {
    llist_foreach(vertex_lists[x], v, owner) {
      if(!membership_vertex(v, mask_list, bits_list, mask_list_len)) {
        continue;
      }
      (*vertices)[y++] = v;
    }
  }

  qsort(*vertices, *count, sizeof(**vertices), vertex_compare);
}

//
// internal
//

void graph_vertex_init(moria_graph * const restrict g, moria_vertex * const restrict v)
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

void graph_edge_init(moria_graph * const restrict g, moria_edge * const restrict e)
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

void moria_graph_init(moria_graph * const restrict g)
{
  memset(g, 0, sizeof(*g));

  g->vertex_mask = UINT64_C(1);
  g->edge_mask = UINT64_C(1);
  llist_init(&g->states);
}

void moria_graph_destroy(moria_graph * const restrict g)
{
  traversal_state *state;
  llist *T;

  llist_foreach_safe(&g->states, state, lln, T) {
    free(state);
  }
}

void moria_graph_say(
    moria_graph * const restrict g
  , llist ** restrict vertex_lists
  , uint16_t vertex_lists_len
  , llist ** restrict edge_lists
  , uint16_t edge_lists_len
  , const attrs32 * vertex_defs
  , const attrs32 * edge_defs
  , const uint32_t * restrict mask_list
  , const uint32_t * restrict bits_list
  , uint16_t mask_list_len
  , narrator * const restrict N
)
{
  map vertex_id_map;
  int x;
  int y;
  uint32_t id;
  moria_vertex ** vertices = 0;
  moria_edge ** edges = 0;
  moria_edge * e;
  moria_vertex * v;
  size_t vertices_count;
  size_t edges_count;
  char label[64];
  size_t label_len;

  map_init(&vertex_id_map);

  vertices_sorted(vertex_lists, vertex_lists_len, mask_list, bits_list, mask_list_len, &vertices, &vertices_count);
  for(x = 0; x < vertices_count; x++)
  {
    v = vertices[x];
    if(!membership_vertex(v, mask_list, bits_list, mask_list_len)) {
      continue;
    }

    map_put(&vertex_id_map, (void*)&v, sizeof(v), (void*)(uintptr_t)(x + 1), 0);
  }

  for(x = 0; x < vertices_count; x++)
  {
    v = vertices[x];
    if(!membership_vertex(v, mask_list, bits_list, mask_list_len)) {
      continue;
    }

    if(x) {
      xsays(" ");
    }

    id = vertex_id_map_get(&vertex_id_map, v);
    xsayf("%"PRIu32"-%.*s", id, v->label_len, v->label);

    if(vertex_defs && (label_len = attrs32_zcat(label, sizeof(label), vertex_defs, v->attrs)))
    {
      xsayf("!%.*s", (int)label_len, label);
    }
    else if(v->attrs)
    {
      xsayf("!0x%"PRIx32, v->attrs);
    }
  }

  edges_sorted(edge_lists, edge_lists_len, &vertex_id_map, mask_list, bits_list, mask_list_len, &edges, &edges_count);
  for(x = 0; x < edges_count; x++)
  {
    xsays(" ");
    e = edges[x];

    if(!membership_edge(e, mask_list, bits_list, mask_list_len)) {
      continue;
    }

    if(e->attrs & MORIA_EDGE_HYPER)
    {
      if(e->Blen == 0)
      {
        xsays("_");
      }
      else
      {
        qsort(e->Blist, e->Blen, sizeof(*e->Blist), vertex_compare);
        for(y = 0; y < e->Blen; y++)
        {
          if(y) {
            xsays(",");
          }
          id = vertex_id_map_get(&vertex_id_map, e->Blist[y].v);
          xsayf("%"PRIu32, id);
        }
      }
    }
    else
    {
      id = vertex_id_map_get(&vertex_id_map, e->B);
      xsayf("%"PRIu32, id);
    }

    if(edge_defs && (label_len = attrs32_zcat(label, sizeof(label), edge_defs, e->attrs)))
    {
      xsayf(":%.*s", (int)label_len, label);
    }
    else if(e->attrs)
    {
      xsayf(":0x%"PRIx32, e->attrs);
    }
    xsays(":");

    if(e->attrs & MORIA_EDGE_HYPER)
    {
      if(e->Alen == 0)
      {
        xsays("_");
      }
      else
      {
        qsort(e->Alist, e->Alen, sizeof(*e->Alist), vertex_compare);
        for(y = 0; y < e->Alen; y++)
        {
          if(y) {
            xsays(",");
          }
          id = vertex_id_map_get(&vertex_id_map, e->Alist[y].v);
          xsayf("%"PRIu32, id);
        }
      }
    }
    else
    {
      id = vertex_id_map_get(&vertex_id_map, e->A);
      xsayf("%"PRIu32, id);
    }
  }

  map_destroy(&vertex_id_map);
  free(edges);
  free(vertices);
}
