/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <time.h>
#include <stdio.h>
#include <inttypes.h>

#include "xlinux/xstdlib.h"
#include "moria.h"
#include "moria/vertex.h"
#include "bam/ipc.h"
#include "bam/events.h"
#include "bacon/builder.h"
#include "bacon/writer.h"
#include "narrator/fixed.h"
#include "narrator/units.h"
#include "channel.h"
#include "common/attrs.h"
#include "common/hash.h"

#include "system.h"
#include "events.h"
#include "zcat.h"
#include "configset.h"
#include "ruleset.h"
#include "varset.h"
#include "module.h"
#include "stats.h"
#include "run_thread.h"
#include "graph.h"
#include "vertex.h"
#include "params.h"
#include "timeout_thread.h"
#include "fsent.h"
#include "args.h"
#include "formula.h"
#include "rule.h"

#include "times.h"

attrs16 system_state_attrs = (attrs16) {
#undef DEF
#define DEF(x, s, v) + 1
    num : 0
      SYSTEM_STATE_TABLE
  , members : (member16[]) {
#undef DEF
#define DEF(x, s, v) { name : s, value : x, range : 0xff },
SYSTEM_STATE_TABLE
#undef DEF
  }
};

union stats_system system_stats;
moria_vertex *g_system = (moria_vertex[]){{ }};
char g_system_id[40];
uint16_t g_system_id_len;

enum system_state system_state = SYSTEM_STATE_BOOTSTRAP;
bool system_error;

static void __attribute__((constructor)) init()
{
  attrs16_init(&system_state_attrs);
}

void system_state_event()
{
  events_context ctx;
  event_bacon *msg;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;

  if(!(msg = events_bacon_would(&ctx, BAMIPC_EVENT_SYSTEM_STATE_CHANGE))) {
    return;
  }

  narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  STRING(attrs16_name_byvalue(&system_state_attrs, system_state));

  msg->size = fixed.l;
  events_publish(&ctx);
}

void system_state_change(enum system_state state)
{
  if(state == system_state) {
    return;
  }

  switch(state) {
    case SYSTEM_STATE_BOOTSTRAP:
      system_stats.system_state_bootstrap++;
      break;
    case SYSTEM_STATE_CONFIG_SYSTEM_RECONCILE:
      system_stats.system_state_config_reconcile++;
      break;
    case SYSTEM_STATE_WALKER_RECONCILE:
      system_stats.system_state_walker_reconcile++;
      break;
    case SYSTEM_STATE_VAR_SYSTEM_RECONCILE:
      system_stats.system_state_var_reconcile++;
      break;
    case SYSTEM_STATE_MODULE_SYSTEM_RECONCILE:
      system_stats.system_state_module_reconcile++;
      break;
    case SYSTEM_STATE_RULE_SYSTEM_RECONCILE:
      system_stats.system_state_rule_reconcile++;
      break;
    case SYSTEM_STATE_BPE_SYSTEM_RECONCILE:
      system_stats.system_state_bpe_reconcile++;
      break;
    case SYSTEM_STATE_FORMULA_SYSTEM_RECONCILE:
      system_stats.system_state_formula_reconcile++;
      break;
    case SYSTEM_STATE_FILESYSTEM_RECONCILE:
      system_stats.system_state_filesystem_reconcile++;
      break;
    case SYSTEM_STATE_PLUGIN_SYSTEM_RECONCILE:
      system_stats.system_state_plugin_reconcile++;
      break;
    case SYSTEM_STATE_FSENT_RECONCILE:
      system_stats.system_state_fsent_reconcile++;
      break;
    case SYSTEM_STATE_BUILDING:
      system_stats.system_state_building++;
      break;
    case SYSTEM_STATE_OK:
      system_stats.system_state_ok++;
      break;
  }

  printf("%-20s -> %s\n"
    , attrs16_name_byvalue(&system_state_attrs, system_state)
    , attrs16_name_byvalue(&system_state_attrs, state)
  );

  system_state = state;
  system_state_event();
}

