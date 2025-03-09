/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>
#include <signal.h>

#include "narrator.h"
#include "narrator/fixed.h"
#include "valyria/rbtree.h"
#include "valyria/llist.h"
#include "bacon/builder.h"
#include "bacon/location.h"
#include "bacon/value.h"
#include "moria/foreach.h"
#include "bam/sigutil.h"

#include "channel.h"
#include "config.h"
#include "fsent.h"
#include "system.h"
#include "plugin_system.h"
#include "params.h"
#include "plugin.h"
#include "plugin_thread.h"
#include "vertex.h"
#include "exec_value.h"
#include "namespace.h"
#include "edges.h"
#include "vertices.h"
#include "pattern.h"
#include "fsname.h"
#include "resolve.h"
#include "lookup.h"
#include "meta_edge.h"
#include "edge.h"
#include "shadow.h"
#include "rcu.h"
#include "sysvar.h"
#include "exec.h"
#include "disintegrate.h"

#include "events/events.h"
#include "zcat.h"
#include "times.h"

static exec_builder eb;
static struct exec_render_context eb_ctx;

uint32_t plugin_system_epoch;
llist plugin_invalid_list = LLIST_INITIALIZER(plugin_invalid_list);

uint32_t plugin_system_stdin_size;
uint32_t plugin_system_stdout_size;
uint32_t plugin_system_stderr_size;
uint32_t plugin_system_auxout_size;

#define PLUGIN_BUFFER_SIZE_MIN 4096
#define PLUGIN_BUFFER_SIZE_MAX UINT16_MAX
#define PLUGIN_BUFFER_SIZE_DEFAULT 16384
#define INRANGE(x, min, max) ((x) != 0 && ((x) >= (min) && (x) <= (max)))

static void arg_resolve_error_build(bacon_writer * restrict builder_writer, const lookup_context * restrict ctx)
{
  { MAPPING
    if(ctx->rctx.ref->bound) {
      STRING("unmatched plugin arg lookup");
    } else {
      STRING("unresolved plugin arg lookup");
    }
    { SET
      lookup_error_mappings(builder_writer, ctx);
    }
  }
}

static void env_resolve_error_build(bacon_writer * restrict builder_writer, const lookup_context * restrict ctx)
{
  { MAPPING
    if(ctx->rctx.ref->bound) {
      STRING("unmatched plugin env lookup");
    } else {
      STRING("unresolved plugin env lookup");
    }
    { SET
      lookup_error_mappings(builder_writer, ctx);
    }
  }
}

static void file_resolve_error_build(bacon_writer * restrict builder_writer, const lookup_context * restrict ctx)
{
  { MAPPING
    if(ctx->rctx.ref->bound) {
      STRING("unmatched plugin file lookup");
    } else {
      STRING("unresolved plugin file lookup");
    }
    { SET
      lookup_error_mappings(builder_writer, ctx);
    }
  }
}

static void lookup_connect(lookup_context * restrict ctx, struct plugin * restrict plugin, edge_type type, graph_invalidation_context * restrict invalidation)
{
  meta_edge *me;

  /* edge on the fsname visited */
  if(ctx->hit) {
    me = meta_connect(&ctx->hit->vertex, &plugin->vertex, EDGE_FSNAME_PLUGIN, invalidation);
    me->epoch = plugin_system_epoch;
  }
  if(ctx->miss) {
    me = meta_connect(&ctx->miss->vertex, &plugin->vertex, EDGE_FSNAME_PLUGIN, invalidation);
    me->epoch = plugin_system_epoch;
  }

  /* edge on the target */
  me = meta_connect(ctx->rctx.vertices[0], &plugin->vertex, type, invalidation);
  me->epoch = plugin_system_epoch;
}

static void plugin_lookup_error(const lookup_context * restrict ctx, void (*build)(bacon_writer *, const lookup_context *))
{
  narrator_fixed fixed;
  bacon_writer writer;
  events_context ectx;
  bamipc_message *msg;

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);

  bacon_writer_open(&writer, &fixed.base);
  build(&writer, ctx);
  bacon_writer_close(&writer);

  msg->code = EINVAL;
  msg->len = fixed.l;
  events_publish(&ectx);
}

