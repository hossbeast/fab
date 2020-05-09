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

#include <stdlib.h>

#include "types.h"
#include "xapi.h"

#include "xlinux/xfcntl.h"
#include "xlinux/xmman.h"
#include "xlinux/xstat.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "xlinux/xunistd.h"
#include "moria/graph.h"
#include "moria/operations.h"
#include "moria/vertex.h"
#include "moria/edge.h"
#include "moria/operations.h"
#include "valyria/list.h"
#include "valyria/llist.h"
#include "valyria/set.h"
#include "valyria/map.h"
#include "value.h"
#include "narrator.h"

#include "node_operations.h"
#include "logging.h"
#include "node.internal.h"
#include "shadow.h"
#include "filesystem.h"
#include "path.h"
#include "graph.h"
#include "module.h"
#include "rule.h"
#include "params.h"
#include "stats.h"

#include "macros.h"
#include "attrs.h"
#include "hash.h"

uint32_t node_invalidation_counter;

//
// static
//

xapi node_index(node * n)
{
  enter;

  vertex * v = vertex_containerof(n);

  // index the node for lookup
  fatal(graph_identity_indexw, g_graph, v, n->name->name, n->name->namel);

  finally : coda;
}

/*
 * visit rules attached to a directory, queue up their rma for re-evaluation
 */
static xapi children_changed_visitor(edge * e, void * ctx, traversal_mode mode, int distance, int * result)
{
  enter;

  rule_dirnode_edge *rde;
  rule_module_association *rma;

  RUNTIME_ASSERT(e->attrs == EDGE_TYPE_RULE_DIR);
  rde = edge_value(e);
  rma = rde->rma;

  graph_rma_enqueue(rma);

  finally : coda;
}

/*
 * n - directory node
 */
static xapi dirnode_children_changed(node * restrict n, struct graph_invalidation_context * restrict invalidation)
{
  enter;

  // traverse to rules attached to this directory node
  fatal(graph_traverse_vertex_edges
    , g_graph
    , vertex_containerof(n)
    , children_changed_visitor
    , invalidation->edge_traversal
    , (traversal_criteria[]) {{
          edge_travel : EDGE_TYPE_RULE_DIR
        , edge_visit : EDGE_TYPE_RULE_DIR
      }}
    , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_POST
    , 0
  );

  finally : coda;
}

//
// public
//

xapi node_operations_create_vertex(
    vertex ** const restrict rv
  , graph * const restrict g
  , uint32_t attrs
  , const char * const restrict label
  , uint16_t label_len
)
{
  enter;

  vertex * v;
  node * n;
  path * p = 0;

  RUNTIME_ASSERT(label_len != 0);

  STATS_INC(nodes);
  fatal(vertex_create, &v, g, attrs | VERTEX_TYPE_NODE);
  n = vertex_value(v);
  n->self_bpe.typemark = BPE_NODE;

  fatal(path_createw, &p, label, label_len);
  n->name = p;
  p = 0;

  // the vertex label points at the node name
  v->label = n->name->name;
  v->label_len = n->name->namel;

  if(log_would(L_NODE))
  {
    narrator *N;
    fatal(log_start, L_NODE, &N);
    xsays("create ");
    fatal(node_project_relative_path_say, n, N);
    fatal(log_finish);
  }

  n->notify_epoch = -1;
  n->wd = -1;

  n->not_loaded = 1;
  n->not_parsed = 1;

  fatal(node_index, n);
  llist_init_node(&n->pending.lln);
  n->pending.kind = SWEEPER_EVENT_SELF;
  rbtree_init(&n->pending_child);

  *rv = v;

finally:
  fatal(path_xfree, &p);
coda;
}

xapi node_vertex_create(
    node ** const restrict n
  , uint32_t attrs
  , const char * restrict label
  , uint16_t label_len
)
{
  enter;

  vertex * v = 0;
  fatal(node_operations_create_vertex, &v, g_graph, attrs, label, label_len);

  *n = vertex_value(v);

  finally : coda;
}

