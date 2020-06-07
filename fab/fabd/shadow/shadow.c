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

#include <string.h>

#include "types.h"
#include "xapi.h"

#include "moria/edge.h"
#include "moria/operations.h"
#include "narrator.h"

#include "shadow.h"
#include "node.internal.h"
#include "path.h"
#include "filesystem.internal.h"
#include "module.h"
#include "node_operations.h"
#include "graph.h"
#include "MODULE.errtab.h"
#include "logging.h"
#include "stats.h"

#include "zbuffer.h"
#include "common/attrs.h"

uint8_t node_shadow_epoch;

node * g_shadow;
node * g_shadow_module;
node * g_shadow_modules;

static xapi shadow_root_create()
{
  enter;

  vertex * v;
  node * n;
  path * p = 0;

  STATS_INC(nodes_shadow);
  fatal(vertex_create, &v, g_graph, VERTEX_FILETYPE_DIR | VERTEX_TYPE_NODE | VERTEX_SHADOWTYPE_FS);
  n = vertex_value(v);

  fatal(path_creates, &p, "(shadow)");
  n->name = p;
  p = 0;

  v->label = n->name->name;
  v->label_len = n->name->namel;

  n->wd = -1;
  n->fst = &fstree_shadow;
  n->fs = n->fst->fs;

  g_shadow = n;

finally:
  fatal(path_free, p);
coda;
}

static xapi shadow_link(
    node * restrict parent
  , module * restrict mod
  , const char * restrict name
  , uint16_t namel
  , node * restrict ref
  , edge ** restrict nep
  , node ** restrict displacedp
  , graph_invalidation_context * restrict invalidation
)
{
  enter;

  vertex * v;
  node * n;
  path * p = 0;
  node * displaced;
  edge *ne;
  path plocal;

  fatal(vertex_create, &v, g_graph, VERTEX_SHADOW_LINK);
  n = vertex_value(v);

  /* temp name - might delete this vertex */
  n->name = &plocal;
  plocal.name = (void*)name;
  plocal.namel = namel;
  v->label = n->name->name;
  v->label_len = n->name->namel;
  v->ref = vertex_containerof(ref);

  n->wd = -1;
  n->fst = &fstree_shadow;
  n->fs = n->fst->fs;
  n->mod = mod;

  fatal(node_connect_fs, parent, n, EDGE_TYPE_FS, invalidation, &ne, &displaced);

  if(nep) {
    *nep = ne;
  }

  if(displacedp) {
    *displacedp = 0;
  }

  /* existing link by this name - same referent */
  if(!displaced && ne->B != v)
  {
    n->name = 0;
    fatal(graph_vertex_delete, g_graph, v);
  }
  else
  {
    STATS_INC(nodes_shadow);
    fatal(path_createw, &p, name, namel);
    n->name = p;
    p = 0;
    v->label = n->name->name;
    v->label_len = n->name->namel;

    /* existing link replaced with a link to some other referent */
    if(displaced)
    {
      *displacedp = displaced;
    }
    /* otherwise, no existing link by this name */
  }

finally:
  fatal(path_free, p);
coda;
}

static xapi shadow_create(
    node * restrict parent
  , node ** restrict np
  , module * restrict mod
  , const char * restrict name
  , uint16_t namel
  , uint32_t attrs
  , graph_invalidation_context * restrict invalidation
)
{
  enter;

  vertex * v;
  node * n;
  path * p = 0;

  STATS_INC(nodes_shadow);
  fatal(vertex_create, &v, g_graph, attrs);
  n = vertex_value(v);

  fatal(path_createw, &p, name, namel);
  n->name = p;
  p = 0;

  v->label = n->name->name;
  v->label_len = n->name->namel;

  n->wd = -1;
  n->fst = &fstree_shadow;
  n->fs = n->fst->fs;
  n->mod = mod;

  fatal(node_connect_fs, parent, n, EDGE_TYPE_FS, invalidation, 0, 0);

  if(np) {
    *np = n;
  }

finally:
  fatal(path_free, p);
coda;
}

//
// public
//

xapi shadow_setup()
{
  enter;

  graph_invalidation_context invalidation = { 0 };

  fatal(shadow_root_create);

  fatal(graph_invalidation_begin, &invalidation);
  fatal(shadow_create, g_shadow, &g_shadow_module, 0, MMS("module"), VERTEX_SHADOW_MODULE, &invalidation);
  fatal(shadow_create, g_shadow, &g_shadow_modules, 0, MMS("modules"), VERTEX_SHADOW_MODULES, &invalidation);

  node_shadow_epoch = 1;

finally:
  graph_invalidation_end(&invalidation);
coda;
}

void shadow_generation()
{
  node_shadow_epoch++;
}

