/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria.h"
#include "moria/edge.h"
#include "moria/connect.h"

#include "graph.h"
#include "disintegrate.h"
#include "dependency.h"
#include "fsedge.h"
#include "vertex.h"
#include "ns_edge.h"
#include "dir_fsname_edge.h"
#include "dir_rma_edge.h"
#include "ref_edge.h"
#include "module_edge.h"
#include "edge.h"
#include "special_edge.h"
#include "meta_edge.h"
#include "edges.h"
#include "vertices.h"
#include "disintegrate/probes.h"

static void edge_release(moria_edge * restrict e, graph_invalidation_context * restrict invalidation)
{
  switch(edge_type_get(e)) {
    case EDGE_VARSET_MOD:
    case EDGE_RULESET_USES_MOD:
    case EDGE_RULESET_BUILD_MOD:
    case EDGE_RULESET_ANCHOR_MOD:
      module_edge_disconnecting(containerof(e, module_edge, edge), invalidation);
      break;
    case EDGE_FML_RMA:
    case EDGE_FML_BPE:
    case EDGE_RULE_RMA:
    case EDGE_RMA_BPE:
    case EDGE_RMA_BIND:
    case EDGE_RMA_MOD:
    case EDGE_RULE_RULESET:
    case EDGE_FML_REC_BPE:
    case EDGE_FML_REC_BUILD_REC:
    case EDGE_LOOKUP:
    case EDGE_LOOKUP_PLUGIN_FILE:
      meta_disconnecting(containerof(e, meta_edge, edge), invalidation);
      break;
    case EDGE_BPE_TGT:
    case EDGE_SRC_BPE:
      dependency_disconnecting(containerof(e, dependency, edge), invalidation);
      break;
    case EDGE_SPECIAL:
    case EDGE_ANCHOR_DIR_MOD:
    case EDGE_BUILD_DIR_MOD:
    case EDGE_FSENT_FML:
    case EDGE_FSENT_VARSET:
    case EDGE_FSENT_CONFIGSET:
    case EDGE_FSENT_RULESET:
      special_disconnecting(containerof(e, special_edge, edge), invalidation);
      break;
    case EDGE_REF_SYMLINK:
    case EDGE_SHADOW_REF:
      ref_disconnecting(containerof(e, ref_edge, edge), invalidation);
      break;
    case EDGE_DIR_FSNAME:
      dir_fsname_edge_disconnecting(containerof(e, dir_fsname_edge, edge), invalidation);
      break;
    case EDGE_FS:
      fsedge_disconnecting(containerof(e, fsedge, edge), invalidation);
    case EDGE_DIR_RMA:
    case EDGE_FSNAME_RMA:
    case EDGE_FSNAME_NS:
    case EDGE_FSTREE_NS:
    case EDGE_NS_RMA:
    case EDGE_FSNAME_MOD:
    case EDGE_NS_PLUGIN:
    case EDGE_FSNAME_PLUGIN:
      break;
  }

  /* remove this edge from the graph */
  moria_edge_disconnect(&g_graph, e);

  /* release the edge */
  switch(edge_type_get(e)) {
    case EDGE_DIR_RMA:
      dir_rma_edge_release(containerof(e, dir_rma_edge, edge));
      break;
    case EDGE_SPECIAL:
    case EDGE_ANCHOR_DIR_MOD:
    case EDGE_BUILD_DIR_MOD:
    case EDGE_FSENT_FML:
    case EDGE_FSENT_VARSET:
    case EDGE_FSENT_CONFIGSET:
    case EDGE_FSENT_RULESET:
      special_edge_release(containerof(e, special_edge, edge));
      break;
    case EDGE_REF_SYMLINK:
    case EDGE_SHADOW_REF:
      ref_edge_release(containerof(e, ref_edge, edge));
      break;
    case EDGE_VARSET_MOD:
    case EDGE_RULESET_USES_MOD:
    case EDGE_RULESET_BUILD_MOD:
    case EDGE_RULESET_ANCHOR_MOD:
      module_edge_release(containerof(e, module_edge, edge));
      break;
    case EDGE_FML_RMA:
    case EDGE_FML_BPE:
    case EDGE_RULE_RMA:
    case EDGE_RMA_BPE:
    case EDGE_RMA_BIND:
    case EDGE_RMA_MOD:
    case EDGE_RULE_RULESET:
    case EDGE_FML_REC_BPE:
    case EDGE_FML_REC_BUILD_REC:
    case EDGE_NS_RMA:
    case EDGE_LOOKUP:
    case EDGE_LOOKUP_PLUGIN_FILE:
    case EDGE_FSNAME_RMA:
    case EDGE_FSNAME_MOD:
    case EDGE_FSNAME_PLUGIN:
      meta_edge_release(containerof(e, meta_edge, edge));
      break;
    case EDGE_FS:
      fsedge_release(containerof(e, fsedge, edge));
      break;
    case EDGE_BPE_TGT:
    case EDGE_SRC_BPE:
      dependency_release(containerof(e, dependency, edge));
      break;
    case EDGE_DIR_FSNAME:
      dir_fsname_edge_release(containerof(e, dir_fsname_edge, edge));
      break;
    case EDGE_FSNAME_NS:
    case EDGE_FSTREE_NS:
      ns_edge_release(containerof(e, ns_edge, edge));
      break;
    case EDGE_NS_PLUGIN:
      RTA;
  }
}

