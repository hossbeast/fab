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
#include "xapi.h"

#include "moria/operations.h"
#include "moria/graph.h"
#include "moria/vertex.h"
#include "moria/edge.h"

#include "node_operations_test.h"
#include "node.h"
#include "shadow.h"
#include "node_operations.h"
#include "filesystem.h"
#include "graph.h"

/* used in unit tests */
static graph_invalidation_context invalidation;

static xapi node_operations_setup(operations_dispatch * restrict dispatch, graph * restrict g)
{
  enter;

  fatal(graph_invalidation_begin, &invalidation);

  finally : coda;
}

static xapi node_operations_cleanup(operations_dispatch * restrict dispatch, graph * restrict g)
{
  enter;

  graph_invalidation_end(&invalidation);

  finally : coda;
}

static xapi node_operations_connect(graph * const restrict g, vertex * const restrict A, vertex * const restrict B, uint32_t attrs, edge ** restrict e, bool * restrict r)
{
  enter;

  node *An = vertex_value(A);
  node *Bn = vertex_value(B);

  if((attrs & EDGE_TYPE_FS) == EDGE_TYPE_FS)
    fatal(node_connect_fs, An, Bn, attrs, &invalidation, e, 0);
  else if((attrs & EDGE_TYPE_STRONG) == EDGE_TYPE_STRONG)
    fatal(node_connect_dependency, An, Bn, attrs, &invalidation, 0, 0);
  else
    fatal(node_connect_generic, An, Bn, attrs, &invalidation, e);

  finally : coda;
}

static xapi node_operations_refresh_vertex(graph * const restrict g, vertex * const restrict v)
{
  enter;

  node *n;

  n = vertex_value(v);

  node_state_set(n, VERTEX_OK);

  finally : coda;
}

static xapi node_operations_invalidate_vertex(graph * const restrict g, vertex * const restrict v)
{
  enter;

  node *n;

  n = vertex_value(v);
  fatal(node_invalidate, n, &invalidation);

  finally : coda;
}

static xapi node_operations_disconnect(graph * const restrict g, edge * restrict e)
{
  enter;

  fatal(node_edge_disconnect, e, &invalidation);

  finally : coda;
}

static xapi create_vertex(
    vertex ** const restrict rv
  , graph * const restrict g
  , uint32_t attrs
  , uint8_t opattrs
  , const char * const restrict label
  , uint16_t label_len
)
{
  enter;

  node *n;
  node *b;
  bool shadow = false;
  bool root = false;

  /* first segment in the identifier, and starting with double-slash */
  if(opattrs & MORIA_OPATTRS_INIT_SLASH) // 0x80000000)
  {
    shadow = true;
  }
  /* first segment in the identifier */
  else if(opattrs & MORIA_OPATTRS_INIT) // 0x40000000)
  {
    root = true;
  }

  // named shadow node lookup
  if(shadow && g_shadow)
  {
    if(memncmp(label, label_len, MMS("module")) == 0)
    {
      *rv = vertex_containerof(g_shadow_module);
    }
    else if(memncmp(label, label_len, MMS("modules")) == 0)
    {
      *rv = vertex_containerof(g_shadow_modules);
    }
    else
    {
      /* no such node at the top of the shadow hierarchy */
      RUNTIME_ABORT();
    }

    goto XAPI_FINALLY;
  }

  fatal(node_operations_create_vertex, rv, g, attrs | VERTEX_OK, label, label_len);
  n = vertex_value(*rv);

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
    fatal(node_operations_connect, g, vertex_containerof(b), vertex_containerof(n), EDGE_TYPE_FS, 0, 0);
  }

  finally : coda;
}

operations_dispatch * node_operations_test_dispatch = (operations_dispatch[]) {{
    .create_vertex = create_vertex
  , .connect = node_operations_connect
  , .disconnect = node_operations_disconnect
  , .refresh_vertex = node_operations_refresh_vertex
  , .invalidate_vertex = node_operations_invalidate_vertex
  , .setup = node_operations_setup
  , .cleanup = node_operations_cleanup
}};
