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

#include "types.h"

#include "narrator.h"
#include "moria/operations.h"
#include "moria/graph.h"
#include "moria/vertex.h"
#include "moria/edge.h"
#include "moria/parser.h"

#include "node_operations_test.h"
#include "fsent.h"
#include "shadow.h"
#include "fsedge.h"
#include "dependency.h"
#include "filesystem.h"
#include "graph.h"
#include "selection.h"
#include "lookup.h"
#include "dependency.h"
#include "graph.h"

/* used in unit tests */
static graph_invalidation_context invalidation;

static void node_operations_setup(graph_parser * restrict parser)
{
  graph_invalidation_begin(&invalidation);
}

static void node_operations_cleanup(graph_parser * restrict parser)
{
  graph_invalidation_end(&invalidation);
}

static void node_operations_connect(
    graph_parser * const restrict parser
  , moria_vertex * const restrict A
  , moria_vertex * const restrict B
  , uint32_t attrs
  , moria_edge ** restrict e
)
{
  fsent *An = containerof(A, fsent, vertex);
  fsent *Bn = containerof(B, fsent, vertex);
  dependency *dep;

  if((attrs & EDGE_TYPE_OPT) == EDGE_TYPE_FSTREE)
  {
    fsedge_connect(An, Bn, &invalidation);
  }
  else if((attrs & EDGE_TYPE_OPT) == EDGE_TYPE_DEPENDS)
  {
    dependency_connect(An, Bn, attrs, &invalidation, &dep);
  }
  else
  {
    RUNTIME_ABORT();
  }
}

static void node_operations_refresh_vertex(graph_parser * restrict parser, moria_vertex * const restrict v)
{
  fsent *n;

  n = containerof(v, fsent, vertex);

  fsent_ok(n);
}

static void node_operations_invalidate_vertex(graph_parser * restrict parser, moria_vertex * const restrict v)
{
  fsent *n;

  n = containerof(v, fsent, vertex);
  fsent_invalidate(n, &invalidation);
}

static void node_operations_disconnect(graph_parser * restrict parser, moria_edge * restrict e)
{
  graph_disintegrate(e, &invalidation);
}

static void create_vertex(
    graph_parser * restrict parser
  , moria_vertex ** const restrict rv
  , uint32_t attrs
  , uint8_t opattrs
  , const char * const restrict label
  , uint16_t label_len
)
{
  fsent *n;
  fsent *b;
  bool shadow = false;
  bool root = false;

  /* first segment in the identifier, and starting with double-slash */
  if(opattrs & MORIA_OPATTRS_INIT_SLASH)
  {
    shadow = true;
  }
  /* first segment in the identifier */
  else if(opattrs & MORIA_OPATTRS_INIT)
  {
    root = true;
  }

  // named shadow node lookup
  if(shadow && g_shadow)
  {
    if(memncmp(label, label_len, MMS("module")) == 0)
    {
      *rv = &g_shadow_module->vertex;
    }
    else if(memncmp(label, label_len, MMS("modules")) == 0)
    {
      *rv = &g_shadow_modules->vertex;
    }
    else
    {
      /* no such node at the top of the shadow hierarchy */
      RUNTIME_ABORT();
    }

    goto XAPI_FINALLY;
  }

  fsent_create(&n, attrs, VERTEX_OK, label, label_len);
  *rv = &n->vertex;

  b = 0;
  if(root && g_root)
  {
    b = g_root;
  }
  else if(shadow && g_shadow)
  {
    b = g_shadow;
  }

  if(b)
  {
    node_operations_connect(parser, &b->vertex, &n->vertex, EDGE_TYPE_FSTREE, 0);
  }
}

void resolve_fragment(const char *frag, uint16_t fragl, fsent **entp)
{
  moria_vertex *v;

  moria_graph_linear_search(&fsent_list, frag, fragl, &v);
  RUNTIME_ASSERT(v);

  *entp = containerof(v, fsent, vertex);
}

void graph_say_lists(narrator * restrict N, llist ** vertex_lists, uint16_t vertex_lists_len, llist ** edge_lists, uint16_t edge_lists_len)
{
  fatal(moria_graph_say
    , &g_graph
    , vertex_lists
    , vertex_lists_len
    , edge_lists
    , edge_lists_len
    , graph_vertex_attrs
    , graph_edge_attrs
    , N
  );
}

void graph_say(narrator * restrict N)
{
  llist *vertex_lists[1];
  llist *edge_lists[2];

  vertex_lists[0] = &fsent_list;
  edge_lists[0] = &fsedge_list;
  edge_lists[1] = &dependency_list;

  graph_say_lists(N, vertex_lists, sizeof(vertex_lists) / sizeof(*vertex_lists), edge_lists, sizeof(edge_lists) / sizeof(*edge_lists));
}

moria_operations_dispatch * node_operations_test_dispatch = (moria_operations_dispatch[]) {{
    .create_vertex = create_vertex
  , .connect = node_operations_connect
  , .disconnect = node_operations_disconnect
  , .refresh_vertex = node_operations_refresh_vertex
  , .invalidate_vertex = node_operations_invalidate_vertex
  , .setup = node_operations_setup
  , .cleanup = node_operations_cleanup
}};
