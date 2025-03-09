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
#include "bam/config_tell.h"
#include "narrator/fixed.h"
#include "narrator/growing.h"
#include "valyria/hashtable.h"
#include "valyria/llist.h"
#include "bacon/writer.h"
#include "bacon/builder.h"
#include "common/snarf.h"
#include "common/attrs.h"
#include "lorien/path_normalize.h"

#include "configset.h"
#include "config.h"
#include "configset_parser.h"
#include "build.h"
#include "params.h"
#include "fsent.h"
#include "walker.h"
#include "shadow.h"
#include "args.h"
#include "zcat.h"
#include "channel.h"
#include "pattern.h"
#include "stats.h"
#include "system.h"
#include "events/events.h"
#include "reconcile.h"
#include "module_system.h"
#include "vertex.h"
#include "vertex/probes.h"
#include "config_system.h"
#include "special_edge.h"
#include "request_handler.h"
#include "fsedge.h"
#include "timeout_thread.h"
#include "sweeper_thread.h"
#include "plugin_system.h"
#include "edges.h"
#include "vertices.h"
#include "subsys.h"
#include "disintegrate.h"

#include "macros.h"
#include "marshal.h"

union stats_sys_config config_sys_stats;

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

subsys sys_config;

//
// static
//

static uint16_t render_pathspec(char path[PATH_MAX], const char * restrict pathspec)
{
  path_normalize_context ctx;

  ctx.home = g_params.homedir;
  ctx.home_len = g_params.homedir_len;
  ctx.base = g_basedir;
  ctx.base_len = g_basedir_len;

  return path_normalize(&ctx, path, PATH_MAX, pathspec, strlen(pathspec));
}

//
// public
//

void config_system_setup(void)
{
  char *c;

  subsys_init(&sys_config, VERTEX_SYS_CONFIG);
  shadow_mkref(g_shadow_bm, sys_config.vertex.label, sys_config.vertex.label_len, &sys_config.vertex);

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
  struct edges edges;
  struct vertices unlinking;
  struct vertices releasing;

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
    if((v = moria_vertex_downw(dirv, MMS(FSENT_NAME_CONFIG))))
    {
      n = containerof(v, fsent, vertex);
      fsent_configset_bootstrap(n, invalidation);
    }

    dirv = moria_vertex_up(dirv);
  }

  if(!invalidated) {
    config_sys_stats.reconfigure_no_work++;
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
    if((v = moria_vertex_downw(dirv, MMS(FSENT_NAME_CONFIG))))
    {
      RUNTIME_ASSERT(vertex_kind_get(v) == VERTEX_KIND_FSENT);
      if(!vertex_exists_get(v)) {
        goto next;
      }
      n = containerof(v, fsent, vertex);
      cfg = fsent_is_configset_file(n);
      llist_delete(&cfg->vertex.owner);
      llist_prepend(&configset_list, &cfg->vertex.owner);
    }

  next:
    dirv = moria_vertex_up(dirv);
  }

  /* global config nodes */
  if(system_configset_node) {
    cfg = fsent_is_configset_file(system_configset_node);
    llist_delete(&cfg->vertex.owner);
    llist_prepend(&configset_list, &cfg->vertex.owner);
  }

  if(user_configset_node) {
    cfg = fsent_is_configset_file(user_configset_node);
    llist_delete(&cfg->vertex.owner);
    llist_prepend(&configset_list, &cfg->vertex.owner);
  }

  /* config nodes not seen by the above loop are now unused */
  edges_init(&edges);
  vertices_init(&unlinking);
  vertices_init(&releasing);
  llist_foreach_safe(&unused, cfg, vertex.owner, T) {
    vertices_push(&unlinking, &cfg->vertex);
  }
  graph_disintegrate(&edges, &unlinking, &releasing, invalidation);

  /* parse active config files onto a new config */
  config_alloc(&config_next);
  llist_foreach_node(&configset_list, T) {
    cfg = containerof(T, typeof(*cfg), vertex.owner);
    configset_parse(cfg, parser, config_next);
    if(system_has_error) {
      goto err;
    }
  }

  /* validate the new config */
  namespace_reconfigure(config_next, true, invalidation);
  if(system_has_error) {
    goto err;
  }
  module_system_reconfigure(config_next, true, invalidation);
  if(system_has_error) {
    goto err;
  }
  fstree_reconfigure(config_next, true);
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
  timeout_thread_reconfigure(config_next, true);
  if(system_has_error) {
    goto err;
  }
  sweeper_thread_reconfigure(config_next, true);
  if(system_has_error) {
    goto err;
  }
  plugin_system_reconfigure(config_next, true, invalidation);
  if(system_has_error) {
    goto err;
  }

  config_sys_stats.reconfigure_ok++;

  /* flip */
  config_free(config_active, parser);
  config_active = config_next;
  config_next = 0;