static void plugin_exec_prepare(
    struct plugin * restrict plugin
  , struct bacon_value * restrict file
  , struct bacon_value * restrict envs
  , struct bacon_value * restrict args
)
{
  exec_builder_reset(&eb);
  exec_render_context_configure(&eb_ctx, &eb, g_root, 0, 0, sysvar_globals, 0);
  eb_ctx.lookups = &plugin->lookups;

  /* file is required and enforced in the parser */
  exec_render_file(&eb_ctx, file);

  /* args */
  if(args) {
    exec_render_args(&eb_ctx, args);
  }

  /* envs */
  if(envs) {
    exec_render_envs(&eb_ctx, envs);
  }
  exec_render_envs(&eb_ctx, sysvar_globals);

  if(plugin->exec) {
    exec_free(plugin->exec);
  }

  exec_builder_build(&eb, 0);
  exec_builder_take(&eb, &plugin->exec);
}

static void plugin_resolve(config_plugin_entry *pe, graph_invalidation_context * restrict invalidation)
{
  exec_value *ev;
  bacon_value *val;
  fsent *fse;
  struct plugin *plugin;
  lookup_context ctx;
  bacon_value *file = 0;
  bacon_value *args = 0;
  bacon_value *envs = 0;

  /* guaranteed to be present by the config parser */
  RUNTIME_ASSERT((int)pe->file.lookup->type == EXEC_VALUE_LOOKUP);
  ev = containerof(pe->file.lookup, typeof(*ev), bacon);

  /* resolve/lookup the plugin file */
  memset(&ctx, 0, sizeof(ctx));
  ctx.rctx.base = g_base;
  ctx.rctx.ref = &ev->lookup.pattern->pattern;
  ctx.loc = &ev->lookup.pattern->pattern.loc;
  ctx.ns_name = ev->lookup.namespace;
  ctx.ns_name_len = ev->lookup.namespace_len;
  if(lookup_resolve(&ctx, ev->lookup.pattern) != 1) {
    plugin_lookup_error(&ctx, file_resolve_error_build);
    return;
  }

  fse = containerof(ctx.rctx.vertices[0], typeof(*fse), vertex);

  /* get/allocate the plugin */
  if(pe->name) {
    plugin = plugin_get(fse, pe->name->s, pe->name->len, invalidation);
  } else {
    plugin = plugin_get(fse, 0, 0, invalidation);
  }

  /* if it resolves and is not invalid, no need to rebuild its exec */
  if(!vertex_invalid_get(&plugin->vertex)) {
    plugin->epoch = plugin_system_epoch;
    return;
  }

  lookup_connect(&ctx, plugin, EDGE_LOOKUP_PLUGIN_FILE, invalidation);
  lookup_store_put(&plugin->lookups, ev, &fse->vertex);
  file = &ev->bacon;

  /* resolve args lookups if any */
  if(pe->args) {
    llist_foreach(&pe->args->list, val, lln) {
      if((int)val->type != EXEC_VALUE_LOOKUP) {
        continue;
      }
      ev = containerof(val, typeof(*ev), bacon);

      memset(&ctx, 0, sizeof(ctx));
      ctx.ns_name = ev->lookup.namespace;
      ctx.ns_name_len = ev->lookup.namespace_len;
      if(lookup_resolve(&ctx, ev->lookup.pattern) != 1) {
        plugin_lookup_error(&ctx, arg_resolve_error_build);
        return;
      }

      lookup_connect(&ctx, plugin, EDGE_LOOKUP, invalidation);
      lookup_store_put(&plugin->lookups, ev, ctx.rctx.vertices[0]);
    }
    args = pe->args;
  }

  /* resolve envs lookups if any */
  if(pe->envs) {
    rbtree_foreach(&pe->envs->set, val, rbn) {
      if((int)val->type != EXEC_VALUE_LOOKUP) {
        continue;
      }
      ev = containerof(val, typeof(*ev), bacon);
      memset(&ctx, 0, sizeof(ctx));
      ctx.rctx.ref = &ev->lookup.pattern->pattern;
      ctx.ns_name = ev->lookup.namespace;
      ctx.ns_name_len = ev->lookup.namespace_len;
      if(lookup_resolve(&ctx, ev->lookup.pattern) != 1) {
        plugin_lookup_error(&ctx, env_resolve_error_build);
        return;
      }

      lookup_connect(&ctx, plugin, EDGE_LOOKUP, invalidation);
      lookup_store_put(&plugin->lookups, ev, ctx.rctx.vertices[0]);
    }
    envs = pe->envs;
  }

  plugin->epoch = plugin_system_epoch;
  plugin_exec_prepare(plugin, file, envs, args);
}

