/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <sys/wait.h>

#include "valyria/llist.h"
#include "xlinux/xstdlib.h"
#include "bam/events.h"
#include "moria/foreach.h"
#include "moria/connect.h"
#include "moria/edge.h"
#include "bacon/parser.h"
#include "bacon/builder.h"
#include "bacon/value.h"
#include "bacon/make.h"
#include "narrator/fixed.h"
#include "narrator/units.h"

#include "plugin.h"
#include "vertex/probes.h"
#include "edges.h"
#include "vertices.h"
#include "edge.h"
#include "request_handler.h"
#include "special_edge.h"
#include "plugin_thread.h"
#include "plugin_process.h"
#include "times.h"
#include "params.h"
#include "args.h"
#include "shadow.h"
#include "fsedge.h"
#include "detail.h"
#include "identity.h"
#include "exec.h"
#include "disintegrate.h"

llist plugin_list = LLIST_INITIALIZER(plugin_list);                 // active
static llist plugin_freelist = LLIST_INITIALIZER(plugin_freelist);  // free

attrs32 plugin_thread_state_attrs = (attrs32){
#undef DEF
#define DEF(x, s, y) + 1
		num : 0
			PLUGIN_THREAD_STATE_TABLE
	, members : (member32[]) {
#undef DEF
#define DEF(x, s, y) { name : s, value : UINT32_C(y), range : UINT32_C(0xff) },
PLUGIN_THREAD_STATE_TABLE
	}
};

static void __attribute__((constructor)) init()
{
	attrs32_init(&plugin_thread_state_attrs);
}

static void plugin_dispose(struct plugin * restrict plugin)
{
  lookup_store_destroy(&plugin->lookups);
  exec_free(plugin->exec);

  llist_delete(&plugin->vertex.owner);
  llist_append(&plugin_freelist, &plugin->vertex.owner);
  ASAN_TAINT(plugin);
}

static void compute_id(struct identity_ctx * restrict id, const fsent * restrict self, const char * restrict name, uint16_t name_len)
{
  identity_type(id, VERTEX_PLUGIN);
  identity_add(id, " ", 1);
  fsent_id_path_compute(self, id);
  identity_add(id, "\n", 1);
  identity_name(id, VERTEX_PLUGIN, name, name_len);
}

void plugin_release(struct plugin * restrict plugin)
{
  moria_vertex *v;

  v = &plugin->vertex;

  /* must have been fully excised from the graph */
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  plugin_dispose(plugin);
}

void plugin_setup()
{

}

void plugin_cleanup()
{
  plugin *v;

  /* this is where active plugins are cleaned up */
  while((v = llist_shift(&plugin_list, typeof(*v), vertex.owner))) {
    graph_demolish(&v->vertex);
  }

  while((v = llist_shift(&plugin_freelist, typeof(*v), vertex.owner))) {
    ASAN_CLEAN(v);
    plugin_thread_release(v->thread);
    plugin_process_free(v->process);

    free(v);
  }
}

static plugin *plugin_alloc(
    fsent * restrict self
  , const char *id
  , uint16_t id_len
  , const char *name
  , uint16_t name_len
  , graph_invalidation_context * restrict invalidation
)
{
  struct plugin *plugin;
  struct detail *detail;

  if((plugin = llist_shift(&plugin_freelist, typeof(*plugin), vertex.owner)) == 0) {
    ASAN_CLEAN(plugin);
    xmalloc(&plugin, sizeof(*plugin));
  } else {
    memset(plugin, 0, sizeof(*plugin));
  }

  moria_vertex_init(&plugin->vertex, &g_graph, VERTEX_PLUGIN | VERTEX_OK);//, id, id_len);
  lookup_store_init(&plugin->lookups);

  plugin->id_len = zcatw(plugin->id, sizeof(plugin->id), id, id_len);
  plugin->vertex.label = plugin->id;
  plugin->vertex.label_len = plugin->id_len;
  plugin->name_len = zcatw(plugin->name, sizeof(plugin->name), name, name_len);
  plugin->self_node = self;

  llist_prepend(&plugin_list, &plugin->vertex.owner);

  vertex_alloc_probe(&plugin->vertex);

  /* build out the shadow dir */
  shadow_create(g_shadow_plugins, &plugin->shadow, plugin->id, plugin->id_len, VERTEX_SHADOW_DIR, invalidation);

  /* enable traversal via specials graph */
  special_connect(&plugin->shadow->vertex, &plugin->vertex, EDGE_SPECIAL, invalidation);

  /* other nodes */
  detail = detail_alloc(&plugin->vertex, VERTEX_PLUGIN_STDIN);
  shadow_mkref(plugin->shadow, MMS("stdin"), &detail->vertex);
  detail = detail_alloc(&plugin->vertex, VERTEX_PLUGIN_STDOUT);
  shadow_mkref(plugin->shadow, MMS("stdout"), &detail->vertex);
  detail = detail_alloc(&plugin->vertex, VERTEX_PLUGIN_STDERR);
  shadow_mkref(plugin->shadow, MMS("stderr"), &detail->vertex);
  detail = detail_alloc(&plugin->vertex, VERTEX_PLUGIN_AUXOUT);
  shadow_mkref(plugin->shadow, MMS("auxout"), &detail->vertex);

  if(name_len) {
    shadow_mkref(g_shadow_plugins_byname, plugin->name, plugin->name_len, &plugin->vertex);
  }

  return plugin;
}

