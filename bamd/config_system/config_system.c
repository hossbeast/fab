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
#include "common/snarf.h"
#include "common/hash.h"
#include "common/attrs.h"
#include "lorien/path_normalize.h"

#include "configset.h"
#include "config.h"
#include "configset_parser.h"
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
#include "system.h"
#include "events.h"
#include "reconcile.h"
#include "module_system.h"
#include "vertex.h"
#include "vertex/probes.h"
#include "config_system.h"
#include "special_edge.h"
#include "fsedge.h"
#include "timeout_thread.h"
#include "plugin_system.h"

#include "macros.h"
#include "marshal.h"

static configset_parser *parser;
static bool invalidated = true;

char user_config_dir[PATH_MAX];
uint16_t user_config_dir_len;
char user_config_name[256];
uint16_t user_config_name_len;

char system_config_dir[PATH_MAX];
uint16_t system_config_dir_len;
char system_config_name[256];
uint16_t system_config_name_len;

config *config_active;
fsent *system_config_dirnode;
fsent *user_config_dirnode;

//
// static
//

static uint16_t render_pathspec(char path[PATH_MAX], const char * restrict pathspec)
{
  path_normalize_context ctx;

  ctx.home = g_params.homedir;
  ctx.home_len = g_params.homedir_len;
  ctx.base = g_base_dir;
  ctx.base_len = g_base_dir_len;

  return path_normalize(&ctx, path, PATH_MAX, pathspec, strlen(pathspec));
}

//
// public
//

void config_system_setup(void)
{
  char *c;

  configset_parser_create(&parser);

  /* user config file path */
  user_config_dir_len = render_pathspec(user_config_dir, g_args.user_config_path);
  c = user_config_dir + user_config_dir_len;
  while(c != user_config_dir && *c != '/') {
    c--;
    user_config_dir_len--;
  }
  RUNTIME_ASSERT(c != user_config_dir);
  *c = 0;
  fsent_graft(user_config_dir, &user_config_dirnode);

  user_config_name_len = strlen(c + 1);
  memcpy(user_config_name, c + 1, user_config_name_len);
  user_config_name[user_config_name_len] = 0;

  /* system config file path */
  system_config_dir_len = render_pathspec(system_config_dir, g_args.system_config_path);
  c = system_config_dir + system_config_dir_len;
  while(c != system_config_dir && *c != '/') {
    c--;
    system_config_dir_len--;
  }
  RUNTIME_ASSERT(c != system_config_dir);
  *c = 0;
  fsent_graft(system_config_dir, &system_config_dirnode);

  system_config_name_len = strlen(c + 1);
  memcpy(system_config_name, c + 1, system_config_name_len);
  system_config_name[system_config_name_len] = 0;
}

void config_system_cleanup()
{
  config_free(config_active, parser);
  configset_parser_free(parser);
}

void config_system_reconcile(bool * restrict work, graph_invalidation_context * restrict invalidation)
{
  fsent *n;
  moria_vertex *dirv, *v;
  llist unused, *T;
  configset *cfg;
  config *config_next;

  /* global system config */
  if((v = moria_vertex_downw(&system_config_dirnode->vertex, system_config_name, system_config_name_len))) {
    system_configset_node = containerof(v, fsent, vertex);
    fsent_configset_bootstrap(system_configset_node, invalidation);
  } else {
    system_configset_node = 0;
  }

  /* global user config */
  if((v = moria_vertex_downw(&user_config_dirnode->vertex, user_config_name, user_config_name_len))) {
    user_configset_node = containerof(v, fsent, vertex);
    fsent_configset_bootstrap(user_configset_node, invalidation);
  } else {
    user_configset_node = 0;
  }

  /* bootstrap any config files in the ancestry of cwd - this can set invalidated */
  dirv = &g_base->vertex;
  while(dirv)
  {
    if((v = moria_vertex_downw(dirv, MMS(fsent_name_config))))
    {
      n = containerof(v, fsent, vertex);
      fsent_configset_bootstrap(n, invalidation);
    }

    dirv = moria_vertex_up(dirv);
  }

  if(!invalidated) {
    *work = false;
    return;
  }
  *work = true;

  /* now, rebuild the active list, in precedence order */
  llist_init(&unused);
  llist_splice_head(&unused, &configset_list);

  /* apply config files in the ancestry of cwd */
  dirv = &g_base->vertex;
  while(dirv)
  {
    if((v = moria_vertex_downw(dirv, MMS(fsent_name_config))))
    {
      RUNTIME_ASSERT(vertex_type_get(v) == VERTEX_CONFIGSET_FILE);
      if(!vertex_exists_get(v)) {
        goto next;
      }

      n = containerof(v, fsent, vertex);
      cfg = n->self_configset;
      llist_delete(&cfg->vertex.owner);
      llist_prepend(&configset_list, &cfg->vertex.owner);
    }

  next:
    dirv = moria_vertex_up(dirv);
  }

  /* global config nodes */
  if(system_configset_node) {
    llist_delete(&system_configset_node->self_configset->vertex.owner);
    llist_prepend(&configset_list, &system_configset_node->self_configset->vertex.owner);
  }

  if(user_configset_node) {
    llist_delete(&user_configset_node->self_configset->vertex.owner);
    llist_prepend(&configset_list, &user_configset_node->self_configset->vertex.owner);
  }

  /* config nodes not seen by the above loop are now unused */
  llist_foreach_safe(&unused, cfg, vertex.owner, T) {
    n = cfg->self_node;
    configset_release(cfg, invalidation);
  }

  /* parse active config files onto a new config */
  config_alloc(&config_next);
  llist_foreach_node(&configset_list, T) {
    cfg = containerof(T, typeof(*cfg), vertex.owner);
    configset_parse(cfg, parser, config_next);
    if(system_has_error) {
      goto err;
    }
  }

  // validate the new config
  fstree_reconfigure(config_next, true);
  if(system_has_error) {
    goto err;
  }
  fsent_reconfigure(config_next, true);
  if(system_has_error) {
    goto err;
  }
  walker_system_reconfigure(config_next, true);
  if(system_has_error) {
    goto err;
  }
  build_reconfigure(config_next, true);
  if(system_has_error) {
    goto err;
  }
  path_cache_reconfigure(config_next, true);
  if(system_has_error) {
    goto err;
  }
  timeout_thread_reconfigure(config_next, true);
  if(system_has_error) {
    goto err;
  }
  plugin_system_reconfigure(config_next, true, invalidation);
  if(system_has_error) {
    goto err;
  }

  /* flip */
  config_free(config_active, parser);
  config_active = config_next;
  config_next = 0;

  // reconfigure subsystems
  fstree_reconfigure(config_active, false);
  fsent_reconfigure(config_active, false);
  walker_system_reconfigure(config_active, false);
  build_reconfigure(config_active, false);
  path_cache_reconfigure(config_active, false);
  timeout_thread_reconfigure(config_active, false);
  plugin_system_reconfigure(config_active, false, invalidation);

  llist_foreach(&configset_list, cfg, vertex.owner) {
    vertex_mark_fresh(&cfg->vertex);
    vertex_mark_fresh(&cfg->self_node->vertex);
  }
  invalidated = false;
  return;

err:
  config_free(config_next, parser);
}

void config_system_invalidate()
{
  invalidated = true;
}
