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

#include "system.h"
#include "events/events.h"
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
#include "namespace.h"
#include "fsname.h"
#include "bpe.h"
#include "rma.h"
#include "plugin.h"
#include "fstree.h"
#include "request_handler.h"
#include "build_record.h"
#include "formula_record.h"
#include "client.h"
#include "shadow.h"
#include "detail.h"
#include "subsys.h"
#include "fsedge.h"
#include "module_edge.h"
#include "meta_edge.h"
#include "special_edge.h"
#include "dependency.h"
#include "ref_edge.h"
#include "ns_edge.h"
#include "dir_rma_edge.h"

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

static bamipc_message system_error_msg;
static char system_error_msg_text[16384];

union stats_system_objects system_stats_objects;
union stats_system_states system_stats_states;
union stats_system_events system_stats_events;

subsys sys_system;

enum system_state system_state = SYSTEM_STATE_BOOTSTRAP;
bool system_has_error;

static void __attribute__((constructor)) init()
{
  attrs16_init(&system_state_attrs);
  system_error_msg.text = system_error_msg_text;
  system_error_msg.size = sizeof(system_error_msg_text);
}

static void system_stats_collate(void)
{
  system_stats_states.system_error = system_has_error;
  system_stats_states.autorun = run_thread_autorun;

  system_stats_objects.modules = llist_count(&module_list);
  system_stats_objects.configsets = llist_count(&configset_list);
  system_stats_objects.rulesets = llist_count(&ruleset_list);
  system_stats_objects.varsets = llist_count(&varset_list);
  system_stats_objects.rules = llist_count(&rule_list);
  system_stats_objects.fsents = llist_count(&fsent_list);
  system_stats_objects.formulas = llist_count(&formula_list);
  system_stats_objects.namespaces = llist_count(&namespace_list);
  system_stats_objects.fsnames = llist_count(&fsname_list);
  system_stats_objects.bpes = llist_count(&bpe_list);
  system_stats_objects.rmas = llist_count(&rma_list);
  system_stats_objects.plugins = llist_count(&plugin_list);
  system_stats_objects.fstrees = llist_count(&fstree_list);
  system_stats_objects.clients = llist_count(&client_list);
  system_stats_objects.build_records = llist_count(&build_record_list);
  system_stats_objects.formula_records = llist_count(&formula_record_list);
  system_stats_objects.details = llist_count(&detail_list);

  system_stats_objects.fsedges = llist_count(&fsedge_list);
  system_stats_objects.dir_rma_edges = llist_count(&drm_list);
  system_stats_objects.module_edges = llist_count(&module_edge_list);
  system_stats_objects.meta_edges = llist_count(&meta_edge_list);
  system_stats_objects.special_edges = llist_count(&special_edge_list);
  system_stats_objects.dependency_edges = llist_count(&dependency_list);
  system_stats_objects.ref_edges = llist_count(&ref_edge_list);
  system_stats_objects.ns_edges = llist_count(&ns_edge_list);
}

void system_state_event(request_handler * restrict handler)
{
  events_context ctx;
  bamipc_message *msg;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;

  if(!(msg = events_bacon_would(&ctx, BAMIPC_EVENT_SYSTEM_STATE_CHANGE))) {
    return;
  }

  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  STRING(attrs16_name_byvalue(&system_state_attrs, system_state));

  msg->len = fixed.l;
  events_publish(&ctx);
}

void system_state_change(enum system_state state)
{
  if(state == system_state) {
    return;
  }

  switch(state) {
    case SYSTEM_STATE_BOOTSTRAP:
      system_stats_events.system_state_bootstrap++;
      break;
    case SYSTEM_STATE_CONFIG_SYSTEM_RECONCILE:
      system_stats_events.system_state_config_reconcile++;
      break;
    case SYSTEM_STATE_WALKER_RECONCILE:
      system_stats_events.system_state_walker_reconcile++;
      break;
    case SYSTEM_STATE_NAMESPACE_RECONCILE:
      system_stats_events.system_state_namespace_reconcile++;
      break;
    case SYSTEM_STATE_VAR_SYSTEM_RECONCILE:
      system_stats_events.system_state_var_reconcile++;
      break;
    case SYSTEM_STATE_MODULE_SYSTEM_RECONCILE:
      system_stats_events.system_state_module_reconcile++;
      break;
    case SYSTEM_STATE_RULE_SYSTEM_RECONCILE:
      system_stats_events.system_state_rule_reconcile++;
      break;
    case SYSTEM_STATE_BPE_SYSTEM_RECONCILE:
      system_stats_events.system_state_bpe_reconcile++;
      break;
    case SYSTEM_STATE_FORMULA_SYSTEM_RECONCILE:
      system_stats_events.system_state_formula_reconcile++;
      break;
    case SYSTEM_STATE_FSTREE_RECONCILE:
      system_stats_events.system_state_fstree_reconcile++;
      break;
    case SYSTEM_STATE_FILESYSTEM_RECONCILE:
      system_stats_events.system_state_filesystem_reconcile++;
      break;
    case SYSTEM_STATE_PLUGIN_SYSTEM_RECONCILE:
      system_stats_events.system_state_plugin_reconcile++;
      break;
    case SYSTEM_STATE_FSENT_RECONCILE:
      system_stats_events.system_state_fsent_reconcile++;
      break;
    case SYSTEM_STATE_READY:
      system_stats_events.system_state_ready++;
      break;
    case SYSTEM_STATE_BUILDING:
      system_stats_events.system_state_building++;
      break;
    case SYSTEM_STATE_OK:
      system_stats_events.system_state_ok++;
      break;
    case SYSTEM_STATE_ERR:
      system_stats_events.system_state_err++;
      break;
  }

  printf("%-20s -> %s\n"
    , attrs16_name_byvalue(&system_state_attrs, system_state)
    , attrs16_name_byvalue(&system_state_attrs, state)
  );

  system_state = state;
  system_state_event(0);
}