static void duplicate_plugins(bacon_writer *builder_writer, void *ctx)
{
  const config_plugin_entry *pe0 = ctx;

  { MAPPING STRING("duplicate plugin names")
    { SET
      { MAPPING STRING("name")
        BYTES(pe0->name->s, pe0->name->len)
      }
    }
  }
}

void plugin_system_setup(void)
{
  exec_builder_init(&eb);
}

void plugin_system_cleanup(void)
{
  exec_builder_destroy(&eb);
}

void plugin_system_reconfigure(config * restrict cfg, bool dryrun, graph_invalidation_context * restrict invalidation)
{
  config_plugin_entry *pe0, *pe1;
  struct plugin *plugin;
  config_param param;

  if(dryrun) {
    /* sane buffer sizes */
    param.min = PLUGIN_BUFFER_SIZE_MIN;
    param.max = PLUGIN_BUFFER_SIZE_MAX;

    param.config_path = "plugins.stdin-buffer-size";
    param.val = cfg->plugins.stdin_buffer_size.v;
    config_validate(&param);

    param.config_path = "plugins.stdout-buffer-size";
    param.val = cfg->plugins.stdout_buffer_size.v;
    config_validate(&param);

    param.config_path = "plugins.stderr-buffer-size";
    param.val = cfg->plugins.stderr_buffer_size.v;
    config_validate(&param);

    param.config_path = "plugins.auxout-buffer-size";
    param.val = cfg->plugins.auxout_buffer_size.v;
    config_validate(&param);
    if(system_has_error) {
      return;
    }

    /* names among the plugins must be unique */
    llist_foreach(&cfg->plugins.list, pe0, lln) {
      llist_foreach(&cfg->plugins.list, pe1, lln) {
        if(pe0 == pe1) {
          continue;
        }
        if(!pe0->name) {
          continue;
        }
        if(!pe1->name) {
          continue;
        }
        if(memncmp(pe0->name->s, pe0->name->len, pe1->name, pe1->name->len) != 0) {
          continue;
        }

        // WTF
        return system_error(EINVAL, duplicate_plugins, pe0);
      }
    }

    return;
  }

  /* when these settings change, invalidate all plugins */
  if(0
     || plugin_system_stdin_size != cfg->plugins.stdin_buffer_size.u32
     || plugin_system_stdout_size != cfg->plugins.stdout_buffer_size.u32
     || plugin_system_stderr_size != cfg->plugins.stderr_buffer_size.u32
     || plugin_system_auxout_size != cfg->plugins.auxout_buffer_size.u32)
  {
    llist_foreach(&plugin_list, plugin, vertex.owner) {
      vertex_invalidate(&plugin->vertex, invalidation);
    }
  }

  plugin_system_stdin_size = cfg->plugins.stdin_buffer_size.u32;
  plugin_system_stdout_size = cfg->plugins.stdout_buffer_size.u32;
  plugin_system_stderr_size = cfg->plugins.stderr_buffer_size.u32;
  plugin_system_auxout_size = cfg->plugins.auxout_buffer_size.u32;
}