xapi node_connect(node * restrict A, node * restrict B, edge_type relation, graph_invalidation_context * restrict invalidation, node_edge ** restrict nep, node ** restrict oldp)
{
  enter;

  bool created = false;
  edge * e = 0;
  node_edge *ne;
  narrator * N;
  uint64_t cat = 0;
  vertex_shadowtype ash;
  vertex_shadowtype bsh;
  vertex *old_vertex = 0, **oldvp = 0;

  if(oldp) {
    oldvp = &old_vertex;
    *oldp = 0;
  }

  fatal(graph_connect_replace
    , g_graph
    , vertex_containerof(A)
    , vertex_containerof(B)
    , relation
    , &e
    , &created
    , oldvp
  );

  ne = edge_value(e);
  if(nep)
    *nep = ne;

  if(!created)
    goto XAPI_FINALLY;

  if(oldp && old_vertex) {
    *oldp = vertex_value(old_vertex);
  }

  /* invalidate the consumer nodes */
  if(relation & EDGE_INVALIDATIONS)
  {
    fatal(node_invalidate, A, invalidation);
    cat = L_DEPGRAPH;
  }

  if(relation & EDGE_INVALIDATIONS)
  {
    llist_init_node(&ne->lln);
    ne->bpe.typemark = BPE_NODE_EDGE;
  }
  else if(relation == EDGE_TYPE_FS)
  {
    node_filetype_set(A, VERTEX_FILETYPE_DIR);
    if(!node_filetype_get(B))
      node_filetype_set(B, VERTEX_FILETYPE_REG);

    ash = node_shadowtype_get(A);
    bsh = node_shadowtype_get(B);

    /* propagate the shadow-fs property */
    if(ash && !bsh)
    {
      node_shadowtype_set(B, VERTEX_SHADOWTYPE_FS);
    }
    if(ash)
    {
      B->mod = A->mod;
    }

    fatal(dirnode_children_changed, A, invalidation);
    cat = L_FSGRAPH;
  }

  if(log_would(cat))
  {
    fatal(log_start, cat, &N);
    xsayf("%8s ", "connect");
    fatal(graph_edge_say, e, N);
    fatal(log_finish);
  }

  finally : coda;
}

xapi node_hyperconnect(
    struct node ** restrict Axlist, uint16_t Alen
  , struct node ** restrict Bxlist
  , uint16_t Blen
  , edge_type relation
  , graph_invalidation_context * restrict invalidation
  , struct node_edge ** restrict nep
)
{
  enter;

  vertex ** Alist = 0;
  vertex ** Blist = 0;
  int x;
  bool created;
  edge *e;
  node_edge *ne;
  narrator * N;
  uint64_t cat = 0;

  fatal(xmalloc, &Alist, sizeof(*Alist) * Alen);
  for(x = 0; x < Alen; x++)
    Alist[x] = vertex_containerof(Axlist[x]);

  fatal(xmalloc, &Blist, sizeof(*Blist) * Blen);
  for(x = 0; x < Blen; x++)
    Blist[x] = vertex_containerof(Bxlist[x]);

  created = false;
  e = 0;
  fatal(graph_hyperconnect
    , g_graph
    , Alist
    , Alen
    , Blist
    , Blen
    , relation
    , &e
    , &created
  );

  ne = edge_value(e);
  if(nep)
    *nep = ne;

  if(!created)
    goto XAPI_FINALLY;

  llist_init_node(&ne->lln);
  ne->bpe.typemark = BPE_NODE_EDGE;

  /* when the edge is created, invalidate the consumer nodes */
  if(relation == EDGE_TYPE_STRONG || relation == EDGE_TYPE_CONDUIT)
  {
    for(x = 0; x < Alen; x++)
    {
      fatal(node_invalidate, Axlist[x], invalidation);
    }

    cat = L_DEPGRAPH;
  }

  if(log_would(cat))
  {
    fatal(log_start, cat, &N);
    xsayf("%8s ", "connect");
    fatal(graph_edge_say, e, N);
    fatal(log_finish);
  }

finally:
  wfree(Alist);
  wfree(Blist);
coda;
}

xapi node_disconnect(node * restrict A, node * restrict B, struct graph_invalidation_context * restrict invalidation)
{
  enter;

  vertex *va;
  vertex *vb;
  edge *e;

  va = vertex_containerof(A);
  vb = vertex_containerof(B);
  if(!(e = edge_between(va, vb)))
    goto XAPI_FINALLY;

  fatal(node_edge_disconnect, edge_value(e), invalidation);

  finally : coda;
}

