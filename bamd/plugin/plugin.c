/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <sys/wait.h>

#include "valyria/llist.h"
#include "xlinux/xstdlib.h"
#include "moria/foreach.h"
#include "bam/events.h"
#include "moria/connect.h"
#include "bacon/builder.h"
#include "narrator/fixed.h"
#include "narrator/units.h"

#include "plugin.h"
#include "vertex/probes.h"
#include "entlist.h"
#include "edge.h"
#include "handler.h"
#include "special_edge.h"
#include "plugin_thread.h"
#include "plugin_process.h"
#include "times.h"

llist plugin_list = LLIST_INITIALIZER(plugin_list);                 // active
static llist plugin_freelist = LLIST_INITIALIZER(plugin_freelist);  // free

static void plugin_dispose(struct plugin * restrict plugin)
{
  llist_delete(&plugin->vertex.owner);
  llist_append(&plugin_freelist, &plugin->vertex.owner);

  plugin_process_free(plugin->process);
}

void plugin_release(struct plugin * restrict plugin, graph_invalidation_context * restrict invalidation)
{
  moria_vertex *v;
  moria_edge *edge;
  moria_foreach_state st;
  entlist edges, *eln;
  special_edge *se;

  /* proactive edge disconnect */
  v = &plugin->vertex;
  entlist_init(&edges);
  moria_foreach_edge(&st, v, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_SPECIAL:
        entlist_push(&edges, edge);
        break;
      default:
        break;
    }
  }

  entlist_foreach(&edges, edge, eln) {
    se = containerof(edge, special_edge, edge);
    special_disconnecting(se, invalidation);
    moria_edge_disconnect(&g_graph, edge);
    special_edge_release(se);
  }
  entlist_destroy(&edges);

  /* should be fully excised from the graph previously */
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  vertex_deleted_probe(&plugin->vertex);
  plugin_dispose(plugin);
}

void plugin_cleanup()
{
  plugin *v;
  llist *T;

  llist_foreach_safe(&plugin_list, v, vertex.owner, T) {
    plugin_dispose(v);
  }

  llist_foreach_safe(&plugin_freelist, v, vertex.owner, T) {
    free(v);
  }
}

void plugin_alloc(plugin ** restrict vp)
{
  struct plugin *plugin;

  if((plugin = llist_shift(&plugin_freelist, typeof(*plugin), vertex.owner)) == 0) {
    xmalloc(&plugin, sizeof(*plugin));
  } else {
    memset(plugin, 0, sizeof(*plugin));
  }

  moria_vertex_init(&plugin->vertex, &g_graph, VERTEX_PLUGIN | VERTEX_OK);

  llist_prepend(&plugin_list, &plugin->vertex.owner);

  vertex_protect_set(&plugin->vertex, true);
  vertex_alloc_probe(&plugin->vertex);
  *vp = plugin;
}

static void elapsed_describe(const struct timespec * restrict start, const struct timespec * restrict end, bacon_writer * restrict builder_writer)
{
  uint64_t ns;
  char buf[256];
  narrator_fixed fixed;

  ns = timespec_delta(start, end);

  narrator_fixed_init(&fixed, buf, sizeof(buf));
  interval_say_ns(ns, &fixed.base);

  BYTES(buf, fixed.l);
}

void plugin_stats_build(struct plugin * restrict plugin, bool reset, bacon_writer * restrict builder_writer)
{
  stats_build(&plugin->stats.record, &descriptor_stats_vertex, builder_writer);
  stats_build(&plugin->plugin_stats.record, &descriptor_stats_plugin, builder_writer);
}

