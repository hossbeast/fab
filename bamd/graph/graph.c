/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria.h"
#include "moria/graph.h"
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
