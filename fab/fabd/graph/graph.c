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

#include "valyria/map.h"
#include "valyria/dictionary.h"
#include "valyria/set.h"
#include "xlinux/xstat.h"
#include "moria/graph.h"
#include "moria/vertex.h"
#include "moria/operations.h"
#include "moria/edge.h"
#include "narrator.h"

#include "graph.h"
#include "node.h"
#include "rule.h"
#include "module.h"
#include "node_operations.h"
#include "logging.h"
#include "params.h"
#include "path.h"

#include "common/attrs.h"

graph * g_graph;

/* refresh state */
uint32_t graph_refresh_id;
static uint32_t graph_refresh_round_id;
static llist graph_rma_list[2];
static int graph_rma_list_index;

attrs32 * graph_node_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0
      VERTEX_KIND_TABLE
      VERTEX_STATE_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : y, range : r },
VERTEX_KIND_TABLE
VERTEX_STATE_TABLE
  }
}};

attrs32 * graph_edge_attrs = (attrs32[]) {{
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

attrs32 * graph_vertex_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0
      VERTEX_TYPE_TABLE
      VERTEX_NODETYPE_TABLE
      VERTEX_FILETYPE_TABLE
      VERTEX_SHADOWTYPE_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : y, range : r },
VERTEX_TYPE_TABLE
VERTEX_NODETYPE_TABLE
VERTEX_FILETYPE_TABLE
VERTEX_SHADOWTYPE_TABLE
  }
}};

attrs32 * graph_kind_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0
      VERTEX_KIND_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : y, range : r },
VERTEX_KIND_TABLE
  }
}};

attrs32 * graph_state_attrs = (attrs32[]) {{
#undef DEF
#define DEF(x, s, r, y) + 1
    num : 0
      VERTEX_STATE_TABLE
  , members : (member32[]) {
#undef DEF
#define DEF(x, s, r, y) { name : s, value : y, range : r },
VERTEX_STATE_TABLE
  }
}};

static void __attribute__((constructor)) init()
{
  attrs32_init(graph_edge_attrs);
  attrs32_init(graph_vertex_attrs);
  attrs32_init(graph_kind_attrs);
  attrs32_init(graph_state_attrs);
  attrs32_init(graph_node_attrs);
}

static xapi vertex_value_xdestroy(void * restrict val)
{
  enter;

  vertex * v;
  vertex_type type;

  v = vertex_containerof(val);
  type = v->attrs & VERTEX_TYPE_OPT;

  if(type == VERTEX_TYPE_NODE)
  {
    fatal(node_xdestroy, val);
  }
  else if(type == VERTEX_TYPE_RULE)
  {
    fatal(rule_xdestroy, val);
  }

  finally : coda;
}

static xapi edge_value_xdestroy(void * restrict val)
{
  enter;

  finally : coda;
}

/* visit rules attached to a directory, part of run-invalid-rules
 *
 * v - rule node
 */
static xapi rule_run_rma(rule_module_association * restrict rma, rule_run_context * restrict ctx)
{
  enter;

  if((ctx->variants = rma->variants) == 0) {
    ctx->variants = rma->mod->unscoped_block->variants;
  }
  ctx->mod = rma->mod;
  ctx->mod_owner = rma->mod_owner;
  ctx->rma = rma;

  fatal(rule_run, rma->rule, ctx);

  finally : coda;
}

//
// public
//

xapi graph_setup()
{
  enter;

  fatal(graph_createx, &g_graph, EDGE_TYPE_FS, GRAPH_VERTEX_VALUE_SIZE, GRAPH_EDGE_VALUE_SIZE, 0, vertex_value_xdestroy, 0, edge_value_xdestroy);

  graph_vertex_definitions_set(g_graph, graph_node_attrs);
  graph_edge_definitions_set(g_graph, graph_edge_attrs);

  graph_refresh_id = 1;
  graph_refresh_round_id = 1;
  llist_init_node(&graph_rma_list[0]);
  llist_init_node(&graph_rma_list[1]);

  finally : coda;
}

xapi graph_cleanup()
{
  enter;

  fatal(graph_xfree, g_graph);

  finally : coda;
}