bamipc_message *system_error_event(events_context *ctx)
{
  bamipc_message *msg;

  system_has_error = true;
  if((msg = events_bacon_would(ctx, BAMIPC_EVENT_SYSTEM_STATE_CHANGE))) {
    msg->flags |= EVENT_SYSTEM_ERROR;
    return msg;
  }

  /* unowned event for rendering onto */
  ctx->channel = 0;
  ctx->plugin = 0;
  ctx->msg = &system_error_msg;
  system_error_msg.len = 0;
  ctx->msg->flags |= EVENT_SYSTEM_ERROR;
  ctx->msg->type = BAMIPC_MSG_EVENTS;
  ctx->msg->evtype = BAMIPC_EVENT_SYSTEM_STATE_CHANGE;
  return ctx->msg;
}

void system_error(int code, void (*build)(bacon_writer * restrict builder_writer, void *ctx), const void *ctx)
{
  events_context ectx;
  bamipc_message *msg;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  build(builder_writer, (void*)ctx);

  bacon_writer_close(&writer);

  msg->code = code;
  msg->len = fixed.l;
  events_publish(&ectx);
}

void system_setup(void)
{
  subsys_init(&sys_system, VERTEX_SYS_SYSTEM);
  shadow_mkref(g_shadow_bm, sys_system.vertex.label, sys_system.vertex.label_len, &sys_system.vertex);
}

void system_stats_show_fd(int fd)
{
  struct descriptor_field *member;
  int x;

  system_stats_collate();

  for(x = 0; x < descriptor_stats_system_objects.members_len; x++)
  {
    member = descriptor_stats_system_objects.members[x];
    switch(member->size) {
      case 1:
        dprintf(fd, "%-35.*s %hhu\n", member->name_len, member->name, system_stats_objects.record.u8[member->offset / 1]);
        break;
      case 2:
        dprintf(fd, "%-35.*s %hu\n", member->name_len, member->name, system_stats_objects.record.u16[member->offset / 2]);
        break;
      case 4:
        dprintf(fd, "%-35.*s %"PRIu32"\n", member->name_len, member->name, system_stats_objects.record.u32[member->offset / 4]);
        break;
      case 8:
        dprintf(fd, "%-35.*s %"PRIu64"u\n", member->name_len, member->name, system_stats_objects.record.u64[member->offset / 8]);
        break;
    }
  }
}

void system_stats_show()
{
  system_stats_show_fd(1);
}


void system_sys_stats_build(const moria_vertex * restrict v, bool reset, bacon_writer * restrict builder_writer)
{
  system_stats_collate();
  stats_build(&system_stats_objects.record, &descriptor_stats_system_objects, builder_writer);
  stats_build(&system_stats_states.record, &descriptor_stats_system_states, builder_writer);
  stats_build(&system_stats_events.record, &descriptor_stats_system_events, builder_writer);
}

void system_sys_mappings(const struct moria_vertex * restrict v, struct bacon_writer * restrict builder_writer)
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
  { MAPPING STRING("bamd_id") BYTES(g_args.bamd_id, g_args.bamd_id_len) }
  { MAPPING STRING("basedir") BYTES(g_basedir, g_basedir_len) }
  { MAPPING STRING("ipcdir") BYTES(g_params.ipcdir, g_params.ipcdir_len) }
  { MAPPING STRING("homedir") BYTES(g_params.homedir, g_params.homedir_len) }
  { MAPPING STRING("system-config-path") STRING(g_args.system_config_path) }
  { MAPPING STRING("user-config-path") STRING(g_args.user_config_path) }
}

void system_sys_describe(const moria_vertex * restrict v, struct bacon_writer * restrict builder_writer)
{
  { MAPPING
    vertex_path_describe(v, builder_writer);
    { SET
      system_sys_mappings(v, builder_writer);
      { MAPPING STRING("stats")
        { SET
          system_sys_stats_build(v, false, builder_writer);
        }
      }
      vertex_relations_describe(v, builder_writer);
    }
  }
}

void system_sys_tell(const moria_vertex * restrict v, struct request_handler * restrict handler)
{
  bamipc_message *msg;
  narrator_fixed fixed;
  narrator *N;
  bacon_writer writer;

  msg = request_handler_tx(handler, BAMIPC_MSG_RESULT, BAMIPC_MSG_BACON, 0);

  N = narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  bacon_writer_open(&writer, N);

  system_sys_mappings(v, &writer);

  bacon_writer_close(&writer);
  bacon_writer_destroy(&writer);
  msg->len = fixed.l;
  request_handler_post(handler, msg);
}
