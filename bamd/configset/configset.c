/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <unistd.h>
#include <errno.h>

#include "types.h"

#include "xlinux/xstdlib.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "valyria/hashtable.h"
#include "valyria/llist.h"
#include "bacon/writer.h"
#include "bacon/builder.h"
#include "common/snarf.h"
#include "common/attrs.h"
#include "moria/foreach.h"
#include "moria/connect.h"

#include "configset.h"
#include "configset_parser.h"
#include "config.h"
#include "filesystem.h"
#include "build.h"
#include "params.h"
#include "fsent.h"
#include "vertex.h"
#include "walker.h"
#include "args.h"
#include "zcat.h"
#include "channel.h"
#include "pattern.h"
#include "stats.h"
#include "system.h"
#include "events/events.h"
#include "reconcile.h"
#include "vertex/probes.h"
#include "edges.h"
#include "vertices.h"
#include "edge.h"
#include "special_edge.h"
#include "fsedge.h"

#include "macros.h"
#include "marshal.h"

/* active list is maintained in precendence order, including system and user config files */
llist configset_list = LLIST_INITIALIZER(configset_list);                 // active
static llist configset_freelist = LLIST_INITIALIZER(configset_freelist);  // free

/* these config nodes always exist */
fsent *system_configset_node;
fsent *user_configset_node;

static void configset_parse_error(configset * restrict cfg, configset_parser * restrict parser)
{
  bamipc_message *msg;
  events_context ectx;
  bacon_writer writer, *builder_writer;
  narrator_fixed fixed;

  msg = system_error_event(&ectx);
  narrator_fixed_init(&fixed, msg->text, msg->size);
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  MAPPING STRING("configset error")
  { SET
    { MAPPING STRING("error")
      BYTES(parser->error, parser->error_len);
    }
  }
  bacon_writer_close(&writer);

  msg->code = EINVAL;
  msg->len = fixed.l;
  events_publish(&ectx);
}

void configset_parse(configset * restrict cfg, configset_parser * restrict parser, config * restrict cnfg)
{
  char *text = 0;
  size_t text_len;
  int exit;

  usnarfs(&text, &text_len, cfg->self_node_abspath);
  if(!text_len) {
    cfg->configset_stats.parse_ok++;
    system_stats_events.configset_parse_ok++;
    goto end;
  }

  if((exit = configset_parser_parse(parser, text, text_len + 2, cfg->self_node_abspath, 0, cnfg)) == 0)
  {
    cfg->configset_stats.parse_ok++;
    system_stats_events.configset_parse_ok++;
  }
  else
  {
    cfg->configset_stats.parse_err++;
    system_stats_events.configset_parse_err++;

    configset_parse_error(cfg, parser);
  }

end:
  free(text);
}

static void configset_dispose(configset * restrict cfg)
{
  llist_delete(&cfg->vertex.owner);
  llist_append(&configset_freelist, &cfg->vertex.owner);
  ASAN_TAINT(cfg);
}

void configset_release(configset * restrict cfg)
{
  moria_vertex *v;

  /* must have been fully excised from the graph previously */
  v = &cfg->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  configset_dispose(cfg);
}

void configset_setup(void)
{
}

void configset_cleanup()
{
  configset *v;

  while((v = llist_shift(&configset_freelist, typeof(*v), vertex.owner))) {
    ASAN_CLEAN(v);
    free(v);
  }
}

void configset_alloc(configset ** restrict vp)
{
  struct configset *config;

  if((config = llist_shift(&configset_freelist, typeof(*config), vertex.owner))) {
    ASAN_CLEAN(config);
    memset(config, 0, sizeof(*config));
  } else {
    xmalloc(&config, sizeof(*config));
  }

  moria_vertex_init(&config->vertex, &g_graph, VERTEX_CONFIGSET | VERTEX_OK);

  llist_prepend(&configset_list, &config->vertex.owner);

  vertex_alloc_probe(&config->vertex);
  *vp = config;
}

void configset_stats_collate(configset * restrict config, bool reset)
{
  if(reset) {
    memset(&config->stats, 0, sizeof(config->stats));
    memset(&config->configset_stats, 0, sizeof(config->configset_stats));
  }
}

size_t configset_path_cat(void * restrict dst, size_t sz, const configset * restrict cfg)
{
  return zcatf(dst, sz, "//bm/configsets/%.*s", cfg->id_len, cfg->id);
}

void configset_stats_build(configset * restrict configset, bool reset, bacon_writer * restrict builder_writer)
{
  stats_build(&configset->stats.record, &descriptor_stats_vertex, builder_writer);
  stats_build(&configset->configset_stats.record, &descriptor_stats_configset, builder_writer);
}

void configset_describe(const configset * restrict configset, struct bacon_writer * restrict builder_writer)
{
  { MAPPING
    bacon_writer_push_string(builder_writer);
    vertex_path_say(&configset->vertex, builder_writer->N);
    bacon_writer_pop_string(builder_writer);
    { SET
      {
        vertex_mappings(&configset->vertex, builder_writer);
        { MAPPING
          STRING("self")
          BYTES(configset->self_node_abspath, configset->self_node_abspath_len)
        }
        { MAPPING
          STRING("stats")
          { SET
            configset_stats_build((struct configset *)configset, false, builder_writer);
          }
        }
        vertex_relations_describe(&configset->vertex, builder_writer);
      }
    }
  }
}

void configset_identity_describe(const struct configset * restrict configset, struct bacon_writer * restrict builder_writer)
{
  attrs32_describe(configset->vertex.attrs, graph_vertex_attrs, builder_writer);

  { MAPPING
    STRING("id")
    BYTES(configset->id, configset->id_len);
  }
  { MAPPING
    STRING("path")
    STRING(configset->self_node_abspath);
  }
}

void configset_id(const struct configset * restrict configset, const char ** restrict id, uint16_t * restrict idl)
{
  *id = configset->id;
  *idl = configset->id_len;
}

void configset_compute_id(const struct configset * restrict cfg, struct identity_ctx * restrict id)
{
  /* configset */
  identity_type(id, vertex_type_get(&cfg->vertex));
  identity_add(id, " ", 1);

  /* path */
  fsent_id_path_compute(cfg->self_node, id);
}

#if CONFIG_DEVEL
void configsets_show(void)
{
  configset *fml;
  int x;

  printf("configsets, live : %zu free : %zu\n", llist_count(&configset_list), llist_count(&configset_freelist));
  x = 0;
  llist_foreach(&configset_list, fml, vertex.owner) {
    printf("%3u ", x++);
    vertex_identity_show(&fml->vertex);
    vertex_edges_show(&fml->vertex);
  }
}
#endif
