/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bam/events.h"
#include "bam/ipc.h"
#include "narrator.h"
#include "bacon/writer.h"
#include "valyria/llist.h"

#include "goals.h"
#include "build.h"
#include "buildplan.h"
#include "fsent.h"
#include "channel.h"
#include "module.h"
#include "selector.h"
#include "channel.h"
#include "handler.h"
#include "path_cache.h"
#include "shadow.h"

#include "common/attrs.h"
#include "events.h"
#include "zcat.h"

bool goals_reconcile;
bool goals_build;
static bool goals_script;
static selector * goals_target_direct_selector;
static selector * goals_target_transitive_selector;
static selector_context goals_selector_context;

//
// static
//

void goals_create_buildplan()
{
  path_cache_reset();
  buildplan_reset();

  if(goals_target_direct_selector || goals_target_transitive_selector)
  {
    goals_selector_context.mod = fsent_module_get(g_cwd);
    goals_selector_context.cwd = g_cwd;

    if(goals_target_direct_selector)
    {
      selector_exec(goals_target_direct_selector, &goals_selector_context, SELECTION_ITERATION_TYPE_ORDER);
      buildplan_select_direct(&goals_selector_context.selection->list);
    }

    if(goals_target_transitive_selector)
    {
      selector_exec(goals_target_transitive_selector, &goals_selector_context, SELECTION_ITERATION_TYPE_ORDER);
      buildplan_select_transitive(&goals_selector_context.selection->list);
    }
  }
  else
  {
    /* otherwise, the //targets node is the default */
    buildplan_add_transitive(&g_shadow_targets->vertex);
  }

  if(buildplan_state == NOOP)
  {
    buildplan_finalize();

    if(buildplan_selection.selected_entities.size != 0)
    {
      buildplan_state = READY;
    }
  }
}

//
// public
//

void goals_setup()
{
  selector_context_init(&goals_selector_context);
}

void goals_cleanup()
{
  selector_context_destroy(&goals_selector_context);
  selector_free(goals_target_direct_selector);
  selector_free(goals_target_transitive_selector);
}

void goals_say(narrator * restrict N)
{
  bacon_writer writer;

  bacon_writer_init(&writer);
  bacon_writer_open(&writer, N);
  bacon_writer_push_set(&writer);

  if(goals_reconcile) {
    bacon_writer_string(&writer, "reconcile");
  }
  if(goals_target_direct_selector) {
    bacon_writer_push_mapping(&writer);
    bacon_writer_string(&writer, "target-direct");
    bacon_writer_push_list(&writer);
    selector_writer_write(goals_target_direct_selector, &writer);
    bacon_writer_pop_list(&writer);
    bacon_writer_pop_mapping(&writer);
  }
  if(goals_target_transitive_selector) {
    bacon_writer_push_mapping(&writer);
    bacon_writer_string(&writer, "target-transitive");
    bacon_writer_push_list(&writer);
    selector_writer_write(goals_target_transitive_selector, &writer);
    bacon_writer_pop_list(&writer);
    bacon_writer_pop_mapping(&writer);
  }
  if(goals_build) {
    bacon_writer_string(&writer, "build");
  }
  if(goals_script) {
    bacon_writer_string(&writer, "script");
  }

  bacon_writer_pop_set(&writer);
  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
}

void goals_set(
    uint64_t msg_id
  , bool reconcile
  , bool build
  , bool script
  , selector * restrict target_direct
  , selector * restrict target_transitive
)
{
  bamipc_message *msg;
  channel *chan;

  goals_reconcile = reconcile;
  goals_build = build;
  goals_script = script;
  selector_ifree(&goals_target_direct_selector);
  goals_target_direct_selector = target_direct;
  selector_ifree(&goals_target_transitive_selector);
  goals_target_transitive_selector = target_transitive;

  if(events_would(BAMIPC_EVENT_GOALS, &chan, &msg)) {
    msg->id = msg_id;
    events_publish(chan, msg);
  }
}
