/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria.h"
#include "moria/graph.h"
#include "moria/traverse.h"
#include "moria/connect.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "bacon/writer.h"
#include "bacon/builder.h"
#include "moria/foreach.h"
#include "xlinux/xstdlib.h"

#include "graph.h"
#include "dependency.h"
#include "fsedge.h"
#include "fsent.h"
#include "vertex.h"
#include "module.h"
#include "formula.h"
#include "rule.h"
#include "rma.h"
#include "ns_edge.h"
#include "fsname.h"
#include "ruleset.h"
#include "varset.h"
#include "dir_rma_edge.h"
#include "module_edge.h"
#include "edge.h"
#include "special_edge.h"
#include "meta_edge.h"
#include "entlist.h"
#include "bpe.h"
#include "configset.h"
#include "config_system.h"

#include "zcat.h"
#include "common/attrs.h"

#include "graph.h"

/* fs is the identity subgraph - moria treats it specially */
STATIC_ASSERT(EDGE_FS & MORIA_EDGE_IDENTITY);

moria_graph g_graph = MORIA_GRAPH_INITIALIZER(g_graph);

attrs32 *graph_vertex_attrs = (attrs32[]) {{
    num : 0
#undef DEF
#define DEF(x, s, r, y) + 1
      VERTEX_TYPE_TABLE
#undef DEF
#define DEF(x, s, n, r, y) + 1
      VERTEX_STATE_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : y, range : r },
VERTEX_TYPE_TABLE
#undef DEF
#define DEF(x, s, n, r, y) { name : s, value : y, range : r },
VERTEX_STATE_TABLE
    { name : "protect", value : VERTEX_PROTECT_BIT, range : VERTEX_PROTECT_BIT },
    { name : "shield", value : VERTEX_SHIELD_BIT, range : VERTEX_SHIELD_BIT },
  }
}};

attrs32 *graph_vertex_type_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0
      VERTEX_TYPE_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : y, range : r },
VERTEX_TYPE_TABLE
  }
}};

attrs32 *graph_filetype_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0
      VERTEX_FILETYPE_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : y, range : r },
VERTEX_FILETYPE_TABLE
  }
}};

attrs32 *graph_vertex_state_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, n, r, y) + 1
    num : 0
      VERTEX_STATE_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, n, r, y) { name : n, value : y, range : r },
VERTEX_STATE_TABLE
  }
}};

attrs32 *graph_edge_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0
      EDGE_TYPE_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : y, range : r },
EDGE_TYPE_TABLE
  }
}};

static void __attribute__((constructor)) init()
{
  attrs32_init(graph_edge_attrs);
  attrs32_init(graph_vertex_attrs);
  attrs32_init(graph_filetype_attrs);
}

static void vertex_release(moria_vertex * restrict v, graph_invalidation_context * restrict invalidation)
{
  switch(vertex_type_get(v)) {
    case VERTEX_CONFIGSET_FILE:
    case VERTEX_CONTEXT_MODULE:
    case VERTEX_DIR:
    case VERTEX_FILE:
    case VERTEX_FORMULA_FILE:
    case VERTEX_MODULE_DIR:
    case VERTEX_MODULE_FILE:
    case VERTEX_RULESET_FILE:
    case VERTEX_SHADOW_DIR:
    case VERTEX_SHADOW_FILE:
    case VERTEX_SHADOW_LINK:
    case VERTEX_SYMLINK:
    case VERTEX_VARSET_FILE:
      fsent_release(containerof(v, fsent, vertex));
      break;
    case VERTEX_BPE:
      bpe_release(containerof(v, buildplan_entity, vertex), invalidation);
      break;
    default:
      RTA;
  }
}

static void gather_vertices(moria_edge * restrict e, entlist * restrict vertices)
{
  int x;

  /*
   * save the incident vertices in bottom-up order. The order matters because when a directory
   * node is considered, its downward edges must have already been deleted, if they are going to
   * be deleted here, so that it will be recognized as empty
   */
  if(!(e->attrs & MORIA_EDGE_HYPER))
  {
    entlist_push(vertices, e->A);
    entlist_push(vertices, e->B);
  }
  else
  {
    for(x = 0; x < e->Alen; x++) {
      entlist_push(vertices, e->Alist[x].v);
    }

    for(x = 0; x < e->Blen; x++) {
      entlist_push(vertices, e->Blist[x].v);
    }
  }
}