event_bacon *system_error_event(events_context *ctx)
{
  event_bacon *msg;

  system_error = true;
  if((msg = events_bacon_would(ctx, BAMIPC_EVENT_SYSTEM_STATE_CHANGE))) {
    return msg;
  }

  /* unowned event for rendering onto */
  ctx->chan = 0;
  ctx->msg = 0;
  ctx->txq = 0;
  return ctx->pb = event_bacon_alloc();
}

void system_setup(void)
{
  moria_vertex_initw(g_system, &g_graph, VERTEX_SYSTEM | VERTEX_OK, MMS("(system)"));
  g_system_id_len = zcatf(g_system_id, sizeof(g_system_id), "%016"PRIx64, hash64(0x3456, MMS("(system)")));
}

void system_stats_build(bool reset, bacon_writer * restrict builder_writer)
{
  system_stats.system_error = system_error;
  system_stats.autorun = run_thread_autorun;

  system_stats.modules = llist_count(&module_list);
  system_stats.configsets = llist_count(&configset_list);
  system_stats.rulesets = llist_count(&ruleset_list);
  system_stats.varsets = llist_count(&varset_list);
  system_stats.rules = llist_count(&rule_list);
  system_stats.fsents = llist_count(&fsent_list);
  system_stats.formulas = llist_count(&formula_list);

  stats_build(&system_stats.record, &descriptor_stats_system, builder_writer);
}

size_t system_id_cat(char * dst, size_t sz)
{
  return zcatw(dst, sz, g_system_id, g_system_id_len);
}

void system_mappings(struct bacon_writer * restrict builder_writer)
{
  struct timespec now;
  uint64_t ns;
  narrator_fixed fixed;
  char buf[128];
  size_t z;

  clock_gettime(CLOCK_MONOTONIC, &now);
  ns = timespec_delta(&g_params.starttime, &now);
  interval_say(ns / NSEC_PER_SEC, narrator_fixed_init(&fixed, buf, sizeof(buf)));
  { MAPPING STRING("uptime") BYTES(fixed.s, fixed.l) }

  ns = timespec_delta(&now, &timeout_thread_deadline);
  interval_say(ns / NSEC_PER_SEC, narrator_fixed_init(&fixed, buf, sizeof(buf)));
  { MAPPING STRING("timeout") BYTES(fixed.s, fixed.l) }

  z = attrs16_zcat(buf, sizeof(buf), &system_state_attrs, system_state);
  { MAPPING STRING("system-state") BYTES(buf, z) }

  { MAPPING STRING("autorun") BOOL(run_thread_autorun) }

  { MAPPING STRING("pid") UINT(g_params.pid) }
  { MAPPING STRING("hash") HEX64(g_args.hash) }
  { MAPPING STRING("basedir") BYTES(g_base_dir, g_base_dir_len) }
  { MAPPING STRING("ipcdir") BYTES(g_params.ipcdir, g_params.ipcdir_len) }
  { MAPPING STRING("homedir") BYTES(g_params.homedir, g_params.homedir_len) }
  { MAPPING STRING("system-config-path") STRING(g_args.system_config_path) }
  { MAPPING STRING("user-config-path") STRING(g_args.user_config_path) }
}

void system_describe(struct bacon_writer * restrict builder_writer)
{
  { MAPPING
    vertex_path_describe(g_system, builder_writer);
    { SET
      system_mappings(builder_writer);
      { MAPPING STRING("stats")
        { SET
          system_stats_build(false, builder_writer);
        }
      }
    }
  }
}

void system_tell(struct channel * restrict channel)
{
  bamipc_message *msg;
  narrator_fixed fixed;
  narrator *N;
  bacon_writer writer;

  msg = channel_produce(channel);
  msg->type = BAMIPC_MSG_RESULT;
  msg->attrs = 0;

  N = narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
  bacon_writer_init(&writer);
  bacon_writer_open(&writer, N);

  system_mappings(&writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  msg->size = fixed.l;
  channel_post(channel, msg);
}
