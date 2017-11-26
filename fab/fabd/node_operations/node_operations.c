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

#include "xlinux/xunistd.h"
#include "moria/graph.h"
#include "moria/vertex.h"
#include "moria/edge.h"
#include "valyria/list.h"
#include "valyria/map.h"
#include "value.h"
#include "narrator.h"

#include "node_operations.h"
#include "logging.h"
#include "node.h"
#include "filesystem.h"

#include "macros.h"

#define restrict __restrict

static xapi disintegrate_visitor(edge * e, int distance, void * arg)
{
  enter;

  list * li = arg;
  fatal(list_push, li, e);

  finally : coda;
}

static xapi invalidate_visitor(vertex * v, int distance, void * arg)
{
  enter;

  node * n = vertex_value(v);
  n->invalid = 1;

  if(log_would(L_GRAPH))
  {
    narrator * N;
    fatal(log_start, L_GRAPH, &N);
    xsays("invalidate ");
    fatal(node_path_say, n, N);
    fatal(log_finish);
  }

  finally : coda;
}

//
// public
//

xapi node_connect(node * restrict parent, node * restrict n)
{
  enter;

  fatal(graph_connect_edge
    , g_node_graph
    , vertex_containerof(parent)
    , vertex_containerof(n)
    , NODE_RELATION_FS
  );
  n->fsparent = parent;

  if(log_would(L_GRAPH))
  {
    narrator * N;
    fatal(log_start, L_GRAPH, &N);
    xsays("connect RELATION_FS ");
    fatal(node_path_say, n, N);
    xsays(" : ");
    fatal(node_path_say, parent, N);
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
    , 0
    , 0
    , NODE_RELATION_FS
    , NODE_RELATION_FS
    , GRAPH_TRAVERSE_DOWN
    , li
  );

  int x;
  for(x = 0; x < li->l; x++)
    fatal(edge_disconnect, g_node_graph, list_get(li, x));

finally:
  fatal(list_xfree, li);
coda;
}

xapi node_refresh(node * restrict n, int traversal)
{
  enter;

  int fd = -1;

  if(n->fs->attrs == FILESYSTEM_INVALIDATE_ALWAYS)
  {
    fatal(node_invalidate, n, traversal);
  }
  else if(n->fs->attrs == FILESYSTEM_INVALIDATE_STAT)
  {
    
  }
  else if(n->fs->attrs == FILESYSTEM_INVALIDATE_CONTENT)
  {
    
  }

finally:
  fatal(ixclose, &fd);
coda;
}

xapi node_invalidate(node * restrict n, int traversal)
{
  enter;

  fatal(graph_traverse_vertices
    , g_node_graph
    , vertex_containerof(n)
    , invalidate_visitor
    , traversal
    , 0
    , 0
    , NODE_RELATION_STRONG | NODE_RELATION_WEAK   // travel weak edges
    , NODE_RELATION_STRONG                        // callback on strong edges
    , GRAPH_TRAVERSE_UP | GRAPH_TRAVERSE_PRE
    , 0
  );

  finally : coda;
}

int node_traversal_begin()
{
  return graph_traversal_begin(g_node_graph);
}
