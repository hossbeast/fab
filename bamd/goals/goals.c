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
#include "handler.h"
#include "path_cache.h"
#include "shadow.h"
#include "vertex.h"

#include "common/attrs.h"
#include "events.h"
#include "zcat.h"
#include "times.h"
#include "params.h"
#include "graph.h"

uint32_t goals_id;
struct timespec goals_time;
pid_t goals_pid;
uint64_t goals_token;
char goals_label[128];
uint16_t goals_label_len;
bool goals_reconcile;
bool goals_build;
static selector * goals_target_direct_selector;
static selector * goals_target_transitive_selector;
static selector_context goals_selector_context;
moria_vertex *g_goals = (moria_vertex[]) {{ }};

//
// static
//

void goals_create_buildplan()
{
  path_cache_reset();
  buildplan_reset();
  fsent *fse;
  moria_vertex_traversal_state *state;

  /* targets selector, if any */
  if(goals_target_direct_selector || goals_target_transitive_selector) {
    goals_selector_context.mod = vertex_module_get(&g_base->vertex);
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
  /* //targets, if used */
  } else if(!rbtree_empty(&g_shadow_targets->vertex.down)) {
    buildplan_add_transitive(&g_shadow_targets->vertex, 0);
  /* otherwise, build everything (slow to compute) */
  } else {
    moria_vertex_traversal_begin(&g_graph, &state);
    llist_foreach(&fsent_list, fse, vertex.owner) {
      switch(vertex_type_get(&fse->vertex)) {
        case VERTEX_FILE:
          break;
        default:
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
}

//
// public
//

void goals_setup()
{
  selector_context_init(&goals_selector_context);

  moria_vertex_initw(g_goals, &g_graph, VERTEX_GOALS | VERTEX_OK, MMS("(goals)"));
}

void goals_cleanup()
{
  selector_context_destroy(&goals_selector_context);
  selector_free(goals_target_direct_selector);
  selector_free(goals_target_transitive_selector);
}

void goals_describe(bacon_writer * restrict builder_writer)
{
  struct timespec now;
  uint64_t ns;
  char buf[128];
  narrator_fixed fixed;

  { MAPPING
    STRING("//goals")
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
          selector_writer_write(goals_target_direct_selector, builder_writer);
        }
      }
      if(goals_target_transitive_selector) {
        MAPPING
        STRING("target-transitive")
        { LIST
          selector_writer_write(goals_target_transitive_selector, builder_writer);
        }
      }
      if(goals_build) {
        STRING("build");
      }
    }
  }
}

void goals_say(narrator * restrict N)
{
  bacon_writer writer;

  bacon_writer_init(&writer);
  bacon_writer_open(&writer, N);

  goals_describe(&writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
}

void goals_set_event()
{
  event_bacon *msg;
  events_context ectx;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;

  if(!(msg = events_bacon_would(&ectx, BAMIPC_EVENT_GOALS_SET))) {
    return;
  }

  narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    STRING("goals-set")
    { SET
      goals_describe(builder_writer);
    }
  }

  msg->size = fixed.l;
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

  goals_set_event();
}

void goals_stats_build(bool reset, struct bacon_writer * restrict builder_writer)
{

}