static void disintegrate(graph_disintegrate_context * restrict ctx)
{
  struct vertices *vi;
  struct edges *ei;
  moria_vertex *v;
  moria_edge *e;
  int x;

  disintegrate_probe(ctx);

  while(!llist_empty(&ctx->edges->lln) || !llist_empty(&ctx->unlinking->lln) || !llist_empty(&ctx->releasing->lln))
  {
    disintegrate_round_probe(ctx);

    if((ei = edges_shift(ctx->edges))) {
      e = ei->p;
      edges_free(ei);

      if((e->attrs & MORIA_EDGE_HYPER)) {
        for(x = 0; x < e->Alen; x++) {
          vertices_push(ctx->releasing, e->Alist[x].v);
        }
        for(x = 0; x < e->Blen; x++) {
          vertices_push(ctx->releasing, e->Blist[x].v);
        }
      } else {
        vertices_push(ctx->releasing, e->A);
        vertices_push(ctx->releasing, e->B);
      }

      edge_release(e, ctx->invalidation);

      /* only consider a vertex again after all of its edges have been considered */
      continue;
    }

    if((vi = vertices_shift(ctx->unlinking))) {
      v = vi->p;
      vertices_free(vi);

      if(!vertex_exists_get(v)) {
        disintegrate_keep_unlinked_probe(v);
        continue;
      }

      vertex_unlink(v, ctx);
      vertices_push(ctx->releasing, v);

      /* only consider a vertex for release after its edges have been considered */
      continue;
    }

    if((vi = vertices_shift(ctx->releasing))) {
      v = vi->p;
      vertices_free(vi);

      if(!vertex_is_unlinked(v)) {
        if(vertex_reap(v, ctx)) {
          disintegrate_reap_probe(v);

          /* force UNLINKED nodes to be considered */
          vertex_exists_set(v, true);

          vertex_unlink(v, ctx);
          vertices_push(ctx->releasing, v);

          /* come back to this after its edges if any have been released */
          continue;
        }
      }

      if(vertex_exists_get(v)) {
        disintegrate_keep_exists_probe(v);
        continue;
      }

      if(!rbtree_empty(&v->up)) {
        disintegrate_keep_has_up_probe(v);
        continue;
      }
      if(!rbtree_empty(&v->down)) {
        disintegrate_keep_has_down_probe(v);
        continue;
      }

      /* an fsent can be unlinked, and yet remain. only delete the identity edge when it is
       * affirmatively deleted */
      if(v->up_identity) {
        vertices_push(ctx->releasing, v->up_identity->A);
        edge_release(v->up_identity, ctx->invalidation);
      }

      vertex_release(v);
    }
  }

  edges_destroy(ctx->edges);
  vertices_destroy(ctx->unlinking);
}

void graph_disintegrate(
    struct edges * restrict edges
  , struct vertices * restrict unlinking
  , struct vertices * restrict releasing
  , struct graph_invalidation_context * restrict invalidation
)
{
  graph_disintegrate_context ctx;
  ctx.invalidation = invalidation;
  ctx.unlinking = unlinking;
  ctx.releasing = releasing;
  ctx.edges = edges;

  disintegrate(&ctx);
}

void graph_demolish(moria_vertex * restrict v)
{
  struct edges edges;
  struct vertices unlinking;
  struct vertices releasing;
  graph_invalidation_context invalidation = { };

  graph_invalidation_begin(&invalidation);
  edges_init(&edges);
  vertices_init(&unlinking);
  vertices_init(&releasing);
  vertices_push(&unlinking, v);
  graph_disintegrate(&edges, &unlinking, &releasing, &invalidation);
  graph_invalidation_end(&invalidation);
}