static xapi __attribute__((unused)) log_edges(node * restrict n)
{
  enter;

  narrator *N;
  vertex *v = vertex_containerof(n);
  edge *e;
  node *c;
  int x;

  fatal(log_start, L_INFO, &N);
  fatal(node_project_relative_path_say, n, N);
  fatal(log_finish);

  logf(L_INFO, " up %zu", rbtree_count(&v->up) + (v->up_identity ? 1 : 0));
  rbtree_foreach(&v->up, e, rbn_up) {
    fatal(log_start, L_INFO, &N);
    xsays("  ");
    xsays(attrs32_name_byvalue(graph_edge_attrs, EDGE_TYPE_OPT & e->attrs));
    xsays(" ");
    if(!(e->attrs & MORIA_EDGE_HYPER))
    {
      if(e->A->attrs == VERTEX_RULE)
      {
        xsayf("rule %p", e->A);
      }
      else
      {
        c = vertex_value(e->A);
        fatal(node_project_relative_path_say, c, N);
      }
    }
    else
    {
      for(x = 0; x < e->Alen; x++)
      {
        c = vertex_value(e->Alist[x].v);
        if(x)
          fatal(narrator_xsays, N, ", ");
        fatal(node_project_relative_path_say, c, N);
      }
    }
    fatal(log_finish);
  }

  logf(L_INFO, " down %zu", rbtree_count(&v->down));
  rbtree_foreach(&v->down, e, rbn_down) {
    fatal(log_start, L_INFO, &N);
    xsays("  ");
    xsays(attrs32_name_byvalue(graph_edge_attrs, EDGE_TYPE_OPT & e->attrs));
    xsays(" ");
    if(!(e->attrs & MORIA_EDGE_HYPER))
    {
      if(e->B->attrs == VERTEX_RULE)
      {
        xsayf("rule %p", e->B);
      }
      else
      {
        c = vertex_value(e->B);
        fatal(node_project_relative_path_say, c, N);
      }
    }
    else
    {
      for(x = 0; x < e->Blen; x++)
      {
        c = vertex_value(e->Blist[x].v);
        if(x)
          fatal(narrator_xsays, N, ", ");
        fatal(node_project_relative_path_say, c, N);
      }
    }
    fatal(log_finish);
  }

  finally : coda;
}

xapi node_delete_node(node *n)
{
  enter;

  vertex *v = vertex_containerof(n);

  if(log_would(L_NODE))
  {
    narrator * N;
    fatal(log_start, L_NODE, &N);
    xsayf("delete %p %s", n, v->label);
    fatal(log_finish);
  }

  fatal(graph_delete_vertex, v);

  finally : coda;
}

static xapi disconnect_edge(node_edge * restrict ne, graph_invalidation_context * restrict invalidation)
{
  enter;

  narrator * N;
  uint64_t cat = 0;
  edge *e;

  e = edge_containerof(ne);
  if((e->attrs & EDGE_TYPE_OPT) == EDGE_TYPE_FS)
  {
    cat = L_FSGRAPH;
  }
  else if((e->attrs & EDGE_TYPE_OPT) & EDGE_INVALIDATIONS)
  {
    cat = L_DEPGRAPH;
  }
  else if((e->attrs & EDGE_TYPE_OPT) == EDGE_TYPE_RULE_DIR)
  {
    cat = L_RULEGRAPH;
  }

  if(log_would(cat))
  {
    fatal(log_start, cat, &N);
    xsayf("%p disconnect ", e);
    fatal(graph_edge_say, e, N);
    fatal(log_finish);
  }

  fatal(graph_edge_disconnect, g_graph, e);

  finally : coda;
}