plugin *plugin_get(fsent * restrict fse, const char * restrict name, uint16_t name_len, graph_invalidation_context * restrict invalidation)
{
  char id[40];
  uint16_t id_len;
  char path[PATH_MAX];
  size_t path_len;
  moria_vertex *v;
  struct plugin *plugin;
  moria_edge *edge;
  moria_foreach_state st;
  identity_ctx ctx;

  identity_init(&ctx);
  compute_id(&ctx, fse, name, name_len);
  id_len = identity_mk(&ctx, id, sizeof(id));

  /* hash : plugin file path + plugin name */
  path_len = fsent_absolute_path_cat(path, sizeof(path), fse);

  if((v = moria_vertex_downw(&g_shadow_plugins->vertex, id, id_len))) {
    /* upwards from the plugin shadow dir -> the plugin */
    moria_foreach_edge_up(&st, v, edge) {
      if(edge_type_get(edge) == EDGE_SPECIAL) {
        break;
      }
    }
    RUNTIME_ASSERT(edge);
    plugin = containerof(edge->A, typeof(*plugin), vertex);
  } else {
    plugin = plugin_alloc(fse, id, id_len, name, name_len, invalidation);
  }

  /* there is no fse -> plugin special edge, because multiple plugins can be instantiated from one file */
  plugin->self_node_abspath_len = zcatw(plugin->self_node_abspath, sizeof(plugin->self_node_abspath), path, path_len);

  return plugin;
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
  int x;

  { MAPPING
    vertex_path_describe(&plugin->vertex, builder_writer);
    { SET
      {
        vertex_mappings(&plugin->vertex, builder_writer);
        { MAPPING STRING("name")
          BYTES(plugin->name, plugin->name_len)
        }
        { MAPPING STRING("self")
          BYTES(plugin->self_node_abspath, plugin->self_node_abspath_len)
        }
        if(plugin->thread) {
          MAPPING STRING("thread")
          { SET
            { MAPPING STRING("tid") UINT(plugin->thread->tid) }
            {
              MAPPING STRING("events")
              if(plugin->thread->event_mask) {
                attrs32_describe(plugin->thread->event_mask, &bamipc_event_mask_attrs, builder_writer);
              } else {
                STRING("-none-")
              }
            }
          }
        }
        { MAPPING STRING("exec")
          { SET
            if(plugin->exec) {
              { MAPPING STRING("file")
                BYTES(plugin->exec->file_path, plugin->exec->file_path_len);
              }
              { MAPPING STRING("args")
                { LIST
                  for(x = 0; x < plugin->exec->args_len; x++) {
                    STRING(plugin->exec->args[x]);
                  }
                }
              }
              { MAPPING STRING("envs")
                { LIST
                  for(x = 0; x < plugin->exec->envs_len; x++) {
                    STRING(plugin->exec->envs[x]);
                  }
                }
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
            { MAPPING STRING("stdin")
              { SET
                { MAPPING STRING("size")
                  UINT(plugin->process->in_size)
                }
                { MAPPING STRING("total")
                  UINT(plugin->process->in_total)
                }
              }
            }
            { MAPPING STRING("stdout")
              { SET
                { MAPPING STRING("size")
                  UINT(plugin->process->out_size)
                }
                { MAPPING STRING("total")
                  UINT(plugin->process->out_total)
                }
              }
            }
            { MAPPING STRING("stderr")
              { SET
                { MAPPING STRING("size")
                  UINT(plugin->process->err_size)
                }
                { MAPPING STRING("total")
                  UINT(plugin->process->err_total)
                }
              }
            }
            { MAPPING STRING("auxout")
              { SET
                { MAPPING STRING("size")
                  UINT(plugin->process->auxout_size)
                }
                { MAPPING STRING("total")
                  UINT(plugin->process->auxout_total)
                }
              }
            }
            { MAPPING STRING("last")
              request_describe(&plugin->process->last_req, builder_writer);
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
      vertex_relations_describe(&plugin->vertex, builder_writer);
    }
  }
}

size_t plugin_path_cat(void * restrict dst, size_t sz, const struct plugin * restrict plugin)
{
  return zcatf(dst, sz, "//bm/plugins/%.*s", plugin->id_len, plugin->id);
}

void plugin_file_stats_build(fsent * restrict fse, bool reset, bacon_writer * restrict builder_writer)
{
  fsent_stats_build(fse, reset, builder_writer);
}

void plugin_file_describe(const fsent * restrict fse, struct bacon_writer * restrict builder_writer)
{
  edges list, *eln;
  moria_edge *e;

  { MAPPING
    vertex_path_describe(&fse->vertex, builder_writer);
    { SET
      vertex_mappings(&fse->vertex, builder_writer);
      if(edges_gather_up(&fse->vertex, EDGE_SPECIAL, &list)) {
        MAPPING STRING("plugins")
        { SET
          edges_foreach(&list, e, eln) {
            vertex_path_describe(e->A, builder_writer);
          }
        }
        edges_destroy(&list);
      }
      { MAPPING STRING("stats")
        { SET
          plugin_file_stats_build((fsent *)fse, false, builder_writer);
        }
      }
    }
  }
}

void plugin_identity_describe(const struct plugin * restrict plugin, struct bacon_writer * restrict builder_writer)
{
  attrs32_describe(plugin->vertex.attrs, graph_vertex_attrs, builder_writer);
  { MAPPING STRING("id")
    BYTES(plugin->id, plugin->id_len);
  }
  { MAPPING STRING("name")
    BYTES(plugin->name, plugin->name_len);
  }
  { MAPPING STRING("path")
    STRING(plugin->self_node_abspath);
  }
}

void plugin_file_identity_describe(const struct fsent * restrict fse, struct bacon_writer * restrict builder_writer)
{
  attrs32_describe(fse->vertex.attrs, graph_vertex_attrs, builder_writer);
  STRING("path")
  fsent_absolute_path_describe(fse, builder_writer);
}

void plugin_unlink(struct plugin * restrict plugin, struct graph_disintegrate_context * restrict ctx)
{
  moria_foreach_state st;
  moria_edge *edge;

  /* when an plugin is unlinked, also unlink (upwards) its bpes and bind targets */
  moria_foreach_edge(&st, &plugin->vertex, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_LOOKUP:
      case EDGE_LOOKUP_PLUGIN_FILE:
      case EDGE_NS_PLUGIN:
      case EDGE_FSNAME_PLUGIN:
      case EDGE_SPECIAL:
        edges_push(ctx->edges, edge);
      default:
        break;
    }
  }
}

void plugin_id(const struct plugin * restrict plugin, const char ** restrict id, uint16_t * restrict idl)
{
  *id = plugin->id;
  *idl = plugin->id_len;
}

void plugin_compute_id(const struct plugin * restrict v, struct identity_ctx * restrict id)
{
  compute_id(id, v->self_node, v->name, v->name_len);
}

#if CONFIG_DEVEL
void plugins_show(void)
{
  plugin *fml;
  int x;

  printf("plugins, live : %zu free : %zu\n", llist_count(&plugin_list), llist_count(&plugin_freelist));
  x = 0;
  llist_foreach(&plugin_list, fml, vertex.owner) {
    printf("%3u ", x++);
    vertex_identity_show(&fml->vertex);
    vertex_edges_show(&fml->vertex);
  }
}
#endif
