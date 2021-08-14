/* Copyright (c) 2012-2018 Todd Freed <todd.freed@gmail.com>

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

#include "xlinux/xstdlib.h"
#include "moria/edge.h"
#include "moria/traverse.h"

#include "fsedge.h"
#include "fsent.h"
#include "module.h"

llist fsedge_list = LLIST_INITIALIZER(fsedge_list);                 // active
static llist fsedge_freelist = LLIST_INITIALIZER(fsedge_freelist);  // free

//
// static
//

static xapi disintegrate_visitor(moria_edge * e, void * ctx, moria_traversal_mode mode, int distance, int * result)
{
  enter;

  llist *edges = ctx;
  llist_append(edges, e, lln);

  finally : coda;
}

static xapi __attribute__((nonnull(1, 2))) node_invalidate_visitor_first(moria_vertex * v, void * arg, moria_traversal_mode mode, int distance, int * result)
{
  enter;

  *result = MORIA_TRAVERSE_PRUNE;
  fatal(fsent_invalidate_visitor, v, arg, mode, distance, 0);

  finally : coda;
}

static xapi linking(fsent *n, graph_invalidation_context * restrict invalidation)
{
  enter;

  module *mod;

  mod = fsent_module_get(n);

  if(memncmp(n->name.name, n->name.namel, MMS(fsent_name_module)) == 0) {
    if(mod) {
      /* propagate new module.bam to the immediate parent module */
      fatal(moria_traverse_vertices
        , &g_graph
        , &mod->dir_node->vertex
        , node_invalidate_visitor_first
        , 0
        , (moria_traversal_criteria[]) {{
              edge_travel : EDGE_FSTREE
            , vertex_visit : VERTEX_MODULE_DIR
          }}
        , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE
        , invalidation
      );
    }
  } else if(memncmp(n->name.name, n->name.namel, MMS(fsent_name_var)) == 0) {
    /* propagate : var.bam -> this and descendant modules */
    fatal(moria_traverse_vertices
      , &g_graph
      , &fsent_parent(n)->vertex
      , fsent_invalidate_visitor
      , 0
      , (moria_traversal_criteria[]) {{
            edge_travel : EDGE_FSTREE
          , vertex_visit : VERTEX_MODULE_DIR
        }}
      , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
      , invalidation
    );
  }

  finally : coda;
}

//
// public
//

xapi fsedge_cleanup()
{
  enter;

  llist *T;
  fsedge *fse;

  llist_foreach_safe(&fsedge_list, fse, edge.owner, T) {
    fsedge_release(fse);
  }

  llist_foreach_safe(&fsedge_freelist, fse, edge.owner, T) {
    wfree(fse);
  }

  finally : coda;
}

xapi fsedge_alloc(fsedge ** restrict rv, moria_graph * restrict g)
{
  enter;

  fsedge *fse;

  if((fse = llist_shift(&fsedge_freelist, typeof(*fse), edge.owner)) == 0)
  {
    fatal(xmalloc, &fse, sizeof(*fse));
  }

  moria_edge_init(&fse->edge, g);

  llist_append(&fsedge_list, fse, edge.owner);
  *rv = fse;

  finally : coda;
}

void fsedge_release(fsedge * restrict fse)
{
  llist_delete(fse, edge.owner);
  llist_append(&fsedge_freelist, fse, edge.owner);
}

#include "narrator.h"

xapi fsedge_connect(
    fsent * restrict A
  , fsent * restrict B
  , graph_invalidation_context * restrict invalidation
)
{
  enter;

  fsedge * e;
  vertex_shadowtype ash;
  vertex_shadowtype bsh;
  vertex_filetype ft;
  moria_connect_context ctx;
  int r;

  /* if the parent is a link, traverse to the referent */
  while(A->vertex.attrs & MORIA_VERTEX_LINK) {
    A = containerof(A->vertex.ref, fsent, vertex);
  }

moria_edge *eee;
  r = moria_preconnect(&ctx, &g_graph, &A->vertex, &B->vertex, EDGE_FSTREE, &eee);
  if(r == MORIA_HASEDGE) {
    goto XAPI_FINALLY;
  }
if(r != MORIA_NOEDGE)
{
  fatal(graph_edge_say, eee, g_narrator_stdout);
}
  RUNTIME_ASSERT(r == MORIA_NOEDGE);
  fatal(fsedge_alloc, &e, &g_graph);
  fatal(graph_connect, &ctx, &A->vertex, &B->vertex, &e->edge, EDGE_FSTREE);

  ash = fsent_shadowtype_get(A);
  bsh = fsent_shadowtype_get(B);

  /* propagate shadowtype */
  if(ash && !bsh)
  {
    fsent_shadowtype_set(B, VERTEX_SHADOWTYPE_FS);

    /* shadow nodes can become invalid but they never actually exist */
    fsent_state_set(B, VERTEX_OK);
  }
  if(ash)
  {
    B->mod = A->mod;
  }

  ft = fsent_filetype_get(A);
  if(ft != VERTEX_FILETYPE_DIR) {
    /* for cases where its not known that an fsent is a directory until a child
     * is linked under it (pattern-generate) */
    RUNTIME_ASSERT(ft == 0 || ft == VERTEX_FILETYPE_REG);
    fsent_filetype_set(A, VERTEX_FILETYPE_DIR);

    if(fsent_kind_get(A) == VERTEX_DIR) {
      fatal(fsent_lookup_index, A);
    }
  }

  ft = fsent_filetype_get(B);
  if(!ft) {
    fsent_filetype_set(B, VERTEX_FILETYPE_REG);
  } else if(fsent_kind_get(B) == VERTEX_DIR) {
    /* for cases where it is known up front that an fsent is a directory (e.g. walker) */
    fatal(fsent_lookup_index, B);
  }

  fatal(linking, B, invalidation);
  fatal(fsent_dirnode_children_changed, A, invalidation);

  finally : coda;
}

xapi fsedge_disconnect(fsedge * restrict fse)
{
  enter;

  fsent *B;

  B = containerof(fse->edge.B, fsent, vertex);
  if(fsent_kind_get(B) == VERTEX_DIR)
  {
    fatal(fsent_lookup_disindex, B);
  }

  fatal(graph_disconnect, &fse->edge);
  fsedge_release(fse);

  finally : coda;
}

xapi fsedge_disintegrate(fsedge * restrict fse, graph_invalidation_context * restrict invalidation)
{
  enter;

  llist edges = LLIST_INITIALIZER(edges);
  moria_edge *e;

  // visit this edge and all edges below it, in reverse depth order
  fatal(moria_traverse_edges
    , &g_graph
    , &fse->edge
    , disintegrate_visitor
    , 0
    , (moria_traversal_criteria[]) {{
          edge_travel: EDGE_TYPE_FSTREE
        , edge_visit : EDGE_TYPE_FSTREE
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_POST | MORIA_TRAVERSE_NOFOLLOW
    , &edges
  );

  while((e = llist_shift(&edges, typeof(*e), lln))) {
    fatal(graph_disintegrate, e, invalidation);
  }

  finally : coda;
}
