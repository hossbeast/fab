/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bam/events.h"
#include "bam/ipc.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "bacon/writer.h"
#include "xlinux/xtime.h"
#include "bacon/builder.h"
#include "valyria/llist.h"
#include "narrator/units.h"
#include "moria/vertex.h"

#include "goals.h"
#include "build.h"
#include "buildplan.h"
#include "fsent.h"
#include "channel.h"
#include "module.h"
#include "selector.h"
#include "channel.h"
#include "request_handler.h"
#include "shadow.h"
#include "vertex.h"

#include "common/attrs.h"
#include "events/events.h"
#include "zcat.h"
#include "times.h"
#include "params.h"
#include "graph.h"
#include "subsys.h"

uint32_t goals_id;
struct timespec goals_time;
pid_t goals_pid;
uint64_t goals_token;
char goals_label[128];
uint16_t goals_label_len;
bool goals_reconcile;
bool goals_build;
static selector *goals_target_direct_selector;
static selector *goals_target_transitive_selector;
static selector_context goals_selector_context;
subsys sys_goals;

//
// static
//

void goals_create_buildplan()
{
  buildplan_reset();
  fsent *fse;
  moria_vertex_traversal_state *state;
  rcu_thread rcu;

  rcu_enter(&rcu);

  /* targets selector, if any */
  if(goals_target_direct_selector || goals_target_transitive_selector) {
    goals_selector_context.mod = vertex_module_of(&g_base->vertex);
    goals_selector_context.cwd = g_base;

    if(goals_target_direct_selector)
    {
      goals_selector_context.iteration_type = SELECTION_ITERATION_TYPE_ORDER;
      selector_exec(goals_target_direct_selector, &goals_selector_context);
      buildplan_select_direct(&goals_selector_context.selection->list);
    }

    if(goals_target_transitive_selector)
    {
      goals_selector_context.iteration_type = SELECTION_ITERATION_TYPE_ORDER;
      selector_exec(goals_target_transitive_selector, &goals_selector_context);
      buildplan_select_transitive(&goals_selector_context.selection->list);
    }
  /* //targets, if referenced by any rules */
  } else if(!rbtree_empty(&g_shadow_targets->vertex.down)) {
    buildplan_add_transitive(&g_shadow_targets->vertex, 0);
  } else {
  /* otherwise, build everything (slow to compute) */
    moria_vertex_traversal_begin(&g_graph, &state);
    llist_foreach(&fsent_list, fse, vertex.owner) {
      if(fsent_is_primary(fse)) {
        continue;
      }

      buildplan_add_transitive(&fse->vertex, state);
    }
    moria_vertex_traversal_end(&g_graph, state);
  }

  if(buildplan_state == NOOP)
  {
    buildplan_finalize();

    if(buildplan_selection.selected_entities.size != 0)
    {
      buildplan_state = READY;
    }
  }

  rcu_leave(&rcu);
}

//
// public
//

void goals_setup()
{
  selector_context_init(&goals_selector_context);

  subsys_init(&sys_goals, VERTEX_SYS_GOALS);
  shadow_mkref(g_shadow_bm, sys_goals.vertex.label, sys_goals.vertex.label_len, &sys_goals.vertex);
}

void goals_cleanup()
{
  selector_context_destroy(&goals_selector_context);
  selector_free(goals_target_direct_selector);
  selector_free(goals_target_transitive_selector);
}

void goals_sys_describe(const struct moria_vertex * restrict v, bacon_writer * restrict builder_writer)
{
  struct timespec now;
  uint64_t ns;
  char buf[128];
  narrator_fixed fixed;

  { MAPPING
    STRING("//bm/sys-goals")
    { SET
      { MAPPING STRING("id") UINT(goals_id) }

      clock_gettime(CLOCK_MONOTONIC, &now);
      ns = timespec_delta(&g_params.starttime, &now);
      interval_say(ns / NSEC_PER_SEC, narrator_fixed_init(&fixed, buf, sizeof(buf)));
      { MAPPING STRING("duration") BYTES(fixed.s, fixed.l) }

      if(goals_pid) {
        MAPPING
        STRING("pid")
        UINT(goals_pid)
      }
      if(goals_token) {
        MAPPING
        STRING("token")
        HEX(goals_token)
      }
      if(goals_label_len) {
        MAPPING
        STRING("label")
        BYTES(goals_label, goals_label_len)
      }
      if(goals_reconcile) {
        STRING("reconcile");
      }
      if(goals_target_direct_selector) {
        MAPPING
        STRING("target-direct")
        { LIST
          selector_describe(goals_target_direct_selector, builder_writer);
        }
      }
      if(goals_target_transitive_selector) {
        MAPPING
        STRING("target-transitive")
        { LIST
          selector_describe(goals_target_transitive_selector, builder_writer);
        }
      }
      if(goals_build) {
        STRING("build");
      }
      vertex_relations_describe(v, builder_writer);
    }
  }
}

void goals_set_event(request_handler * restrict handler)
{
  bamipc_message *msg;
  events_context ectx;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;

  if(!(msg = events_bacon_would(&ectx, BAMIPC_EVENT_GOALS_SET))) {
    return;
  }

  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    STRING("goals-set")
    { SET
      goals_sys_describe(&sys_goals.vertex, builder_writer);
    }
  }

  msg->len = fixed.l;
  events_publish(&ectx);
}

void goals_set(
    pid_t pid
  , uint64_t token
  , const char *label
  , uint16_t label_len
  , bool reconcile
  , bool build
  , selector * restrict target_direct
  , selector * restrict target_transitive
)
{
  goals_id++;
  goals_pid = pid;
  goals_token = token;
  goals_label_len = zcatw(goals_label, sizeof(goals_label), label, label_len);
  goals_reconcile = reconcile;
  goals_build = build;
  selector_ifree(&goals_target_direct_selector);
  goals_target_direct_selector = target_direct;
  selector_ifree(&goals_target_transitive_selector);
  goals_target_transitive_selector = target_transitive;

  clock_gettime(CLOCK_MONOTONIC, &goals_time);

  goals_set_event(0);
}

void goals_sys_stats_build(const moria_vertex * restrict v, bool reset, struct bacon_writer * restrict builder_writer)
{
}
