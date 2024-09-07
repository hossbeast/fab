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
#include "common/hash.h"
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
#include "events.h"
#include "reconcile.h"
#include "vertex/probes.h"
#include "entlist.h"
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

void configset_parse(configset * restrict cfg, configset_parser * restrict parser, config * restrict cnfg)
{
  char *text = 0;
  size_t text_len;
  int exit;
  event_bacon *msg;
  events_context ectx;

  cfg->configset_stats.parse++;
  system_stats.configset_parse++;

  usnarfs(&text, &text_len, cfg->self_node_abspath);
  if(text)
  {
    if((exit = configset_parser_parse(parser, text, text_len + 2, cfg->self_node_abspath, 0, cnfg)))
    {
      msg = system_error_event(&ectx);
      msg->code = EINVAL;
      msg->size += zcatw(msg->text + msg->size, sizeof(msg->text) - msg->size, parser->yyu.error_str, parser->yyu.error_len);
      events_publish(&ectx);
      goto end;
    }
  }

  cfg->configset_stats.parse_ok++;
  system_stats.configset_parse_ok++;

end:
  free(text);
}

static void configset_dispose(configset * restrict cfg)
{
  llist_delete(&cfg->vertex.owner);
  llist_append(&configset_freelist, &cfg->vertex.owner);
}

void configset_release(configset * restrict cfg, graph_invalidation_context * restrict invalidation)
{
  moria_vertex *v;

  /* must have been fully excised from the graph previously */
  v = &cfg->vertex;
  RUNTIME_ASSERT(v->up_identity == 0);
  RUNTIME_ASSERT(rbtree_empty(&v->up));
  RUNTIME_ASSERT(rbtree_empty(&v->down));

  vertex_deleted_probe(&cfg->vertex);
  configset_dispose(cfg);
}

void configset_setup(void)
{
}

void configset_cleanup()
{
  configset *v;
  llist *T;

  llist_foreach_safe(&configset_list, v, vertex.owner, T) {
    configset_dispose(v);
  }

  llist_foreach_safe(&configset_freelist, v, vertex.owner, T) {
    free(v);
  }
}

void configset_alloc(configset ** restrict vp)
{
  struct configset *config;

  if((config = llist_shift(&configset_freelist, typeof(*config), vertex.owner)) == 0) {
    xmalloc(&config, sizeof(*config));
  } else {
    memset(config, 0, sizeof(*config));
  }

  moria_vertex_init(&config->vertex, &g_graph, VERTEX_CONFIGSET | VERTEX_OK);

  llist_prepend(&configset_list, &config->vertex.owner);

  vertex_protect_set(&config->vertex, true);
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
  return zcatf(dst, sz, "//configsets/%.*s", cfg->id_len, cfg->id);
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
      }
    }
  }
}

void configset_file_stats_build(fsent * restrict fse, bool reset, bacon_writer * restrict builder_writer)
{
  fsent_stats_build(fse, reset, builder_writer);
}

void configset_file_describe(const fsent * restrict fse, struct bacon_writer * restrict builder_writer)
{
  { MAPPING
    vertex_path_describe(&fse->vertex, builder_writer);
    { SET
      fsent_mappings(fse, builder_writer);
      { MAPPING
        STRING("configset")
        vertex_path_describe(&fse->self_configset->vertex, builder_writer);
      }
      { MAPPING STRING("stats")
        { SET
          configset_file_stats_build((struct fsent *)fse, false, builder_writer);
        }
      }
    }
  }
}

void configset_identity_say(const struct configset * restrict configset, struct narrator * restrict N)
{
  attrs32_say(configset->vertex.attrs, graph_vertex_attrs, N);
  narrator_sayf(N, " id %.*s path %s", configset->id_len, configset->id, configset->self_node_abspath);
}

void configset_file_identity_say(const struct fsent * restrict fse, struct narrator * restrict N)
{
  attrs32_say(fse->vertex.attrs, graph_vertex_attrs, N);
  narrator_sayf(N, " id %.*s path %s", fse->self_configset->id_len, fse->self_configset->id, fse->self_configset->self_node_abspath);
}
