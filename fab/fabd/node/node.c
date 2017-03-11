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

#include "moria/graph.h"
#include "moria/vertex.h"
#include "valyria/list.h"
#include "valyria/map.h"
#include "value.h"

#include "node.h"
#include "filesystem.h"
#include "reconfigure.h"
#include "path.h"
#include "params.h"

#include "zbuffer.h"

graph * g_node_graph;
node * g_root;
map * g_nodes_by_wd;

#define restrict __restrict

#if 0
static struct {
  const char * name;
  int opt;
} fstype_opts[] = {
#define NODE_FS_TYPE(a, b, c) [ (b) ] = { opt : (b), name : c },
NODE_FS_TYPE_TABLE
#undef NODE_FS_TYPE
};
#endif

static xapi dump_visitor(vertex * v, int distance, void * arg)
{
  enter;

  printf("%*s%s\n", distance * 2, "", v->label);

  finally : coda;
}

//
// public
//

xapi node_setup()
{
  enter;

  fatal(graph_createx, &g_node_graph, sizeof(node), node_destroy, 0);
  fatal(map_create, &g_nodes_by_wd);

  finally : coda;
}

xapi node_cleanup()
{
  enter;

  fatal(graph_xfree, g_node_graph);
  fatal(map_xfree, g_nodes_by_wd);

  finally : coda;
}

xapi node_reconfigure(reconfigure_context * ctx, const value * restrict config, uint32_t dry)
{
  enter;

  list * vertices;
  int x;

  if(!dry && ctx->filesystems_changed)
  {
    vertices = graph_vertices(g_node_graph);

    for(x = 0; x < vertices->l; x++)
    {
      node * n = vertex_value(list_get(vertices, x));
      if(n->fstype == NODE_FS_TYPE_DIR)
        n->fs = filesystem_lookup(n->name->path);
    }
  }

  finally : coda;
}

xapi node_createw(
    node ** restrict n
  , uint8_t fstype
  , const filesystem * restrict fs
  , const char * restrict name
  , size_t name_len
)
{
  enter;

  vertex * v;
  path * p = 0;

  fatal(graph_vertex_create, &v, g_node_graph, 0);
  fatal(path_createw, &p, name, name_len);
  *n = vertex_value(v);
  (*n)->name = p;
  p = 0;

  // the vertex label points at the node name
  v->label = (*n)->name->name;
  v->label_len = (*n)->name->namel;

  (*n)->fstype = fstype;
  (*n)->fs = fs;

  if((*n)->fstype == NODE_FS_TYPE_DIR)
    (*n)->wd = -1;

finally:
  fatal(path_xfree, &p);
coda;
}

xapi node_creates(
    node ** restrict n
  , uint8_t fstype
  , const filesystem * restrict fs
  , const char * restrict name
)
{
  xproxy(node_createw, n, fstype, fs, name, strlen(name));
}

xapi node_dump()
{
  enter;

  fatal(graph_traverse_vertices
    , g_node_graph
    , vertex_containerof(g_root)
    , dump_visitor
    , 0
    , 0
    , 0
    , NODE_RELATION_FS
    , NODE_RELATION_FS
    , GRAPH_TRAVERSE_DOWN | GRAPH_TRAVERSE_PRE
    , 0
  );

  finally : coda;
}

void node_destroy(node * restrict n)
{
  path_xfree(&n->name);
}

size_t node_get_path(node * restrict n, void * restrict dst, size_t sz)
{
  node * ns[64];
  size_t nsl = 0;
  size_t z = 0;
  int x;

  while(n && nsl < (sizeof(ns) / sizeof(ns[0])))
  {
    ns[nsl++] = n;
    n = n->fsparent;
  }

  z += znloads(dst + z, sz - z, g_params.proj_dir);

  /* skip the project root node */
printf(" - %.*s", ns[nsl - 1]->name->namel, ns[nsl - 1]->name->name);
  for(x = nsl - 2; x >= 0; x--)
  {
printf(" + %.*s", ns[x]->name->namel, ns[x]->name->name);
    z += znloads(dst + z, sz - z, "/");
    z += znloadw(dst + z, sz - z, ns[x]->name->name, ns[x]->name->namel);
  }
printf("\n");
  return z;
}