//
// public
//

void graph_setup()
{
}

void graph_cleanup()
{
  moria_graph_destroy(&g_graph);
}

void graph_invalidation_begin(graph_invalidation_context * restrict invalidation)
{
  RUNTIME_ASSERT(invalidation->vertex_traversal == 0);
  moria_vertex_traversal_begin(&g_graph, &invalidation->vertex_traversal);

  RUNTIME_ASSERT(invalidation->edge_traversal == 0);
  moria_edge_traversal_begin(&g_graph, &invalidation->edge_traversal);
  invalidation->any = false;
}

void graph_invalidation_end(graph_invalidation_context * restrict invalidation)
{
  moria_vertex_traversal_end(&g_graph, invalidation->vertex_traversal);
  invalidation->vertex_traversal = 0;

  moria_edge_traversal_end(&g_graph, invalidation->edge_traversal);
  invalidation->edge_traversal = 0;
}

static void edge_release(
    moria_edge * restrict e
  , entlist * restrict edges
  , entlist * restrict vertices
  , graph_invalidation_context * restrict invalidation
)
{
  /* disconnecting */
  switch(edge_type_get(e)) {
    case EDGE_VARSET_MOD:
    case EDGE_RULESET_MOD:
      module_edge_disconnecting(containerof(e, module_edge, edge), edges, vertices, invalidation);
      break;
    case EDGE_FML_RMA:
    case EDGE_RULE_RMA:
    case EDGE_RMA_BPE:
    case EDGE_RMA_TGT:
    case EDGE_RMA_MOD:
    case EDGE_RMA_RULESET:
      meta_disconnecting(containerof(e, meta_edge, edge), edges, vertices, invalidation);
      break;
    case EDGE_BPE_TGT:
    case EDGE_SRC_BPE:
      dependency_disconnecting(containerof(e, dependency, edge), edges, vertices, invalidation);
      break;
    case EDGE_SPECIAL:
    case EDGE_DIR_MOD:
      special_disconnecting(containerof(e, special_edge, edge), invalidation);
      break;
    case EDGE_DIR_RMA:
    case EDGE_FSNAME_RMA:
    case EDGE_DIR_FSNAME:
    case EDGE_FSNAME_NS:
    case EDGE_FSTREE_NS:
    case EDGE_FSTREE:
    case EDGE_NS_RMA:
    case EDGE_LOOKUP:
      break;
    default:
      edge_show(e);
      RTA;
  }

  /* remove this edge from the graph */
  moria_edge_disconnect(&g_graph, e);

  /* release the edge */
  switch(edge_type_get(e)) {
    case EDGE_FSNAME_RMA:
    case EDGE_DIR_RMA:
      dir_rma_edge_release(containerof(e, dir_rma_edge, edge));
      break;
    case EDGE_SPECIAL:
    case EDGE_DIR_MOD:
      special_edge_release(containerof(e, special_edge, edge));
      break;
    case EDGE_VARSET_MOD:
    case EDGE_RULESET_MOD:
      module_edge_release(containerof(e, module_edge, edge));
      break;
    case EDGE_FML_RMA:
    case EDGE_RULE_RMA:
    case EDGE_RMA_BPE:
    case EDGE_RMA_TGT:
    case EDGE_RMA_MOD:
    case EDGE_RMA_RULESET:
    case EDGE_NS_RMA:
    case EDGE_LOOKUP:
      meta_edge_release(containerof(e, meta_edge, edge));
      break;
    case EDGE_FSTREE:
      fsedge_release(containerof(e, fsedge, edge));
      break;
    case EDGE_BPE_TGT:
    case EDGE_SRC_BPE:
      dependency_release(containerof(e, dependency, edge));
      break;
    case EDGE_DIR_FSNAME:
    case EDGE_FSNAME_NS:
    case EDGE_FSTREE_NS:
      ns_edge_release(containerof(e, ns_edge, edge));
      break;
    default:
      RTA;
  }
}