static xapi fsedge_disconnecting(node * restrict n, graph_invalidation_context * restrict invalidation)
{
  enter;

  vertex *v;
  edge *upe;
  llist head;
  rbnode *rbn;

  v = vertex_containerof(n);

  /* when a directory is deleted, also disconnect any rules attached to the directory */
  llist_init_node(&head);
  rbtree_foreach(&v->up, upe, rbn_up) {
    if((upe->attrs & EDGE_TYPE_OPT) == EDGE_TYPE_RULE_DIR) {
      llist_append(&head, &upe->rbn_up, lln);
    } else {
      goto XAPI_FINALLY;
    }
  }

  llist_foreach(&head, rbn, lln) {
    upe = containerof(rbn, typeof(*upe), rbn_up);
    fatal(disconnect_edge, edge_value(upe), invalidation);
  }

  finally : coda;
}

static xapi strong_disconnecting(node_edge * restrict ne, edge * restrict e, graph_invalidation_context * restrict invalidation)
{
  enter;

/*

1. unset the bpe for connected nodes
2. invalidate consumer nodes

*/

  int x;
  node *n;

  if(ne->dir == EDGE_TGT_SRC)
  {
    if(!(e->attrs & MORIA_EDGE_HYPER))
    {
      n = vertex_value(e->A);
      if(n->bpe == &ne->bpe)
        n->bpe = 0;

      fatal(node_invalidate, n, invalidation);
    }
    else
    {
      for(x = 0; x < e->Alen; x++)
      {
        n = vertex_value(e->Alist[x].v);
        if(n->bpe == &ne->bpe)
          n->bpe = 0;

        fatal(node_invalidate, n, invalidation);
      }
    }
  }
  else if(ne->dir == EDGE_SRC_TGT)
  {
    if(!(e->attrs & MORIA_EDGE_HYPER))
    {
      n = vertex_value(e->B);
      if(n->bpe == &ne->bpe)
        n->bpe = 0;

      fatal(node_invalidate, n, invalidation);
    }
    else
    {
      for(x = 0; x < e->Blen; x++)
      {
        n = vertex_value(e->Blist[x].v);
        if(n->bpe == &ne->bpe)
          n->bpe = 0;

        fatal(node_invalidate, n, invalidation);
      }
    }
  }

  finally : coda;
}

xapi node_edge_disconnect(node_edge * restrict ne, graph_invalidation_context * restrict invalidation)
{
  enter;

  edge *e;
  vertex *v;
  node *n;
  int x;
  llist edges;
  llist vertices;

  /* initialize work lists */
  llist_init_node(&edges);
  llist_init_node(&vertices);

  e = edge_containerof(ne);
  llist_append(&edges, e, lln);

  while(!llist_empty(&edges) || !llist_empty(&vertices))
  {
    if((e = llist_first(&edges, typeof(*e), lln)))
    {
      llist_delete(e, lln);
      ne = edge_value(e);

      /*
       * list of nodes in the edge, in bottom-up order. The order matters because when a directory
       * node is considered, its downward edges must have already been deleted, if they are going to
       * be deleted here, so that it will be recognized as empty
       */
      if(!(e->attrs & MORIA_EDGE_HYPER))
      {
        if(!llist_attached(e->B, lln)) {
          llist_append(&vertices, e->B, lln);
        }

        if(!llist_attached(e->A, lln)) {
          llist_append(&vertices, e->A, lln);
        }
      }
      else
      {
        for(x = 0; x < e->Blen; x++) {
          if(!llist_attached(e->Blist[x].v, lln)) {
            llist_append(&vertices, e->Blist[x].v, lln);
          }
        }

        for(x = 0; x < e->Alen; x++) {
          if(!llist_attached(e->Alist[x].v, lln)) {
            llist_append(&vertices, e->Alist[x].v, lln);
          }
        }
      }

      if((e->attrs & EDGE_TYPE_OPT) == EDGE_TYPE_FS)
      {
        n = vertex_value(e->B);
        if(node_exists_get(n))
        {
          fatal(node_invalidate, n, invalidation);
          fatal(dirnode_children_changed, vertex_value(e->A), invalidation);
          node_state_set(n, VERTEX_UNLINKED);
        }

        /* only actually delete filesystem edge if no other references to the downard node exist */
        if(!rbtree_empty(&e->B->down) || !rbtree_empty(&e->B->up))
        {
          continue;
        }
      }
      else if((e->attrs & EDGE_TYPE_OPT) == EDGE_TYPE_STRONG)
      {
        fatal(strong_disconnecting, ne, e, invalidation);
      }

      fatal(disconnect_edge, ne, invalidation);
    }

    if((v = llist_first(&vertices, typeof(*v), lln)))
    {
      llist_delete(v, lln);

      if(!rbtree_empty(&v->down)) {
        continue;
      }

      n = vertex_value(v);
      if(node_exists_get(n)) {
        continue;
      }

      /* opportunistically delete rule-dir edges (up edges) */
      if(node_kind_get(n) == VERTEX_DIR) {
        fatal(fsedge_disconnecting, n, invalidation);
      }

      // node is still referenced, other than by its upwards facing identity edge
      if(!rbtree_empty(&v->up)) {
        continue;
      }

      if(v->up_identity) {
        if(!llist_attached(v->up_identity, lln)) {
          llist_append(&edges, v->up_identity, lln);
        }
        continue;
      }

      fatal(node_delete_node, n);
    }
  }

  finally : coda;
}