//config_show(config_active);

  // reconfigure subsystems
  namespace_reconfigure(config_active, false, invalidation);
  module_system_reconfigure(config_active, false, invalidation);
  fstree_reconfigure(config_active, false);
  walker_system_reconfigure(config_active, false);
  build_reconfigure(config_active, false);
  timeout_thread_reconfigure(config_active, false);
  sweeper_thread_reconfigure(config_active, false);
  plugin_system_reconfigure(config_active, false, invalidation);

  llist_foreach(&configset_list, cfg, vertex.owner) {
    vertex_mark_fresh(&cfg->vertex);
    vertex_mark_fresh(&cfg->self_node->vertex);
  }
  invalidated = false;
  return;

err:
  config_sys_stats.reconfigure_err++;
  config_free(config_next, parser);
}

void config_system_invalidate()
{
  invalidated = true;
}

void config_sys_tell(const moria_vertex * restrict v, struct request_handler * restrict handler)
{
  bamipc_message *msg;
  narrator_growing growing;
  narrator * N;
  int x;
  configset *cfgn;
  size_t z;
  size_t list_size;
  size_t list_size_off;
  size_t list_len_off;
  char *dst;
  size_t sz;

  msg = request_handler_tx(handler, BAMIPC_MSG_RESULT, descriptor_bam_config_tell_result.id, 0);
  dst = msg->text;
  sz = msg->size;
  z = 0;
  z += marshal_u32(dst + z, sz - z, descriptor_bam_config_tell_result.id);

  /* list each contributing config file */
  z += align(z, 4);
  list_size_off = z;
  z += marshal_u32(dst + z, sz - z, 0);            /* size of the list blob */
  z += align(z, 2);
  list_len_off = z;
  z += marshal_u16(dst + z, sz - z, 0);            /* items in the list blob */

  x = 0;
  llist_foreach(&configset_list, cfgn, vertex.owner) {
    z += marshal_u32(dst + z, sz - z, descriptor_bam_config_tell_item.id);

    /* id */
    z += marshal_u16(dst + z, sz - z, cfgn->id_len);
    z += zcatw(dst + z, sz - z, cfgn->id, cfgn->id_len);

    /* abspath */
    z += marshal_u16(dst + z, sz - z, cfgn->self_node_abspath_len);
    z += zcatw(dst + z, sz - z, cfgn->self_node_abspath, cfgn->self_node_abspath_len);
    x++;
  }

  list_size = z - list_size_off;
  marshal_u32(dst + list_size_off, sz - list_size_off, list_size);
  marshal_u16(dst + list_len_off, sz - list_len_off, x);

  msg->len = z;
  request_handler_post(handler, msg);

  N = narrator_growing_init(&growing);
  if(config_active) {
    config_say(config_active, N);
  }

  x = 0;
  while(x < growing.l)
  {
    msg = request_handler_tx(handler, BAMIPC_MSG_RESULT, BAMIPC_MSG_BACON, 0);

    msg->len = MIN(growing.l - x, msg->size);
    memcpy(msg->text, growing.s + x, msg->len);

    request_handler_post(handler, msg);
    x += msg->len;
  }

  narrator_growing_destroy(&growing);
}

void config_sys_stats_build(const moria_vertex * restrict v, bool reset, bacon_writer * restrict builder_writer)
{
  stats_build(&config_sys_stats.record, &descriptor_stats_sys_config, builder_writer);
}

void config_sys_describe(const moria_vertex * restrict v, struct bacon_writer * restrict builder_writer)
{
  { MAPPING
    vertex_path_describe(v, builder_writer);
    { SET
      vertex_mappings(v, builder_writer);
      { MAPPING STRING("active")
        if(config_active) {
          { SET
            config_describe(config_active, builder_writer);
          }
        } else {
          SET
        }
      }
      { MAPPING
        STRING("stats")
        { SET
          config_sys_stats_build(v, false, builder_writer);
        }
      }
      vertex_relations_describe(v, builder_writer);
    }
  }
}
