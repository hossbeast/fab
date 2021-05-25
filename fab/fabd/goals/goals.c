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

#include "fab/events.h"
#include "fab/ipc.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "value/writer.h"
#include "valyria/llist.h"
#include "valyria/set.h"

#include "goals.h"
#include "build_thread.h"
#include "buildplan.h"
#include "fsent.h"
#include "handler.h"
#include "logging.h"
#include "module.internal.h"
#include "path_cache.h"
#include "selector.h"
#include "channel.h"

#include "common/attrs.h"
#include "events.h"

bool goals_autorun;
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
 * Add a node to the plan
 *
 * @distance - length of the path to this node from the target
 */
static xapi plan_visitor_direct(moria_vertex * v, void * arg, moria_traversal_mode mode, int distance, int * restrict result)
{
  enter;

  buildplan_context * restrict bpctx = arg;
  fsent * n;
  char path[512];

  n = containerof(v, fsent, vertex);

  if(!fsent_invalid_get(n)) {
    goto XAPI_FINALIZE;
  }

  /* shadow nodes can act as placeholders, but are never actually built */
  if(!n->dep && fsent_shadowtype_get(n)) {
    goto XAPI_FINALIZE;
  }

  /* this is an error case - node is not up to date, with no way to update it */
  else if(!n->dep)
  {
    if(n->bp_plan_id != buildplan_id)
    {
      bpctx->state = UNSATISFIED;
      fsent_path_znload(path, sizeof(path), n);
      fprintf(stderr, "no way to update %p %s state %s", n, path, attrs32_name_byvalue(graph_vertex_state_attrs, fsent_state_get(n)));
    }
    n->bp_plan_id = buildplan_id;
    goto XAPI_FINALIZE;
  }

  fatal(buildplan_add, n->dep, distance);

  finally : coda;
}

static xapi plan_visitor_transitive(moria_vertex * v, void * arg, moria_traversal_mode mode, int distance, int * restrict result)
{
  enter;

  buildplan_context * restrict bpctx = arg;

  fatal(moria_traverse_vertices
    , &g_graph
    , v
    , plan_visitor_direct
    , 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_DEPENDS
        , edge_visit : EDGE_DEPENDS
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

  selected *sn;

  llist_foreach(selection, sn, lln) {
    fatal(plan_visitor_transitive, (void*)sn->v, bpctx, 0, 0, 0);
  }

  finally : coda;
}

/*
 * add exactly those nodes in the selection to the plan
 */
static xapi plan_select_direct(llist * restrict selection, buildplan_context * restrict bpctx)
{
  enter;

  selected *sn;

  llist_foreach(selection, sn, lln) {
    fatal(plan_visitor_direct, (void*)sn->v, bpctx, 0, 0, 0);
  }

  finally : coda;
}

static xapi create_buildplan(buildplan_context * restrict bpctx, channel * restrict chan)
{
  enter;

  module *mod;

  fatal(buildplan_reset);

  bpctx->state = NOOP;
  if(goal_target_direct_selector || goal_target_transitive_selector)
  {
    goal_selector_context.mod = g_project_root->mod;
    goal_selector_context.chan = chan;

    if(goal_target_direct_selector)
    {
      fatal(selector_exec, goal_target_direct_selector, &goal_selector_context, SELECTION_ITERATION_TYPE_ORDER);
      fatal(plan_select_direct, &goal_selector_context.selection->list, bpctx);
    }

    if(goal_target_transitive_selector)
    {
      fatal(selector_exec, goal_target_transitive_selector, &goal_selector_context, SELECTION_ITERATION_TYPE_ORDER);
      fatal(plan_select_transitive, &goal_selector_context.selection->list, bpctx);
    }
  }
  else if(!rbtree_empty(&g_project_root->mod->shadow_targets->vertex.down))
  {
    // select the targets of the project module
    fatal(moria_traverse_vertices
      , &g_graph
      , &g_project_root->mod->shadow_targets->vertex
      , plan_visitor_direct
      , 0
      , (moria_traversal_criteria[]) {{
            edge_travel : EDGE_DEPENDS
          , edge_visit : EDGE_DEPENDS
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
    llist_foreach(&module_list, mod, vertex.owner) {
      fatal(moria_traverse_vertices
        , &g_graph
        , &mod->shadow_targets->vertex
        , plan_visitor_direct
        , 0
        , (moria_traversal_criteria[]) {{
              edge_travel : EDGE_DEPENDS
            , edge_visit : EDGE_DEPENDS
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

    if(buildplan_selection.selected_entities->size != 0)
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
  selector_free(goal_target_direct_selector);
  selector_free(goal_target_transitive_selector);

  finally : coda;
}

xapi goals_say(narrator * restrict N)
{
  enter;

  value_writer writer;

  value_writer_init(&writer);
  fatal(value_writer_open, &writer, N);

  fatal(value_writer_push_set, &writer);

  if(goal_target_direct_selector) {
    fatal(value_writer_push_mapping, &writer);
    fatal(value_writer_string, &writer, "target-direct");
    fatal(value_writer_push_list, &writer);
    fatal(selector_writer_write, goal_target_direct_selector, &writer);
    fatal(value_writer_pop_list, &writer);
    fatal(value_writer_pop_mapping, &writer);
  }
  if(goal_target_transitive_selector) {
    fatal(value_writer_push_mapping, &writer);
    fatal(value_writer_string, &writer, "target-transitive");
    fatal(value_writer_push_list, &writer);
    fatal(selector_writer_write, goal_target_transitive_selector, &writer);
    fatal(value_writer_pop_list, &writer);
    fatal(value_writer_pop_mapping, &writer);
  }
  if(goal_build) {
    fatal(value_writer_string, &writer, "build");
  }
  if(goal_script) {
    fatal(value_writer_string, &writer, "script");
  }

  fatal(value_writer_pop_set, &writer);
  fatal(value_writer_close, &writer);

finally:
  fatal(value_writer_destroy, &writer);
coda;
}

xapi goals_set(uint64_t msg_id, bool build, bool script, selector * restrict target_direct, selector * restrict target_transitive)
{
  enter;

  fabipc_message *msg;
  handler_context *handler;
  narrator *N;
  narrator_fixed fixed;

  goal_build = build;
  goal_script = script;
  selector_ifree(&goal_target_direct_selector);
  goal_target_direct_selector = target_direct;
  selector_ifree(&goal_target_transitive_selector);
  goal_target_transitive_selector = target_transitive;

  if(events_would(FABIPC_EVENT_GOALS, &handler, &msg)) {
    msg->id = msg_id;
    N = narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
    fatal(goals_say, N);
    events_publish(handler, msg);
  }

  finally : coda;
}

xapi goals_kickoff(handler_context * restrict ctx)
{
  enter;

  buildplan_context bpctx = { };

  // potentially re-create the build plan
  fatal(path_cache_reset);
  fatal(create_buildplan, &bpctx, ctx->chan);

  if(goal_build && bpctx.state == UNSATISFIED) {
    logf(L_WARN, "cannot build");
  }

  // kickoff
  if(goal_build && bpctx.state == READY)
  {
    ctx->state = HANDLER_BUILD_IN_PROGRESS;
    fatal(build_thread_launch, ctx);
  }
  else
  {
    ctx->state = HANDLER_BUILD_NONE;
  }

  finally : coda;
}