static xapi invalidate_visitor(vertex * v, void * arg, traversal_mode mode, int distance, int * result)
{
  enter;

  node * n = vertex_value(v);

  if(node_invalid_get(n)) {
    goto XAPI_FINALIZE;
  }

  node_invalid_set(n, true);
  node_invalidation_counter++;

  if(log_would(L_DEPGRAPH))
  {
    narrator * N;
    fatal(log_start, L_DEPGRAPH, &N);
    xsays("invalidate ");
    fatal(node_project_relative_path_say, n, N);
    fatal(log_finish);
  }

  finally : coda;
}

xapi node_invalidate(node * restrict n, graph_invalidation_context * restrict invalidation)
{
  enter;

  vertex *v;

  // invalidate consumers
  fatal(graph_traverse_vertices
    , g_graph
    , vertex_containerof(n)
    , invalidate_visitor
    , invalidation->vertex_traversal
    , (traversal_criteria[]) {{
          edge_travel : EDGE_INVALIDATIONS
        , edge_visit : EDGE_INVALIDATIONS
        , min_depth : 1
        , max_depth : UINT16_MAX
      }}
    , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
    , 0
  );

  // for the node itself, invalidate only if consumers exist
  v = vertex_containerof(n);
  if(!rbtree_empty(&v->down)) {
    fatal(invalidate_visitor, vertex_containerof(n), 0, 0, 0, 0);
  }

  if(node_filetype_get(n) == VERTEX_FILETYPE_DIR)
  {
    fatal(dirnode_children_changed, n, invalidation);
  }
  else if(node_nodetype_get(n) == VERTEX_NODETYPE_MODULE || node_nodetype_get(n) == VERTEX_NODETYPE_MODEL)
  {
    module_invalidated(node_module_get(n));
  }

  finally : coda;
}

xapi node_ok(node * restrict n)
{
  enter;

  if(node_state_get(n) == VERTEX_OK)
    goto XAPI_FINALIZE;

  node_state_set(n, VERTEX_OK);
  if(log_would(L_DEPGRAPH))
  {
    narrator * N;
    fatal(log_start, L_DEPGRAPH, &N);
    xsays("ok ");
    fatal(node_project_relative_path_say, n, N);
    fatal(log_finish);
  }

  finally : coda;
}

static xapi disintegrate_visitor(edge * e, void * ctx, traversal_mode mode, int distance, int * result)
{
  enter;

  llist *edges = ctx;
  llist_append(edges, e, lln);

  finally : coda;
}

xapi node_disintegrate_fs(node_edge * restrict ne, graph_invalidation_context * restrict invalidation)
{
  enter;

  llist edges = LLIST_INITIALIZER(edges);
  edge *e;

  // visit this edge and all edges below it
  fatal(graph_traverse_edges
    , g_graph
    , edge_containerof(ne)
    , disintegrate_visitor
    , 0
    , (traversal_criteria[]) {{
          edge_travel: EDGE_TYPE_FS
        , edge_visit : EDGE_TYPE_FS
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
    , &edges
  );

  while((e = llist_shift(&edges, typeof(*e), lln)))
  {
    ne = edge_value(e);
    fatal(node_edge_disconnect, ne, invalidation);
  }

  finally : coda;
}
