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

#include "xapi.h"

#include "common/attrs.h"
#include "common/hash.h"
#include "narrator.h"
#include "valyria/dictionary.h"
#include "valyria/hashtable.h"
#include "valyria/llist.h"
#include "valyria/rbtree.h"
#include "xlinux/xstdlib.h"

#include "graph.internal.h"
#include "MORIA.errtab.h"
#include "edge.internal.h"
#include "moria.h"
#include "traverse.internal.h"
#include "vertex.h"

#include "zbuffer.h"

//
// static
//

struct edge_compare_context {
  moria_graph * g;
  dictionary * vertex_id_map;
};

static int edge_compare(const void * _X, const void * _Y, void * _ctx)
{
  dictionary * vertex_id_map = _ctx;

  const moria_edge * X = *(moria_edge **)_X;
  const moria_edge * Y = *(moria_edge **)_Y;
  uint32_t * xid;
  uint32_t * yid;
  int r;
  int x;

  if((r = INTCMP(X->attrs, Y->attrs)))
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

static xapi edges_sorted(llist ** restrict edge_lists, uint16_t edge_lists_len, dictionary * restrict vertex_id_map, moria_edge *** restrict edges, size_t * restrict count)
{
  enter;

  moria_edge * e;
  int x;
  int y;

  *count = 0;
  for(x = 0; x < edge_lists_len; x++)
  {
    *count += llist_count(edge_lists[x]);
  }

  fatal(xmalloc, edges, (*count) * sizeof(*edges));

  y = 0;
  for(x = 0; x < edge_lists_len; x++)
  {
    llist_foreach(edge_lists[x], e, owner) {
      (*edges)[y++] = e;
    }
  }

  qsort_r(*edges, *count, sizeof(**edges), edge_compare, vertex_id_map);

  finally : coda;
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

static xapi vertices_sorted(llist ** restrict vertex_lists, uint16_t vertex_lists_len, moria_vertex *** restrict vertices, size_t * restrict count)
{
  enter;

  moria_vertex * v;
  int x;
  int y;

  *count = 0;
  for(x = 0; x < vertex_lists_len; x++)
  {
    *count += llist_count(vertex_lists[x]);
  }

  fatal(xmalloc, vertices, sizeof(*vertices) * (*count));

  y = 0;
  for(x = 0; x < vertex_lists_len; x++)
  {
    llist_foreach(vertex_lists[x], v, owner) {
      (*vertices)[y++] = v;
    }
  }

  qsort(*vertices, *count, sizeof(**vertices), vertex_compare);

  finally : coda;
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

void API moria_graph_init(moria_graph * const restrict g)
{
  memset(g, 0, sizeof(*g));

  g->vertex_mask = UINT64_C(1);
  g->edge_mask = UINT64_C(1);
  llist_init_node(&g->states);
}

xapi API moria_graph_create(moria_graph ** const restrict g)
{
  enter;

  fatal(xmalloc, g, sizeof(**g));
  moria_graph_init(*g);

  finally : coda;
}

void API moria_graph_destroy(moria_graph * const restrict g)
{
  traversal_state *state;
  llist *T;

  llist_foreach_safe(&g->states, state, lln, T) {
    wfree(state);
  }
}

xapi API moria_graph_xfree(moria_graph * const restrict g)
{
  enter;

  if(g) {
    moria_graph_destroy(g);
  }
  wfree(g);

  finally : coda;
}

xapi API moria_graph_ixfree(moria_graph ** const restrict g)
{
  enter;

  fatal(moria_graph_xfree, *g);
  *g = 0;

  finally : coda;
}

xapi API moria_graph_say(
    moria_graph * const restrict g
  , llist ** restrict vertex_lists
  , uint16_t vertex_lists_len
  , llist ** restrict edge_lists
  , uint16_t edge_lists_len
  , const attrs32 * vertex_defs
  , const attrs32 * edge_defs
  , struct narrator * const restrict N
)
{
  enter;

  dictionary * vertex_id_map = 0;
  int x;
  int y;
  uint32_t * id;
  moria_vertex ** vertices = 0;
  moria_edge ** edges = 0;
  moria_edge * e;
  moria_vertex * v;
  size_t vertices_count;
  size_t edges_count;
  char label[64];
  size_t label_len;

  fatal(dictionary_create, &vertex_id_map, sizeof(*id));

  fatal(vertices_sorted, vertex_lists, vertex_lists_len, &vertices, &vertices_count);
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

    if(vertex_defs && (label_len = znload_attrs32(label, sizeof(label), vertex_defs, v->attrs)))
    {
      xsayf("!%.*s", (int)label_len, label);
    }
    else if(v->attrs)
    {
      xsayf("!0x%"PRIx32, v->attrs);
    }
  }

  fatal(edges_sorted, edge_lists, edge_lists_len, vertex_id_map, &edges, &edges_count);
  for(x = 0; x < edges_count; x++)
  {
    xsays(" ");
    e = edges[x];
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
          if(y)
            xsays(",");
          id = dictionary_get(vertex_id_map, &e->Alist[y].v, sizeof(e->Alist[y].v));
          xsayf("%"PRIu32, *id);
        }
      }
    }
    else
    {
      id = dictionary_get(vertex_id_map, (void*)&e->A, sizeof(e->A));
      xsayf("%"PRIu32, *id);
    }

    if(edge_defs && (label_len = znload_attrs32(label, sizeof(label), edge_defs, e->attrs)))
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
      if(e->Blen == 0)
      {
        xsays("_");
      }
      else
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

xapi API moria_graph_lookup_sentinel(void * restrict _context, const char ** restrict label, uint16_t * restrict label_len)
{
  enter;

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

  finally : coda;
}

uint32_t API moria_vertex_entry_hash(uint32_t h, const void *p, size_t sz)
{
  const moria_vertex_entry *v = p;

  return hash32(h, v->label, v->label_len);
}

int API moria_vertex_entry_cmp(const void *A, size_t Asz, const void *B, size_t Bsz)
{
  const moria_vertex_entry *a = A;
  const moria_vertex_entry *b = B;

  return memncmp(a->label, a->label_len, b->label, b->label_len);
}

int API moria_vertex_entry_key_cmp(const void *A, const void *K, size_t sz)
{
  const moria_vertex_entry *a = (void*)A;
  const moria_vertex_entry *b = K;

  return memncmp(a->label, a->label_len, b->label, b->label_len);
}

xapi API moria_graph_lookup(
    moria_graph * restrict g
  , const hashtable * restrict mm
  , moria_graph_lookup_identifier_callback identifier_callback
  , bool (*candidate_callback)(void * context, const struct moria_vertex * const restrict v)
  , void * restrict context
  , moria_vertex * restrict V[2]   // (returns) located vertices
  , int * restrict r               // (returns) return code
)
{
  enter;

  const char * label;
  uint16_t label_len;
  moria_vertex_entry *entry, key;
  moria_vertex *v, *p;
  bool matched;

  *r = 0;

  // get the starting vertex label
  fatal(identifier_callback, context, 0, 0);
  fatal(identifier_callback, context, &label, &label_len);
  if(!label) {
    goto XAPI_FINALIZE;
  }

  // get the set of starting vertices having the initial label
  key.label = label;
  key.label_len = label_len;
  if(!(entry = hashtable_search(mm, &key, sizeof(key), moria_vertex_entry_hash, moria_vertex_entry_key_cmp))) {
    goto XAPI_FINALIZE;
  }

  rbtree_foreach(&entry->rbt, v, rbn_lookup) {
    if(candidate_callback && !candidate_callback(context, v)) {
      continue;
    }

    /* check whether the sequence of ancestors matches */
    p = v;
    while(1)
    {
      fatal(identifier_callback, context, &label, &label_len);
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
    fatal(identifier_callback, context, 0, 0);
    fatal(identifier_callback, context, &label, &label_len);

    if(!matched) {
      continue;
    }

    V[*r] = v;

    /* identifier matched on multiple nodes */
    if(((*r)++) == 2) {
      break;
    }
  }

  finally : coda;
}

xapi API moria_graph_linear_search(llist * restrict vertex_list, const char * restrict label, uint16_t label_len, moria_vertex ** restrict rv)
{
  enter;

  moria_vertex *v;

  *rv = 0;
  llist_foreach(vertex_list, v, owner) {
    if(memncmp(v->label, v->label_len, label, label_len) == 0) {
      if(*rv) {
        fail(MORIA_AMBIGUOUS);
      }
      *rv = v;
    }
  }

  finally : coda;
}
