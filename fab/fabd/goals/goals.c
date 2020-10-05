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

#include "xapi.h"
#include "moria/vertex.h"
#include "valyria/set.h"
#include "narrator.h"
#include "fab/sigutil.h"

#include "goals.h"
#include "node.h"
#include "module.internal.h"
#include "buildplan.h"
#include "build_thread.h"
#include "selector.h"
#include "logging.h"
#include "path_cache.h"
#include "handler.h"
#include "params.h"

#include "common/attrs.h"
#include "events.h"
#include "zbuffer.h"

static bool goal_build;
static bool goal_script;
static selector * goal_target_direct_selector;
static selector * goal_target_transitive_selector;
static selector_context goal_selector_context;

typedef enum buildplan_state {
    NOOP          // nothing to do
  , UNSATISFIED   // cant build, unsatisfied dependencies
  , READY         // DEW IT
} buildplan_state;

typedef struct buildplan_context {
  buildplan_state state;
} buildplan_context;

//
// static
//

/*
 * Add a node to the plan - a node can be visited
 *
 * @distance - length of the path to this node from the target
 */
static xapi plan_visitor_direct(vertex * v, void * arg, traversal_mode mode, int distance, int * restrict result)
{
  enter;

  buildplan_context * restrict bpctx = arg;
  node * n;
  char path[512];

  n = vertex_value(v);

  if(!node_invalid_get(n)) {
    goto XAPI_FINALIZE;
  }

  /* shadow nodes can act as placeholders */
  if(!n->bpe && node_shadowtype_get(n)) {
    goto XAPI_FINALIZE;
  }

  /* this is an error case - node is not up to date, with no way to update it */
  else if(!n->bpe)
  {
    if(n->bp_plan_id != bp_plan_id)
    {
      bpctx->state = UNSATISFIED;
      node_path_znload(path, sizeof(path), n);
      logf(L_WARN, "no way to update %p %s state %s", n, path, attrs32_name_byvalue(graph_state_attrs, node_state_get(n)));
    }
    n->bp_plan_id = bp_plan_id;
    goto XAPI_FINALIZE;
  }

  fatal(buildplan_add, n->bpe, distance);

  finally : coda;
}

static xapi plan_visitor_transitive(vertex * v, void * arg, traversal_mode mode, int distance, int * restrict result)
{
  enter;

  buildplan_context * restrict bpctx = arg;

  fatal(graph_traverse_vertices
    , g_graph
    , v
    , plan_visitor_direct
    , 0
    , (traversal_criteria[]) {{
          edge_travel : EDGE_TYPE_STRONG
        , edge_visit : EDGE_TYPE_STRONG
        , vertex_visit : VERTEX_FILETYPE_REG
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
    , bpctx
  );

  finally : coda;
}

/*
 * add each node in the selection to the plan, along with its dependencies, recursively
 */
static xapi plan_select_transitive(llist * restrict selection, buildplan_context * restrict bpctx)
{
  enter;

  selected_node *sn;

  llist_foreach(selection, sn, lln) {
    fatal(plan_visitor_transitive, vertex_containerof(sn->n), bpctx, 0, 0, 0);
  }

  finally : coda;
}

/*
 * add exactly those nodes in the selection to the plan
 */
static xapi plan_select_direct(llist * restrict selection, buildplan_context * restrict bpctx)
{
  enter;

  selected_node *sn;

  llist_foreach(selection, sn, lln) {
    fatal(plan_visitor_direct, vertex_containerof(sn->n), bpctx, 0, 0, 0);
  }

  finally : coda;
}

static xapi create_buildplan(buildplan_context * restrict bpctx)
{
  enter;

  module *mod;

  fatal(buildplan_reset);

  bpctx->state = NOOP;
  if(goal_target_direct_selector || goal_target_transitive_selector)
  {
    goal_selector_context.base = g_project_root->mod->dir_node;
    goal_selector_context.mod = g_project_root->mod;

    if(goal_target_direct_selector)
    {
      fatal(selector_exec, goal_target_direct_selector, &goal_selector_context);
      fatal(plan_select_direct, &goal_selector_context.selection->list, bpctx);
    }

    if(goal_target_transitive_selector)
    {
      fatal(selector_exec, goal_target_transitive_selector, &goal_selector_context);
      fatal(plan_select_transitive, &goal_selector_context.selection->list, bpctx);
    }
  }
  else if(!rbtree_empty(&vertex_containerof(g_project_root->mod->shadow_targets)->down))
  {
    // select the targets of the project module
    fatal(graph_traverse_vertices
      , g_graph
      , vertex_containerof(g_project_root->mod->shadow_targets)
      , plan_visitor_direct
      , 0
      , (traversal_criteria[]) {{
            edge_travel : EDGE_TYPE_STRONG
          , edge_visit : EDGE_TYPE_STRONG
          , vertex_visit : VERTEX_FILETYPE_REG
          , min_depth : 1
          , max_depth : UINT16_MAX
        }}
      , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH | MORIA_TRAVERSE_EXHAUSTIVE
      , bpctx
    );
  }
  else
  {
    // otherwise select the targets of all modules
    llist_foreach(&g_modules, mod, lln_modules) {
      fatal(graph_traverse_vertices
        , g_graph
        , vertex_containerof(mod->shadow_targets)
        , plan_visitor_direct
        , 0
        , (traversal_criteria[]) {{
              edge_travel : EDGE_TYPE_STRONG
            , edge_visit : EDGE_TYPE_STRONG
            , vertex_visit : VERTEX_FILETYPE_REG
            , min_depth : 1
            , max_depth : UINT16_MAX
          }}
        , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH | MORIA_TRAVERSE_EXHAUSTIVE
        , bpctx
      );
    }
  }

  if(bpctx->state == NOOP)
  {
    fatal(buildplan_finalize);
    fatal(buildplan_report);

    if(bp_selection.selected_nodes->size != 0)
    {
      bpctx->state = READY;
    }
  }

  finally : coda;
}

//
// public
//

xapi goals_setup()
{
  enter;

  finally : coda;
}

xapi goals_cleanup()
{
  enter;

  fatal(selector_context_xdestroy, &goal_selector_context);

  finally : coda;
}

xapi goals_set(uint32_t msg_id, bool build, bool script, selector * restrict target_direct, selector * restrict target_transitive)
{
  enter;

  fabipc_message *msg;
  handler_context *handler;
  uint32_t tail;

  goal_build = build;
  goal_script = script;
  goal_target_direct_selector = target_direct;
  goal_target_transitive_selector = target_transitive;

  if(events_would(FABIPC_EVENT_GOALS, &handler, &msg, &tail)) {
    msg->id = msg_id;
    events_publish(handler, msg, tail);
  }

  finally : coda;
}

xapi goals_kickoff(handler_context * restrict ctx)
{
  enter;

  buildplan_context bpctx = { 0 };

  // potentially re-create the build plan
  fatal(path_cache_reset);
  fatal(create_buildplan, &bpctx);

  if(goal_build && bpctx.state == UNSATISFIED) {
    logf(L_WARN, "cannot build");
  }

  // kickoff
  if(goal_build && bpctx.state == READY)
  {
    fatal(build_thread_build, ctx);
  }
  else
  {
    ctx->build_state = FAB_BUILD_NONE;
  }

  finally : coda;
}
