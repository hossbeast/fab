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

#include "xlinux/xunistd.h"
#include "moria/graph.h"
#include "moria/operations.h"
#include "moria/vertex.h"
#include "moria/edge.h"
#include "moria/operations.h"
#include "valyria/list.h"
#include "valyria/map.h"
#include "value.h"
#include "narrator.h"

#include "node_operations.h"
#include "logging.h"
#include "node.internal.h"
#include "filesystem.h"
#include "path.h"

#include "macros.h"

static xapi disintegrate_visitor(edge * e, int distance, void * arg)
{
  enter;

  list * li = arg;
  fatal(list_push, li, e);

  finally : coda;
}

//
// public
//

static xapi graph_node_connect(graph * const restrict g, vertex * const restrict A, vertex * const restrict B, uint32_t attrs)
{
  enter;

  node_fstype_set(vertex_value(A), NODE_FSTYPE_DIR);
  node_fstype_set(vertex_value(B), NODE_FSTYPE_FILE);
  fatal(node_connect, vertex_value(A), vertex_value(B), attrs);

  finally : coda;
}

xapi node_connect(node * restrict A, node * restrict B, relation_type relation)
{
  enter;

  fatal(graph_connect
    , g_node_graph
    , vertex_containerof(A)
    , vertex_containerof(B)
    , relation
  );

  if(log_would(L_GRAPH))
  {
    narrator * N;
    fatal(log_start, L_GRAPH, &N);
    xsayf("%8s ", "connect");
    fatal(node_absolute_path_say, A, N);
    xsays(" : ");
    fatal(node_absolute_path_say, B, N);
    xsayf(" %s", relation_type_name(relation));
    fatal(log_finish);
  }

  finally : coda;
}

static xapi graph_node_disconnect(graph * const restrict g, vertex * const restrict A, vertex * const restrict B)
{
  xproxy(node_disconnect, vertex_value(A), vertex_value(B));
}

xapi node_disconnect(node * restrict A, node * restrict B)
{
  enter;

  fatal(graph_disconnect, g_node_graph, vertex_containerof(A), vertex_containerof(B));

  if(log_would(L_GRAPH))
  {
    narrator * N;
    fatal(log_start, L_GRAPH, &N);
    xsayf("%8s ", "disconnect");
    fatal(node_relative_path_say, A, N);
    xsays(" : ");
    fatal(node_relative_path_say, B, N);
    xsays(" RELATION_TYPE_FS");
    fatal(log_finish);
  }

  finally : coda;
}

xapi node_disintegrate_fs(edge * restrict e, int traversal)
{
  enter;

  list * li = 0;
  fatal(list_create, &li);

  fatal(graph_traverse_edges
    , g_node_graph
    , e
    , disintegrate_visitor
    , traversal
    , (traversal_criteria[]) {{
          edge_travel : RELATION_TYPE_FS
        , edge_visit : RELATION_TYPE_FS
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
    , li
  );

  int x;
  for(x = 0; x < li->l; x++)
  {
    edge * e = list_get(li, x);

    if(log_would(L_GRAPH))
    {
      narrator * N;
      fatal(log_start, L_GRAPH, &N);
      xsayf("%8s ", "disconnect");
      fatal(node_relative_path_say, vertex_value(e->A), N);
      xsays(" : ");
      fatal(node_relative_path_say, vertex_value(e->B), N);
      xsays(" RELATION_TYPE_FS");
      fatal(log_finish);
    }

    fatal(edge_disconnect, g_node_graph, e);
  }

finally:
  fatal(list_xfree, li);
coda;
}

xapi node_refresh(node * restrict n, int traversal)
{
  enter;

  if(n->fs->attrs == FILESYSTEM_INVALIDATE_ALWAYS)
  {
    fatal(node_invalidate, n, traversal);
  }
  else if(n->fs->attrs == FILESYSTEM_INVALIDATE_STAT)
  {
    // compare to stat hash
  }
  else if(n->fs->attrs == FILESYSTEM_INVALIDATE_CONTENT)
  {
    // compare to content hash
  }

  finally : coda;
}

static xapi invalidate_visitor(vertex * v, int distance, void * arg)
{
  enter;

  node * n = vertex_value(v);
  n->invalid = true;

  if(log_would(L_GRAPH))
  {
    narrator * N;
    fatal(log_start, L_GRAPH, &N);
    xsays("invalidate ");
    fatal(node_relative_path_say, n, N);
    fatal(log_finish);
  }

  finally : coda;
}

xapi node_invalidate(node * restrict n, int traversal)
{
  enter;

  fatal(graph_traverse_vertices
    , g_node_graph
    , vertex_containerof(n)
    , invalidate_visitor
    , traversal
    , (traversal_criteria[]) {{
          edge_travel : RELATION_TYPE_STRONG | RELATION_TYPE_WEAK   // travel weak edges
        , edge_visit : RELATION_TYPE_STRONG                         // callback on strong edges
      }}
    , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE
    , 0
  );

  finally : coda;
}

operations_dispatch * node_operations_dispatch = (operations_dispatch[]) {{
    .connect = graph_node_connect
  , .disconnect = graph_node_disconnect
  , .vertex_create = graph_node_create
}};
