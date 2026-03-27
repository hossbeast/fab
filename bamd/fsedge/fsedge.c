/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <inttypes.h>

#include "xlinux/xstdlib.h"
#include "moria/edge.h"
#include "moria/traverse.h"
#include "moria/connect.h"
#include "narrator.h"
#include "common/hash.h"

#include "fsedge.h"
#include "fsent.h"
#include "module.h"
#include "vertex.h"
#include "filesystem.h"
#include "edge.h"
#include "edges.h"
#include "vertices.h"
#include "namespace.h"

#include "zcat.h"

llist fsedge_list = LLIST_INITIALIZER(fsedge_list);                 // active
static llist fsedge_freelist = LLIST_INITIALIZER(fsedge_freelist);  // free

//
// static
//

static void fsedge_dispose(fsedge * restrict fse)
{
  llist_delete(&fse->edge.owner);
  llist_append(&fsedge_freelist, &fse->edge.owner);
}

void fsedge_release(fsedge * restrict fse)
{
  int x;

  /* this edge must have been previously excised from the graph */
  if(fse->edge.attrs & MORIA_EDGE_HYPER)
  {
    for(x = 0; x < fse->edge.Alen; x++) {
      RUNTIME_ASSERT(!rbnode_attached(&fse->edge.Alist[x].rbn));
    }
    for(x = 0; x < fse->edge.Blen; x++) {
      RUNTIME_ASSERT(!rbnode_attached(&fse->edge.Blist[x].rbn));
    }
  }
  else
  {
    RUNTIME_ASSERT(!rbnode_attached(&fse->edge.rbn_up));
    RUNTIME_ASSERT(!rbnode_attached(&fse->edge.rbn_down));
  }

  fsedge_dispose(fse);
}

//
// public
//

void fsedge_cleanup()
{
  llist *T;
  fsedge *fse;

  llist_foreach_safe(&fsedge_list, fse, edge.owner, T) {
    fsedge_dispose(fse);
  }

  llist_foreach_safe(&fsedge_freelist, fse, edge.owner, T) {
    free(fse);
  }
}

void fsedge_alloc(fsedge ** restrict rv)
{
  fsedge *fse;

  if((fse = llist_shift(&fsedge_freelist, typeof(*fse), edge.owner)) == 0)
  {
    xmalloc(&fse, sizeof(*fse));
  }

  moria_edge_init(&fse->edge, &g_graph);

  llist_append(&fsedge_list, &fse->edge.owner);
  *rv = fse;
}

void fsedge_connect_ctx(
    fsent * restrict B
  , fsent * restrict A
  , moria_connect_context * restrict ctx
  , graph_invalidation_context * restrict invalidation
)
{
  fsedge *e;
  bool ash;
  bool bsh;
  vertex_filetype ft;

  fsedge_alloc(&e);
  moria_connect(ctx, &g_graph, &e->edge, &B->vertex, &A->vertex, EDGE_FS);

  /* compute final identifiers */
  fsent_compute_identifiers(B);

  /* fst assignment is propagated from the parent
   * importantly, fsent_fstree_epoch is not copied, this allows another fstree to be based on this fsent later
   * fstree assignments are overwritten in fsent_reconcile
   * fsents allocated after that point depend on this assignment */
  B->fst = A->fst;

  ash = vertex_shadow_get(&A->vertex);
  bsh = vertex_shadow_get(&B->vertex);

  /* propagate shadowtype */
  if(ash && !bsh)
  {
    vertex_shadow_set(&B->vertex, true);

    /* shadow nodes can become invalid but they never actually exist */
    vertex_state_set(&B->vertex, VERTEX_OK);
  }

  ft = vertex_filetype_get(&A->vertex);
  if(ft != VERTEX_FILETYPE_DIR) {
    /* for cases where its not known that an fsent is a directory until a child
     * is linked under it (pattern-generate) */
    RUNTIME_ASSERT(ft == 0 || ft == VERTEX_FILETYPE_REG);
    fsent_promote_dir(A);
  }

  ft = vertex_filetype_get(&B->vertex);
  if(!ft) {
    vertex_filetype_set(&B->vertex, VERTEX_FILETYPE_REG);
  } else if(vertex_type_get(&B->vertex) == VERTEX_DIR) {
    /* for cases where it is known up front that an fsent is a directory (e.g. walker) */
    fsent_promote_dir(B);
  }

  /* invalidate the dirnode */
  vertex_invalidate(&A->vertex, invalidation);

  /* for a newly created named file, if any module would find it upon reload, that module
     must be invalidated here */
  switch(vertex_type_get(&B->vertex)) {
    case VERTEX_FILE:
    case VERTEX_SYMLINK:
      break;
    default:
      return;
  }

  modules_invalidate_for_specials(B, invalidation);
}

void fsedge_connect(
    fsent * restrict B
  , fsent * restrict A
  , graph_invalidation_context * restrict invalidation
)
{
  moria_connect_context ctx;
  int r;

  /* links should have been resolved in the caller */
  RUNTIME_ASSERT(!(A->vertex.attrs & MORIA_VERTEX_LINK));
  RUNTIME_ASSERT(vertex_type_get(&A->vertex) != VERTEX_CONTEXT_MODULE);

  r = moria_preconnect(&ctx, &g_graph, &B->vertex, &A->vertex, EDGE_FS, 0);
  if(r == MORIA_HASEDGE) {
    return;
  }
  RUNTIME_ASSERT(r == MORIA_NOEDGE);

  fsedge_connect_ctx(B, A, &ctx, invalidation);
}

void fsedge_disconnecting(fsedge * restrict fse, graph_invalidation_context * restrict invalidation)
{
  /* dirnode is not invalidated here, but when the fsent is unlinked. Thats the point
   * after which it wont be matched by a subsequent search rule */
}
