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
#include "bam/stats.h"
#include "common/snarf.h"
#include "common/hash.h"
#include "common/attrs.h"
#include "moria/foreach.h"
#include "moria/connect.h"

#include "config.h"
#include "config_parser.h"
#include "configuration.h"
#include "filesystem.h"
#include "build.h"
#include "params.h"
#include "fsent.h"
#include "vertex.h"
#include "path_cache.h"
#include "walker.h"
#include "args.h"
#include "zcat.h"
#include "channel.h"
#include "pattern.h"
#include "stats.h"
#include "system_state.h"
#include "events.h"
#include "reconcile.h"
#include "vertex/probes.h"
#include "entlist.h"
#include "edge.h"
#include "special_edge.h"

#include "macros.h"
#include "marshal.h"

/* active list is maintained in precendence order, including system and user config files */
llist config_list = LLIST_INITIALIZER(config_list);                 // active
static llist config_freelist = LLIST_INITIALIZER(config_freelist);  // free

/* these config nodes always exist */
fsent *system_config_node;
fsent *user_config_node;

void config_parse(config * restrict cfg, config_parser * restrict parser, configuration * restrict cnfg)
{
  char *text = 0;
  size_t text_len;
  int exit;
  bamipc_message *msg;
  channel *chan;

  STATS_INC(cfg->config_stats.parse);
  STATS_INC(g_stats.config_parse);

  usnarfs(&text, &text_len, cfg->self_node_abspath);
  if(text)
  {
    if((exit = config_parser_parse(parser, text, text_len + 2, cfg->self_node_abspath, 0, cnfg)))
    {
      msg = system_error_event(&chan);
      msg->code = EINVAL;
      msg->size += zcatw(msg->text + msg->size, sizeof(msg->text) - msg->size, parser->yyu.error_str, parser->yyu.error_len);
      events_publish(chan, msg);
      goto end;
    }
  }

  STATS_INC(cfg->config_stats.parse_ok);
  STATS_INC(g_stats.config_parse_ok);

end:
  free(text);
}

static void config_dispose(config * restrict cfg)
{
  llist_delete(&cfg->vertex.owner);
  llist_append(&config_freelist, &cfg->vertex.owner);
}

void config_release(config * restrict cfg, graph_invalidation_context * restrict invalidation)
{
  moria_vertex *v;
  moria_edge *edge;
  moria_foreach_state st;
  entlist edges, *eln;
  special_edge *se;

  /* proactive edge disconnect */
  v = &cfg->vertex;
  entlist_init(&edges);
  moria_foreach_edge(&st, v, edge) {
    switch(edge_type_get(edge)) {
      case EDGE_CONFIG_SPECIAL:
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

  vertex_deleted_probe(&cfg->vertex);
  config_dispose(cfg);
}

void config_setup(void)
{
}

void config_cleanup()
{
  config *v;
  llist *T;

  config_dispose(system_config_node->self_config);
  config_dispose(user_config_node->self_config);

  llist_foreach_safe(&config_list, v, vertex.owner, T) {
    config_dispose(v);
  }

  llist_foreach_safe(&config_freelist, v, vertex.owner, T) {
    free(v);
  }
}

void config_alloc(config ** restrict vp)
{
  struct config *config;

  if((config = llist_shift(&config_freelist, typeof(*config), vertex.owner)) == 0) {
    xmalloc(&config, sizeof(*config));
  } else {
    memset(config, 0, sizeof(*config));
  }

  moria_vertex_init(&config->vertex, &g_graph, VERTEX_CONFIG | VERTEX_OK);

  llist_prepend(&config_list, &config->vertex.owner);

  vertex_protect_set(&config->vertex, true);
  vertex_alloc_probe(&config->vertex);
  *vp = config;
}

size_t config_stats_collate(void *dst, size_t sz, bool reset, config * restrict config)
{
  size_t z;
  bam_config_stats *stats;
  bam_config_stats tmp;

  /* stats for the config vertex */
  z = 0;
  z += vertex_stats_collate(dst + z, sz - z, reset, &config->vertex);

  /* stats for the config */
  memcpy(&tmp, &config->config_stats, sizeof(tmp));
  if(reset) {
    memset(&config->config_stats, 0, sizeof(config->config_stats));
  }
  stats = &tmp;

  z += descriptor_marshal_object(dst + z, sz - z, &descriptor_bam_config_stats, &stats->record, 0);

  return z;
}