void plugin_describe(const struct plugin * restrict plugin, bacon_writer * restrict builder_writer)
{
  struct timespec now;
  int exit;
  int sig;
  bool core;

  { MAPPING
    vertex_path_describe(&plugin->vertex, builder_writer);
    { SET
      {
        vertex_mappings(&plugin->vertex, builder_writer);
        { MAPPING STRING("self")
          BYTES(plugin->self_node_abspath, plugin->self_node_abspath_len)
        }
        if(plugin->thread) {
          MAPPING STRING("thread")
          { SET
            { MAPPING STRING("tid") UINT(plugin->thread->tid) }

            if(plugin->thread->handler) {
              MAPPING
              STRING("events")
              if(plugin->thread->handler->event_mask) {
                attrs32_describe(plugin->thread->handler->event_mask, &bamipc_event_mask_attrs, builder_writer);
              } else {
                STRING("-none-")
              }
            }
          }
        }
        if(plugin->process) {
          MAPPING STRING("process")
          { SET
            { MAPPING STRING("pid") UINT(plugin->process->pid) }
            { MAPPING STRING("running") BOOL(plugin->process->running) }
            { MAPPING
              STRING("uptime")
              if(plugin->process->running) {
                clock_gettime(CLOCK_MONOTONIC, &now);
                elapsed_describe(&plugin->process->started_at, &now, builder_writer);
              } else {
                elapsed_describe(&plugin->process->started_at, &plugin->process->stopped_at, builder_writer);
              }
            }
            { MAPPING STRING("out-total") UINT(plugin->process->out_total) }
            { MAPPING STRING("err-total") UINT(plugin->process->err_total) }
            { MAPPING STRING("aux-total") UINT(plugin->process->aux_total) }
            if(plugin->process->out_total) {
              MAPPING STRING("stdout") BYTES(plugin->process->out, plugin->process->out_len)
            }
            if(plugin->process->err_total) {
              MAPPING STRING("stderr") BYTES(plugin->process->err, plugin->process->err_len)
            }
            if(!plugin->process->running) {
              exit = 0;
              sig = 0;
              core = false;
              if(WIFEXITED(plugin->process->status)) {
                exit = WEXITSTATUS(plugin->process->status);
              } else if(WIFSIGNALED(plugin->process->status)) {
                sig = WTERMSIG(plugin->process->status);
                core = WCOREDUMP(plugin->process->status);
              }
              { MAPPING STRING("status") UINT(plugin->process->status) }
              { MAPPING STRING("exit") UINT(exit) }
              { MAPPING STRING("signal") UINT(sig) }
              { MAPPING STRING("core") BOOL(core) }
            }
          }
        }
        { MAPPING STRING("stats")
          { SET
            plugin_stats_build((struct plugin *)plugin, false, builder_writer);
          }
        }
      }
    }
  }
}

size_t plugin_path_cat(void * restrict dst, size_t sz, const struct plugin * restrict plugin)
{
  return zcatf(dst, sz, "//plugins/%.*s", plugin->id_len, plugin->id);
}

void plugin_file_stats_build(fsent * restrict fse, bool reset, bacon_writer * restrict builder_writer)
{
  fsent_stats_build(fse, reset, builder_writer);
}

void plugin_file_describe(const fsent * restrict fse, struct bacon_writer * restrict builder_writer)
{
  { MAPPING
    bacon_writer_push_string(builder_writer);
    vertex_path_say(&fse->vertex, builder_writer->N);
    bacon_writer_pop_string(builder_writer);
    { SET
      fsent_mappings(fse, builder_writer);
      { MAPPING STRING("plugin")
        LOADF("//plugins/%.*s",  fse->self_plugin->id_len, fse->self_plugin->id)
      }
    }
  }
}

void plugin_identity_say(const struct plugin * restrict plugin, struct narrator * restrict N)
{
  attrs32_say(plugin->vertex.attrs, graph_vertex_attrs, N);
  narrator_sayf(N, " id %.*s path %s", plugin->id_len, plugin->id, plugin->self_node_abspath);
}

void plugin_file_identity_say(const struct fsent * restrict fse, struct narrator * restrict N)
{
  attrs32_say(fse->vertex.attrs, graph_vertex_attrs, N);
  narrator_sayf(N, " id %.*s", fse->self_plugin->id_len, fse->self_plugin->id);
}
