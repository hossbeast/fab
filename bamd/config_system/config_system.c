/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

/*

config keys that should *only* be legal in the global/user config files
 filesystems
 special

*/

#include <unistd.h>
#include <errno.h>

#include "types.h"

#include "xlinux/xstdlib.h"
#include "xlinux/xunistd.h"
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

#include "config.h"
#include "configuration.h"
#include "config_parser.h"
#include "filesystem.h"
#include "build.h"
#include "params.h"
#include "fsent.h"
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
#include "module_system.h"
#include "vertex.h"
#include "vertex/probes.h"
#include "config_system.h"
#include "special_edge.h"
#include "fsedge.h"

#include "macros.h"
#include "marshal.h"

static config_parser *parser;
static bool invalidated;

configuration *configuration_active;

//
// static
//

static void bootstrap_global_config(fsent ** restrict np, const char * restrict pathspec, graph_invalidation_context *invalidation)
{
  char text[PATH_MAX];
  char path[PATH_MAX];
  size_t len, z = 0;
  const char *src;
  fsent *fse;

  len = strlen(pathspec);
  RUNTIME_ASSERT(len > 1);

  if(len >= 2 && memcmp(pathspec, "~/", 2) == 0)
  {
    z += zcats(text + z, sizeof(text) - z, g_params.homedir);
    z += zcats(text + z, sizeof(text) - z, "/");
    z += zcats(text + z, sizeof(text) - z, pathspec + 2);
    text[z] = 0;
    src = text;
  }
  else if(len >= 6 && memcmp(pathspec, "$HOME/", 6) == 0)
  {
    z += zcats(text + z, sizeof(text) - z, g_params.homedir);
    z += zcats(text + z, sizeof(text) - z, "/");
    z += zcats(text + z, sizeof(text) - z, pathspec + 6);
    text[z] = 0;
    src = text;
  }
  else if(pathspec[0] != '/')
  {
    z += zcats(text + z, sizeof(text) - z, g_cwd_path);
    z += zcats(text + z, sizeof(text) - z, "/");
    z += zcats(text + z, sizeof(text) - z, pathspec);
    text[z] = 0;
    src = text;
  }
  else
  {
    src = pathspec;
  }
  if(!realpath(src, path)) {
    if(errno != ENOENT) {
      dprintf(2, "%s : %s\n", src, strerror(errno));
      exit(1);
    }
  }


  fsent_graft(path, &fse, invalidation);
  fsent_config_bootstrap(fse, invalidation);

  vertex_state_set(&fse->vertex, VERTEX_UNCREATED);
  vertex_shield_set(&fse->vertex, true);
  vertex_protect_set(&fse->vertex, false);

  *np = fse;
}

//
// public
//

void config_system_setup(void)
{
  config_parser_create(&parser);
}

void config_system_bootstrap()
{
  graph_invalidation_context invalidation = { };

  graph_invalidation_begin(&invalidation);

  /* create the immutable config nodes, in precedence order */
  bootstrap_global_config(&user_config_node, g_args.user_config_path, &invalidation);
  bootstrap_global_config(&system_config_node, g_args.system_config_path, &invalidation);

  graph_invalidation_end(&invalidation);
}

void config_system_cleanup()
{
  configuration_free(configuration_active, parser);
  config_parser_free(parser);
}

void config_system_reconcile(bool * restrict work, graph_invalidation_context * restrict invalidation)
{
  fsent *n;
  moria_vertex *dirv, *v;
  llist unused, *T;
  config *cfg;
  configuration *configuration_next;

  /* bootstrap any config files in the ancestry of cwd - this can set invalidated */
  dirv = &g_cwd->vertex;
  while(dirv)
  {
    if((v = moria_vertex_downw(dirv, MMS(fsent_name_config))))
    {
      n = containerof(v, fsent, vertex);
      fsent_config_bootstrap(n, invalidation);
    }

    dirv = moria_vertex_up(dirv);
  }

  if(!invalidated) {
    *work = false;
    return;
  }

  *work = true;

  /* now, rebuild the active list, in precedence order */
  llist_delete(&system_config_node->self_config->vertex.owner);
  llist_delete(&user_config_node->self_config->vertex.owner);
  llist_init(&unused);
  llist_splice_head(&unused, &config_list);

  /* apply config files in the ancestry of cwd */
  dirv = &g_cwd->vertex;
  while(dirv)
  {
    if((v = moria_vertex_downw(dirv, MMS(fsent_name_config))))
    {
      RUNTIME_ASSERT(vertex_type_get(v) == VERTEX_CONFIG_FILE);
      if(!vertex_exists_get(v)) {
        goto next;
      }

      n = containerof(v, fsent, vertex);
      cfg = n->self_config;
      llist_delete(&cfg->vertex.owner);
      llist_prepend(&config_list, &cfg->vertex.owner);
    }

  next:
    dirv = moria_vertex_up(dirv);
  }

  /* global config nodes */
  if(vertex_exists_get(&system_config_node->vertex)) {
    llist_prepend(&config_list, &system_config_node->self_config->vertex.owner);
  } else {
    vertex_state_set(&system_config_node->self_config->vertex, VERTEX_OK);
  }

  if(vertex_exists_get(&user_config_node->vertex)) {
    llist_prepend(&config_list, &user_config_node->self_config->vertex.owner);
  } else {
    vertex_state_set(&user_config_node->self_config->vertex, VERTEX_OK);
  }

  /* config nodes not seen by the above loop are now unused */
  llist_foreach_safe(&unused, cfg, vertex.owner, T) {
    n = cfg->self_node;
    config_release(cfg, invalidation);
    fsedge_disintegrate(containerof(n->vertex.up_identity, fsedge, edge), invalidation);
  }

  /* parse active config files onto a new configuration */
  configuration_alloc(&configuration_next);
  llist_foreach_node(&config_list, T) {
    cfg = containerof(T, typeof(*cfg), vertex.owner);
    config_parse(cfg, parser, configuration_next);
    if(system_error) {
      goto err;
    }
  }

  // validate the new config
  filesystem_reconfigure(configuration_next, true);
  if(system_error) {
    goto err;
  }
  fsent_reconfigure(configuration_next, true);
  if(system_error) {
    goto err;
  }
  walker_system_reconfigure(configuration_next, true);
  if(system_error) {
    goto err;
  }
  build_reconfigure(configuration_next, true);
  if(system_error) {
    goto err;
  }
  path_cache_reconfigure(configuration_next, true);
  if(system_error) {
    goto err;
  }

  /* flip */
  configuration_free(configuration_active, parser);
  configuration_active = configuration_next;
  configuration_next = 0;

  // reconfigure subsystems
  filesystem_reconfigure(configuration_active, false);
  fsent_reconfigure(configuration_active, false);
  walker_system_reconfigure(configuration_active, false);
  build_reconfigure(configuration_active, false);
  path_cache_reconfigure(configuration_active, false);

  llist_foreach(&config_list, cfg, vertex.owner) {
    vertex_mark_fresh(&cfg->vertex);
    vertex_mark_fresh(&cfg->self_node->vertex);
  }
  invalidated = false;
  return;

err:
  configuration_free(configuration_next, parser);
}

void config_system_invalidate()
{
  invalidated = true;
}