void graph_disintegrate_bulk(entlist * restrict edges, entlist * restrict vertices, graph_invalidation_context * restrict invalidation)
{
  entlist *eln;
  fsent *Bn;
  moria_vertex *v;
  moria_edge *e;
  moria_foreach_state st;
  bool any;

  while(!llist_empty(&edges->lln) || !llist_empty(&vertices->lln))
  {
    if((eln = entlist_shift(edges))) {
      e = eln->p;
      entlist_free(eln);

      switch(edge_type_get(e)) {
        case EDGE_FML_RMA:
          /* this is handled in bpe_system_reconcile */
          break;
        case EDGE_VARSET_MOD:
        case EDGE_RULESET_MOD:
        case EDGE_RMA_MOD:
        case EDGE_RMA_RULESET:
          /* modules are deleted during system reconciliation */
          break;
        case EDGE_DIR_FSNAME:
        case EDGE_FSNAME_NS:
        case EDGE_FSTREE_NS:
        case EDGE_NS_RMA:
          break;
        case EDGE_SPECIAL:
        case EDGE_DIR_MOD:
        case EDGE_RULE_RMA:
        case EDGE_RMA_BPE:
        case EDGE_RMA_TGT:
        case EDGE_BPE_TGT:
        case EDGE_SRC_BPE:
        case EDGE_DIR_RMA:
        case EDGE_FSNAME_RMA:
        case EDGE_LOOKUP:
        case EDGE_FSTREE:
          /* greedily proceed to all incident vertices */
          gather_vertices(e, vertices);
          break;
        default:
          edge_show(e);
          RTA;
      }

      switch(edge_type_get(e)) {
        case EDGE_SPECIAL:
        case EDGE_DIR_MOD:
        case EDGE_VARSET_MOD:
        case EDGE_RULESET_MOD:
        case EDGE_FML_RMA:
        case EDGE_RULE_RMA:
        case EDGE_RMA_BPE:
        case EDGE_RMA_TGT:
        case EDGE_RMA_MOD:
        case EDGE_RMA_RULESET:
        case EDGE_BPE_TGT:
        case EDGE_SRC_BPE:
        case EDGE_DIR_RMA:
        case EDGE_FSNAME_RMA:
        case EDGE_DIR_FSNAME:
        case EDGE_FSNAME_NS:
        case EDGE_FSTREE_NS:
        case EDGE_NS_RMA:
        case EDGE_LOOKUP:
          break;
        case EDGE_FSTREE:
          Bn = containerof(e->B, fsent, vertex);

          /* special nodes are handled in their own subsystem */
          if(e->B->attrs & VERTEX_PROTECT_BIT) {
            continue;
          }

          fsent_unlink(Bn, edges, invalidation);

          if(!rbtree_empty(&e->B->up)) {
            continue;
          }
          if(!rbtree_empty(&e->B->down)) {
            continue;
          }

          break;
        default:
          edge_show(e);
          RTA;
      }

      edge_release(e, edges, vertices, invalidation);
    }

    if((eln = entlist_shift(vertices))) {
      v = eln->p;
      entlist_free(eln);

      /* opportunistic delete */
      any = false;
      moria_foreach_edge_down(&st, v, e) {
        switch(edge_type_get(e)) {
          case EDGE_SPECIAL:
          case EDGE_DIR_MOD:
          case EDGE_VARSET_MOD:
          case EDGE_RULESET_MOD:
            entlist_push(edges, e);
            any = true;
          default:
            break;
        }
      }
      if(any) {
        continue;
      }

      if(!rbtree_empty(&v->up)) {
        continue;
      }
      if(!rbtree_empty(&v->down)) {
        continue;
      }
      if(v->attrs & VERTEX_PROTECT_BIT) {
        continue;
      }
      if(vertex_exists_get(v)) {
        continue;
      }

      // node has no references other than its upwards facing identity edge
      if(v->up_identity) {
        entlist_push(edges, v->up_identity);
        continue;
      }

      vertex_release(v, invalidation);
    }
  }
}

void graph_disintegrate(moria_edge * restrict e, graph_invalidation_context * restrict invalidation)
{
  entlist edges;
  entlist vertices;

  /* initialize work lists */
  entlist_init(&edges);
  entlist_init(&vertices);

  entlist_push(&edges, e);
  graph_disintegrate_bulk(&edges, &vertices, invalidation);

  entlist_destroy(&edges);
  entlist_destroy(&vertices);
}