xapi shadow_cleanup()
{
  enter;

  finally : coda;
}

xapi shadow_graft_module(struct module * restrict mod, graph_invalidation_context * restrict invalidation)
{
  enter;

  fatal(shadow_create, g_shadow_modules, &mod->shadow, mod, MMS(mod->id), VERTEX_SHADOW_DIR, invalidation);
  fatal(shadow_create, mod->shadow, &mod->shadow_imports, mod, MMS("imports"), VERTEX_SHADOW_DIR, invalidation);
  fatal(shadow_create, mod->shadow, &mod->shadow_targets, mod, MMS("targets"), VERTEX_SHADOW_FILE, invalidation);
  fatal(shadow_create, mod->shadow, &mod->shadow_scope, mod, MMS("scope"), VERTEX_SHADOW_DIR, invalidation);

  /* //module/this/fs -> module directory node */
  fatal(shadow_link, mod->shadow, mod, MMS("fs"), mod->dir_node, 0, 0, invalidation);

  finally : coda;
}

xapi shadow_graft_imports(struct module * restrict mod, node * restrict ref, const char * restrict as, uint16_t asl, edge ** restrict nep, graph_invalidation_context * restrict invalidation)
{
  enter;

  node *displaced = 0;

  fatal(shadow_link, mod->shadow_imports, mod, as, asl, ref, nep, &displaced, invalidation);

  /* imports must be unique by name */
  if(!displaced)
  {

  }
  else
  {
    logf(L_WARN, "import '%.*s' displaces earlier imported name", (int)asl, as);

    /* the displaced vertex was a link; it is now orphaned and can be deleted */
    STATS_DEC(nodes_shadow);

    fatal(graph_delete_vertex, vertex_containerof(displaced));
  }

  finally : coda;
}

xapi shadow_graft_scope(struct module * restrict mod, node * restrict ref, const char * restrict as, uint16_t asl, bool overwrite, edge ** restrict nep, graph_invalidation_context * restrict invalidation)
{
  enter;

  node *displaced = 0;

  // this will always replace what was in the scope, if anything
  fatal(shadow_link, mod->shadow_scope, mod, as, asl, ref, nep, &displaced, invalidation);

  if(!displaced)
  {

  }
  else
  {
    if(overwrite)
    {
      /* module directory entry has replaced a required node */
      logf(L_WARN, "directory entry '%.*s' displaces required node under that name", (int)asl, as);
    }
    else
    {
      /* required node has replaced a module directory entry */
      logf(L_WARN, "required node '%.*s' displaces directory entry of the same name", (int)asl, as);
    }

    /* the displaced vertex was a link; it is now orphaned and can be deleted */
    STATS_DEC(nodes_shadow);
    fatal(graph_delete_vertex, vertex_containerof(displaced));
  }

  finally : coda;
}

static xapi prune_visitor(edge * e, void * ctx, traversal_mode mode, int distance, int * result)
{
  enter;

  llist *lln = ctx;
  node_edge_imports *ne;

  ne = edge_value(e);

  if(ne->shadow_epoch != node_shadow_epoch) {
    llist_append_node(lln, &e->lln);
  }

  finally : coda;
}

xapi shadow_prune_imports(struct module * restrict mod, graph_invalidation_context * restrict invalidation)
{
  enter;

  llist lln, *cursor;
  edge *e, *scope_edge, *imports_edge;
  node_edge_imports *ne;

  llist_init_node(&lln);
  fatal(graph_traverse_vertex_edges
    , g_graph
    , vertex_containerof(mod->dir_node)
    , prune_visitor
    , 0
    , (traversal_criteria[]) {{
          edge_travel: EDGE_TYPE_IMPORTS
        , edge_visit : EDGE_TYPE_IMPORTS
        , min_depth : 0
        , max_depth : 0
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
    , &lln
  );

  llist_foreach_safe(&lln, e, lln, cursor) {
    ne = edge_value(e);

    /* save the related edges */
    scope_edge = edge_containerof(ne->scope_edge);
    imports_edge = edge_containerof(ne->imports_edge);

    /* import not refreshed in this generation - remove IMPORTS edge */
    fatal(graph_edge_disconnect, g_graph, e);

    /* also remove from //module/imports */
    if(imports_edge)
    {
      STATS_DEC(nodes_shadow);
      fatal(graph_edge_disconnect, g_graph, imports_edge);
      fatal(graph_delete_vertex, imports_edge->B);
    }

    /* also remove from //module/scope */
    if(scope_edge)
    {
      STATS_DEC(nodes_shadow);
      fatal(graph_edge_disconnect, g_graph, scope_edge);
      fatal(graph_delete_vertex, scope_edge->B);
    }
  }

  finally : coda;
}