xapi graph_edge_say(const edge * restrict e, narrator * restrict N)
{
  enter;

  node *n;
  vertex_type vt;
  int x;

  if(!(e->attrs & MORIA_EDGE_HYPER))
  {
    vt = e->A->attrs & VERTEX_TYPE_OPT;
    if(vt == VERTEX_TYPE_NODE)
      fatal(node_path_say, vertex_value(e->A), N);
    else if(vt == VERTEX_TYPE_RULE)
      fatal(narrator_xsayf, N, "(rule %p)", vertex_value(e->A));
  }
  else
  {
    for(x = 0; x < e->Alen; x++)
    {
      n = vertex_value(e->Alist[x].v);
      if(x)
        fatal(narrator_xsays, N, ", ");
      fatal(node_path_say, n, N);
    }
  }

  xsays(" : ");
  xsays(attrs32_name_byvalue(graph_edge_attrs, EDGE_TYPE_OPT & e->attrs) ?: "(wtf)");
  xsays(" : ");

  if(!(e->attrs & MORIA_EDGE_HYPER))
  {
    if((e->B->attrs & VERTEX_TYPE_OPT) == VERTEX_TYPE_NODE)
    {
      fatal(node_path_say, vertex_value(e->B), N);
      if(node_kind_get(vertex_value(e->B)) == VERTEX_SHADOW_LINK)
      {
        fatal(narrator_xsays, N, " -> ");
        fatal(node_path_say, vertex_value(e->B->ref), N);
      }
    }
    else if((e->B->attrs & VERTEX_TYPE_OPT) == VERTEX_TYPE_RULE)
    {
      fatal(narrator_xsayf, N, "(rule %p)", vertex_value(e->B));
    }
  }
  else
  {
    for(x = 0; x < e->Blen; x++)
    {
      n = vertex_value(e->Blist[x].v);
      if(x)
        fatal(narrator_xsays, N, ", ");
      fatal(node_path_say, n, N);
      if(node_kind_get(n) == VERTEX_SHADOW_LINK)
      {
        fatal(narrator_xsays, N, " -> ");
        fatal(node_path_say, vertex_value(vertex_containerof(n)->ref), N);
      }
    }
  }

  finally : coda;
}

xapi graph_invalidation_begin(graph_invalidation_context * restrict invalidation)
{
  enter;

  fatal(graph_vertex_traversal_begin, g_graph, &invalidation->vertex_traversal);
  fatal(graph_edge_traversal_begin, g_graph, &invalidation->edge_traversal);
  invalidation->any = false;

  finally : coda;
}

void graph_invalidation_end(graph_invalidation_context * restrict invalidation)
{
  graph_vertex_traversal_end(g_graph, invalidation->vertex_traversal);
  invalidation->vertex_traversal = 0;

  graph_edge_traversal_end(g_graph, invalidation->edge_traversal);
  invalidation->edge_traversal = 0;
}

xapi graph_full_refresh(rule_run_context * restrict ctx)
{
  enter;

  rule_module_association *rma;
  llist *head;
  node_edge_dependency *ne;
  llist *T;

  /* list of rmas run during the refresh operation */
  llist rma_refresh_list;

  head = &graph_rma_list[graph_rma_list_index];

  do
  {
    graph_refresh_id++;
    llist_init_node(&rma_refresh_list);

    /* run rules to quiescence */
    while(!llist_empty(head))
    {
      graph_invalidation_end(&ctx->invalidation);
      fatal(graph_invalidation_begin, &ctx->invalidation);

      /* new round - flip the rma list */
      graph_refresh_round_id++;
      graph_rma_list_index = !graph_rma_list_index;

      llist_foreach_safe(head, rma, changed[!graph_rma_list_index].lln, T) {
        fatal(rule_run_rma, rma, ctx);
        if(rma->refresh_id != graph_refresh_id) {
          llist_append(&rma_refresh_list, rma, refresh_lln);
          rma->refresh_id = graph_refresh_id;
        }

        llist_delete(rma, changed[!graph_rma_list_index].lln);
      }

      head = &graph_rma_list[graph_rma_list_index];
    }

    /* consider each rma visited on this iteration -> their rules */
    llist_foreach(&rma_refresh_list, rma, refresh_lln) {
      llist_foreach_safe(&rma->edges, ne, lln, T) {
        if(ne->refresh_id != graph_refresh_id) {
          fatal(node_edge_disconnect, edge_containerof(ne), &ctx->invalidation);
          llist_delete(ne, lln);
        }
      }
    }
  } while(!llist_empty(head));

  finally : coda;
}

void graph_rma_enqueue(rule_module_association * restrict rma)
{
  if(rma->changed[graph_rma_list_index].refresh_round_id == graph_refresh_round_id) {
    return;
  }

  llist_append(&graph_rma_list[graph_rma_list_index], rma, changed[graph_rma_list_index].lln);
  rma->changed[graph_rma_list_index].refresh_round_id = graph_refresh_round_id;
}

xapi graph_delete_vertex(vertex *restrict v)
{
  enter;

  if(((v->attrs & VERTEX_TYPE_OPT) == VERTEX_TYPE_NODE) && (v->attrs & VERTEX_SHADOWTYPE_OPT) == 0) {
    fatal(graph_identity_deindex, g_graph, v);
  }

  fatal(graph_vertex_delete, g_graph, v);

  finally : coda;
}
