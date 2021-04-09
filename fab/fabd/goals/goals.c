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
#include "value/writer.h"
#include "valyria/llist.h"
#include "valyria/set.h"

#include "goals.h"
#include "build.h"
#include "buildplan.h"
#include "fsent.h"
#include "channel.h"
#include "logging.h"
#include "module.internal.h"
#include "selector.h"
#include "channel.h"
#include "handler.h"
#include "path_cache.h"

#include "common/attrs.h"
#include "events.h"
#include "zbuffer.h"

bool goals_reconcile;
bool goals_build;
static bool goals_script;
static selector * goals_target_direct_selector;
static selector * goals_target_transitive_selector;
static selector_context goals_selector_context;

//
// static
//

xapi goals_create_buildplan()
{
  enter;

  module *mod;

  fatal(path_cache_reset);
  fatal(buildplan_reset);

  if(goals_target_direct_selector || goals_target_transitive_selector)
  {
    goals_selector_context.mod = g_project_root->mod;
//    goals_selector_context.chan = bpctx->chan;

    if(goals_target_direct_selector)
    {
      fatal(selector_exec, goals_target_direct_selector, &goals_selector_context, SELECTION_ITERATION_TYPE_ORDER);
      fatal(buildplan_select_direct, &goals_selector_context.selection->list);
    }

    if(goals_target_transitive_selector)
    {
      fatal(selector_exec, goals_target_transitive_selector, &goals_selector_context, SELECTION_ITERATION_TYPE_ORDER);
      fatal(buildplan_select_transitive, &goals_selector_context.selection->list);
    }
  }
  else if(!rbtree_empty(&g_project_root->mod->shadow_targets->vertex.down))
  {
    // select the targets of the project module
    fatal(moria_traverse_vertices
      , &g_graph
      , &g_project_root->mod->shadow_targets->vertex
      , buildplan_visitor_direct
      , 0
      , (moria_traversal_criteria[]) {{
            edge_travel : EDGE_DEPENDS
          , edge_visit : EDGE_DEPENDS
          , vertex_visit : VERTEX_FILETYPE_REG
          , min_depth : 1
          , max_depth : UINT16_MAX
        }}
      , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH | MORIA_TRAVERSE_EXHAUSTIVE
      , 0
    );
  }
  else
  {
    // otherwise select the targets of all modules
    llist_foreach(&module_list, mod, vertex.owner) {
      fatal(moria_traverse_vertices
        , &g_graph
        , &mod->shadow_targets->vertex
        , buildplan_visitor_direct
        , 0
        , (moria_traversal_criteria[]) {{
              edge_travel : EDGE_DEPENDS
            , edge_visit : EDGE_DEPENDS
            , vertex_visit : VERTEX_FILETYPE_REG
            , min_depth : 1
            , max_depth : UINT16_MAX
          }}
        , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH | MORIA_TRAVERSE_EXHAUSTIVE
        , 0
      );
    }
  }

  if(buildplan_state == NOOP)
  {
    fatal(buildplan_finalize);
    fatal(buildplan_report);

    if(buildplan_selection.selected_entities->size != 0)
    {
      buildplan_state = READY;
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

  fatal(selector_context_xdestroy, &goals_selector_context);
  selector_free(goals_target_direct_selector);
  selector_free(goals_target_transitive_selector);

  finally : coda;
}

xapi goals_say(narrator * restrict N)
{
  enter;

  value_writer writer;

  value_writer_init(&writer);
  fatal(value_writer_open, &writer, N);
  fatal(value_writer_push_set, &writer);

  if(goals_reconcile) {
    fatal(value_writer_string, &writer, "reconcile");
  }
  if(goals_target_direct_selector) {
    fatal(value_writer_push_mapping, &writer);
    fatal(value_writer_string, &writer, "target-direct");
    fatal(value_writer_push_list, &writer);
    fatal(selector_writer_write, goals_target_direct_selector, &writer);
    fatal(value_writer_pop_list, &writer);
    fatal(value_writer_pop_mapping, &writer);
  }
  if(goals_target_transitive_selector) {
    fatal(value_writer_push_mapping, &writer);
    fatal(value_writer_string, &writer, "target-transitive");
    fatal(value_writer_push_list, &writer);
    fatal(selector_writer_write, goals_target_transitive_selector, &writer);
    fatal(value_writer_pop_list, &writer);
    fatal(value_writer_pop_mapping, &writer);
  }
  if(goals_build) {
    fatal(value_writer_string, &writer, "build");
  }
  if(goals_script) {
    fatal(value_writer_string, &writer, "script");
  }

  fatal(value_writer_pop_set, &writer);
  fatal(value_writer_close, &writer);

finally:
  fatal(value_writer_destroy, &writer);
coda;
}

xapi goals_set(
    uint64_t msg_id
  , bool reconcile
  , bool build
  , bool script
  , selector * restrict target_direct
  , selector * restrict target_transitive
)
{
  enter;

  fabipc_message *msg;
  channel *chan;

  goals_reconcile = reconcile;
  goals_build = build;
  goals_script = script;
  selector_ifree(&goals_target_direct_selector);
  goals_target_direct_selector = target_direct;
  selector_ifree(&goals_target_transitive_selector);
  goals_target_transitive_selector = target_transitive;

  if(events_would(FABIPC_EVENT_GOALS, &chan, &msg)) {
    msg->id = msg_id;
    events_publish(chan, msg);
  }

  finally : coda;
}