void plugin_system_reconcile(graph_invalidation_context * restrict invalidation, struct rcu_thread * restrict rcu)
{
  sigset_t sigs;
  siginfo_t siginfo;
  struct plugin *plugin;
  bool any;
  config_plugin_entry *pe;
  struct edges edges;
  struct vertices unlinking;
  struct vertices releasing;
  moria_foreach_state st;
  moria_edge *edge;
  meta_edge *meta;
  struct timespec interval;
  llist *T;

  interval.tv_sec = 0;
  interval.tv_nsec = MSEC_AS_NSEC(100);

  plugin_system_epoch++;
  rcu_quiesce(rcu);

  /* resolve all plugins from config -- raises an error if any plugin doesnt resolve
   *  this operation refreshes the epoch on,
   *   all plugins
   *   all lookup edges for those plugins
   */
  llist_foreach(&config_active->plugins.list, pe, lln) {
    if(!system_has_error) {
      plugin_resolve(pe, invalidation); // refreshes plugin->epoch
    }
  }

  /* cleanup any edges not refreshed */
  vertices_init(&unlinking);
  vertices_init(&releasing);
  edges_init(&edges);
  llist_foreach(&plugin_list, plugin, vertex.owner) {
    moria_foreach_edge_down(&st, &plugin->vertex, edge) {
      switch(edge_type_get(edge)) {
        case EDGE_LOOKUP:
        case EDGE_FSNAME_PLUGIN:
          break;
        default:
          continue;
      }
      meta = containerof(edge, typeof(*meta), edge);
      if(meta->epoch == plugin_system_epoch) {
        continue;
      }
      edges_push(&edges, &meta->edge);
    }
  }

  graph_disintegrate(&edges, &unlinking, &releasing, invalidation);

  /* only proceed if all plugins resolved */
  if(system_has_error) {
    return;
  }

  // signals handled on this thread
  sigemptyset(&sigs);
  sigaddset(&sigs, SIGUSR1);

  /* kill any plugin that either 1/ was not refreshed, or 2/ has been invalidated
   * and wait for them to exit */
  any = true;
  while(any) {
    any = false;
    llist_foreach(&plugin_list, plugin, vertex.owner) {
      if(plugin->thread_state == PLUGIN_THREAD_STARTING) {
        continue;     /* plugin not started yet */
      } else if(plugin->thread_state == PLUGIN_THREAD_EXITED) {
        continue;     /* plugin crashed */
      } if(plugin->epoch != plugin_system_epoch) {
        /* plugin was not refreshed */
      } else if(vertex_invalid_get(&plugin->vertex)) {
        /* plugin has been invalidated */
      } else {
        continue;
      }

      any = true;
      plugin_thread_kill(plugin->thread);
    }

    if(!any) {
      break;
    }

    rcu_quiesce(rcu);
    sigutil_timedwait(&sigs, &siginfo, &interval);
  }

  /* teardown plugins not refreshed and no longer needed */
  llist_foreach_safe(&plugin_list, plugin, vertex.owner, T) {
    if(plugin->epoch == plugin_system_epoch) {
      continue;
    }

    graph_demolish(&plugin->vertex);
  }

  /* start to quiesce all plugins */
  any = false;
  llist_foreach(&plugin_list, plugin, vertex.owner) {
    if(plugin->thread_state == PLUGIN_THREAD_EXITED) {
      plugin_thread_launch(plugin);             /* launch the plugin thread */
    } else if(plugin->thread_state == PLUGIN_THREAD_STARTING) {
      plugin_thread_launch(plugin);             /* launch the plugin thread */
    } else {
      plugin_thread_unready(plugin->thread);    /* ask this plugin to quiesce */
    }

    any = true;
  }

  /* wait until plugins have quiesced */
  while(any) {
    any = false;
    llist_foreach(&plugin_list, plugin, vertex.owner) {
      if(plugin->thread_state == PLUGIN_THREAD_EXITED) {
        continue;     /* plugin crashed */
      } else if(plugin->thread_state != PLUGIN_THREAD_READY_REPORTED) {
        any = true;
        break;
      }
    }

    rcu_quiesce(rcu);
    sigutil_timedwait(&sigs, &siginfo, &interval);
  }
}
