/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "xlinux/xstdlib.h"
#include "moria/edge.h"
#include "moria/connect.h"

#include "dir_rma_edge.h"
#include "graph.h"
#include "fsent.h"
#include "rma.h"

llist drm_list = LLIST_INITIALIZER(drm_list);
static llist drm_freelist = LLIST_INITIALIZER(drm_freelist);

//
// static
//

static void dir_rma_edge_dispose(dir_rma_edge * restrict drm)
{
  llist_delete(&drm->edge.owner);
  llist_append(&drm_freelist, &drm->edge.owner);
}

void dir_rma_edge_release(dir_rma_edge * restrict rde)
{
  /* this edge must have been previously excised from the graph */
  RUNTIME_ASSERT(moria_edge_degree(&rde->edge) == 0);

  dir_rma_edge_dispose(rde);
}

static void dir_rma_alloc(dir_rma_edge ** restrict drmp)
{
  dir_rma_edge *drm;

  if((drm = llist_shift(&drm_freelist, typeof(*drm), edge.owner))) {
    memset(drm, 0, sizeof(*drm));
  } else {
    xmalloc(&drm, sizeof(*drm));
  }

  moria_edge_init(&drm->edge, &g_graph);
  llist_append(&drm_list, &drm->edge.owner);

  *drmp = drm;
}

//
// public
//

void dir_rma_cleanup(void)
{
  dir_rma_edge *rde;

  while((rde = llist_shift(&drm_freelist, typeof(*rde), edge.owner))) {
    free(rde);
  }
}

void dir_rma_connect(fsent * restrict dir, rule_module_association *rma)
{
  dir_rma_edge *drm = 0;
  moria_connect_context ctx;
  int r;

  RUNTIME_ASSERT(vertex_filetype_get(&dir->vertex) == VERTEX_FILETYPE_DIR);

  r = moria_preconnect(&ctx, &g_graph, &dir->vertex, &rma->vertex, EDGE_DIR_RMA, NULL);
  if(r == MORIA_HASEDGE) {
    return;
  }
  RUNTIME_ASSERT(r == MORIA_NOEDGE);

  dir_rma_alloc(&drm);
  moria_connect(&ctx, &g_graph, &drm->edge, &dir->vertex, &rma->vertex, EDGE_DIR_RMA);

  /* cached */
  drm->rma = rma;
  drm->dir = dir;
}

void dir_rma_connect_visit(void * restrict ctx, fsent * restrict dir)
{
  struct rule_module_association *rma;

  rma = ctx;
  dir_rma_connect(dir, rma);
}
