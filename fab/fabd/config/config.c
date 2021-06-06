/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

/*

config keys that should *only* be legal in the global/user config files
 filesystems
 special

*/

#include <unistd.h>

#include "types.h"
#include "xapi.h"

#include "xapi/trace.h"
#include "xapi/calltree.h"
#include "xlinux/xstdlib.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "valyria/set.h"
#include "valyria/map.h"
#include "valyria/list.h"
#include "valyria/llist.h"
#include "value/writer.h"
#include "fab/stats.h"
#include "common/snarf.h"
#include "common/hash.h"
#include "common/attrs.h"

#include "config.internal.h"
#include "yyutil/box.h"
#include "CONFIG.errtab.h"
#include "config_parser.internal.h"
#include "filesystem.h"
#include "build_thread.h"
#include "params.h"
#include "fsent.h"
#include "path_cache.h"
#include "walker.h"
#include "args.h"
#include "zbuffer.h"
#include "channel.h"
#include "logging.h"
#include "pattern.h"
#include "stats.h"
#include "system_state.h"
#include "events.h"

#include "macros.h"
#include "marshal.h"

static configblob * config_staging;
static config_parser * parser_staging;
static configblob * config_active;
static config_parser * parser_active;

/* these config nodes always exist */
fsent *system_config_node;
fsent *user_config_node;

/* active list is maintained in precendence order, including system and user config files */
static llist config_list = LLIST_INITIALIZER(config_list);          // active
static llist config_freelist = LLIST_INITIALIZER(config_freelist);  // free

/* whether a reconfiguration has completed */
bool config_reconfigured;

//
// static
//

static uint32_t box_string_ht_hash_fn(uint32_t h, const void * _ent, size_t entsz)
{
  const box_string * ent = _ent;
  return hash32(h, ent->v, ent->l);
}

static int box_string_ht_cmp_fn(const void * _A, size_t Asz, const void * _B, size_t Bsz)
{
  const box_string * A = _A;
  const box_string * B = _B;

  return memncmp(A->v, A->l, B->v, B->l);
}

static void config_compare_build(struct config_build * restrict new, struct config_build * restrict old)
{
  new->changed = !config_reconfigured
    || box_int16_cmp(new->concurrency, old->concurrency)
    ;
}

static void config_compare_workers(struct config_workers * restrict new, struct config_workers * restrict old)
{
  new->changed = !config_reconfigured
    || box_int16_cmp(new->concurrency, old->concurrency)
    ;
}

static void config_compare_filesystems(struct config_filesystems * restrict new, struct config_filesystems * restrict old)
{
  new->changed = !config_reconfigured
    || !map_equal(new->entries, old->entries)
    ;
}

static void config_compare_formula_path(struct config_formula_path * restrict new, struct config_formula_path * restrict old)
{
  new->changed = !config_reconfigured
    || !set_equal(new->dirs.entries, old->dirs.entries)
    || box_bool_cmp(new->copy_from_env, old->copy_from_env)
    ;
}

static void config_compare_formula(struct config_formula * restrict new, struct config_formula * restrict old)
{
  config_compare_formula_path(&new->path, &old->path);

  new->changed = !config_reconfigured
      || new->path.changed
      ;
}

static void config_compare_walker_list(struct config_walker_list * restrict new, struct config_walker_list * restrict old)
{
  llist *a = new->list.next;
  llist *b = old->list.next;

  pattern *A, *B;

  if(!config_reconfigured) {
    new->changed = true;
    return;
  }

  /* this will consider lists containing identical patterns in different order as different lists */
  while(a != &new->list && b != &old->list)
  {
    A = containerof(a, pattern, lln);
    B = containerof(b, pattern, lln);

    if(pattern_cmp(A, B)) {
      break;
    }

    a = a->next;
    b = b->next;
  }

  new->changed = false;
  if(a != &new->list || b != &old->list)
  {
    new->changed = true;
  }
}

static void config_compare_walker(struct config_walker * restrict new, struct config_walker * restrict old)
{
  config_compare_walker_list(&new->include, &old->include);
  config_compare_walker_list(&new->exclude, &old->exclude);

  new->changed = !config_reconfigured
      || new->include.changed
      || new->exclude.changed
      ;
}

static xapi parse(config_parser * restrict parser, config * restrict cfgn, configblob **cfgp)
{
  enter;

  char * text = 0;
  size_t text_len;
  channel *chan;
  fabipc_message *msg;
  xapi exit;

  STATS_INC(cfgn->stats.parsed_try);
  STATS_INC(g_stats.config_parsed_try);

  fatal(usnarfs, &text, &text_len, cfgn->self_node_abspath);
  if(text)
  {
tracef();
    if((exit = invoke(config_parser_parse, parser, text, text_len + 2, cfgn->self_node_abspath, 0, cfgp)))
    {
tracef();
      system_error = true;
      if(!events_would(FABIPC_EVENT_SYSTEM_STATE, &chan, &msg)) {
        xapi_calltree_unwind();
        goto XAPI_FINALLY;
      }

      msg->code = EINVAL;
#if DEBUG || DEVEL
      msg->size = xapi_trace_full(msg->text, sizeof(msg->text), 0);
#else
      msg->size = xapi_trace_pithy(msg->text, sizeof(msg->text), 0);
#endif
      events_publish(chan, msg);

      xapi_calltree_unwind();
      goto XAPI_FINALLY;
    }
tracef();
  }

  STATS_INC(cfgn->stats.parsed);
  STATS_INC(g_stats.config_parsed);

finally:
  wfree(text);
  xapi_infos("path", cfgn->self_node_abspath);
coda;
}

static xapi reparse()
{
  enter;

  configblob * cfg = 0;
  fsent *n;
  config *cfgn;

  // reset staging config
  fatal(config_parser_ixfree, &parser_staging);
  fatal(config_parser_create, &parser_staging);
  fatal(config_ixfree, &config_staging);

  llist_foreach(&config_list, cfgn, vertex.owner) {
    n = cfgn->self_node;
    logf(L_CONFIG, "staging config file @ %s", n->self_config->self_node_abspath);
    fatal(parse, parser_staging, cfgn, &cfg);

    if(system_error) {
      break;
    }
    if(!cfg) {
      continue;
    }

    if(config_staging == 0)
    {
      config_staging = cfg;
    }
    else
    {
      fatal(config_merge, config_staging, cfg);
      fatal(config_xfree, cfg);
    }
    cfg = 0;
  }

  /* all files empty */
  if(!config_staging)
  {
    fatal(config_create, &config_staging);
  }

finally:
  fatal(config_xfree, cfg);
coda;
}

xapi config_alloc(config ** restrict vp, moria_graph * restrict g)
{
  enter;

  config *v;

  if((v = llist_shift(&config_freelist, typeof(*v), vertex.owner)) == 0)
  {
    fatal(xmalloc, &v, sizeof(*v));
  }

  moria_vertex_init(&v->vertex, g, VERTEX_CONFIG);

  llist_prepend(&config_list, v, vertex.owner);
  *vp = v;

  finally : coda;
}

static void config_dispose(config * restrict vp)
{
  llist_delete_node(&vp->vertex.owner);
  llist_append(&config_freelist, vp, vertex.owner);
}

static xapi bootstrap_global_node(fsent ** restrict np, const char * restrict pathspec)
{
  enter;

  graph_invalidation_context invalidation = { };
  char text[512];
  const char *path;
  size_t len, z = 0;
  config *cfg = 0;

  len = strlen(pathspec);
  RUNTIME_ASSERT(len > 1);

  if(len >= 2 && memcmp(pathspec, "~/", 2) == 0)
  {
    z += znloads(text + z, sizeof(text) - z, g_params.homedir);
    z += znloads(text + z, sizeof(text) - z, "/");
    z += znloads(text + z, sizeof(text) - z, pathspec + 2);
    text[z] = 0;
    len = z;
    path = text;
  }
  else if(len >= 6 && memcmp(pathspec, "$HOME/", 6) == 0)
  {
    z += znloads(text + z, sizeof(text) - z, g_params.homedir);
    z += znloads(text + z, sizeof(text) - z, "/");
    z += znloads(text + z, sizeof(text) - z, pathspec + 6);
    text[z] = 0;
    len = z;
    path = text;
  }
  else if(pathspec[0] != '/')
  {
    z += znloads(text + z, sizeof(text) - z, g_params.proj_dir);
    z += znloads(text + z, sizeof(text) - z, "/");
    z += znloads(text + z, sizeof(text) - z, pathspec);
    text[z] = 0;
    len = z;
    path = text;
  }
  else
  {
    path = pathspec;
    len = strlen(path);
  }

  fatal(fsent_graft, path, np, &invalidation);
  fsent_kind_set(*np, VERTEX_CONFIG_FILE);
  fsent_protect_set(*np);
  fsent_invalid_set(*np);

  fatal(config_alloc, &cfg, &g_graph);
  znloadw(cfg->self_node_abspath, sizeof(cfg->self_node_abspath), path, len);
  cfg->self_node_abspath_len = len;
  (*np)->self_config = cfg;
  cfg->self_node = *np;

finally:
  graph_invalidation_end(&invalidation);
coda;
}

//
// internal
//

/// config_compare
//
// SUMMARY
//  mark sections of a new config struct as different from those of another config struct
//
bool config_compare(configblob * restrict new, configblob * restrict old)
{
  config_compare_build(&new->build, &old->build);
  config_compare_workers(&new->workers, &old->workers);
  config_compare_filesystems(&new->filesystems, &old->filesystems);
  config_compare_formula(&new->formula, &old->formula);
  config_compare_walker(&new->walker, &old->walker);

  new->changed = !config_reconfigured
    || new->build.changed
    || new->workers.changed
    || new->filesystems.changed
    || new->formula.changed
    || new->walker.changed
    ;

  return !new->changed;
}

#define CFGCOPY(dst, src, section, field) do {              \
  if(src->section.field || src->section.merge_overwrite) {  \
    box_free(refas(dst->section.field, bx));                \
    dst->section.field = src->section.field;                \
    src->section.field = 0;                                 \
  }                                                         \
  if(dst->section.field) {                                  \
    empty = false;                                          \
  }                                                         \
} while(0)

static void pattern_list_free(llist * restrict list)
{
  pattern *pat;
  llist *T;

  llist_foreach_safe(list, pat, lln, T) {
    pattern_free(pat);
  }

  llist_init_node(list);
}

xapi config_merge(configblob * restrict dst, configblob * restrict src)
{
  enter;

  bool empty;

  /* build */
  empty = true;
  CFGCOPY(dst, src, build, concurrency);
  dst->build.merge_significant = !empty;

  /* workers */
  empty = true;
  CFGCOPY(dst, src, workers, concurrency);
  dst->workers.merge_significant = !empty;

  /* walker */
  dst->walker.merge_significant = false;
  if(src->walker.merge_overwrite || src->walker.include.merge_overwrite)
  {
    pattern_list_free(&dst->walker.include.list);
  }
  llist_splice_head(&dst->walker.include.list, &src->walker.include.list);
  dst->walker.include.merge_significant = !llist_empty(&dst->walker.include.list);
  dst->walker.merge_significant |= dst->walker.include.merge_significant;

  if(src->walker.merge_overwrite || src->walker.exclude.merge_overwrite)
  {
    pattern_list_free(&dst->walker.exclude.list);
  }
  llist_splice_head(&dst->walker.exclude.list, &src->walker.exclude.list);
  dst->walker.exclude.merge_significant = !llist_empty(&dst->walker.exclude.list);
  dst->walker.merge_significant |= dst->walker.exclude.merge_significant;

  /* filesystems */
  if(src->filesystems.merge_overwrite)
  {
    fatal(map_xfree, dst->filesystems.entries);
    dst->filesystems.entries = src->filesystems.entries;
    src->filesystems.entries = 0;
  }
  else
  {
    fatal(map_splice, dst->filesystems.entries, src->filesystems.entries);
  }
  dst->filesystems.merge_significant = dst->filesystems.entries->size;

  /* formula path */
  dst->formula.path.merge_significant = false;
  if(src->formula.merge_overwrite || src->formula.path.merge_overwrite || src->formula.path.dirs.merge_overwrite)
  {
    fatal(set_xfree, dst->formula.path.dirs.entries);
    dst->formula.path.dirs.entries = src->formula.path.dirs.entries;
    src->formula.path.dirs.entries = 0;
  }
  else
  {
    fatal(set_splice, dst->formula.path.dirs.entries, src->formula.path.dirs.entries);
  }
  dst->formula.path.dirs.merge_significant = dst->formula.path.dirs.entries->size;
  dst->formula.path.merge_significant |= dst->formula.path.dirs.merge_significant;

  empty = true;
  CFGCOPY(dst, src, formula, path.copy_from_env);
  dst->formula.path.merge_significant |= !empty;
  dst->formula.merge_significant = dst->formula.path.merge_significant;

  finally : coda;
}

xapi config_throw(box * restrict val)
{
  enter;

  xapi_info_pushf("location", "[%d,%d - %d,%d]", val->loc.f_lin + 1, val->loc.f_col + 1, val->loc.l_lin + 1, val->loc.l_col + 1);

  fail(CONFIG_INVALID);

  finally : coda;
}

static void fse_free(void * _fse)
{
  struct config_filesystem_entry * fse = _fse;

  if(fse)
  {
    box_free(refas(fse->invalidate, bx));
  }
  wfree(fse);
}

xapi config_create(configblob ** restrict rv)
{
  enter;

  configblob * cfg = 0;

  fatal(xmalloc, &cfg, sizeof(*cfg));

  llist_init_node(&cfg->walker.exclude.list);
  llist_init_node(&cfg->walker.include.list);

  fatal(map_createx, &cfg->filesystems.entries, 0, fse_free, 0);
  fatal(set_createx
    , &cfg->formula.path.dirs.entries
    , 0
    , box_string_ht_hash_fn
    , box_string_ht_cmp_fn
    , (void*)box_free
    , 0
  );

  *rv = cfg;
  cfg = 0;

finally:
  fatal(config_xfree, cfg);
coda;
}

xapi config_xfree(configblob * restrict cfg)
{
  enter;

  if(cfg)
  {
    box_free(refas(cfg->build.concurrency, bx));
    box_free(refas(cfg->workers.concurrency, bx));

    fatal(map_xfree, cfg->filesystems.entries);
    fatal(set_xfree, cfg->formula.path.dirs.entries);

    box_free(refas(cfg->formula.path.copy_from_env, bx));

    pattern_list_free(&cfg->walker.exclude.list);
    pattern_list_free(&cfg->walker.include.list);
  }
  wfree(cfg);

  finally : coda;
}

xapi config_ixfree(configblob ** restrict cfg)
{
  enter;

  fatal(config_xfree, *cfg);
  *cfg = 0;

  finally : coda;
}

xapi config_writer_write(configblob * restrict cfg, value_writer * const restrict writer)
{
  enter;

  int x;
  const box_string *ent;
  const char *key;
  const struct config_filesystem_entry *fsent;
  pattern *pat;
  narrator_fixed fixed;
  char space[512];
  narrator *N;

  if(cfg->build.merge_significant)
  {
    fatal(value_writer_push_mapping, writer);
    fatal(value_writer_string, writer, "build");
    fatal(value_writer_push_set, writer);
    if(cfg->build.concurrency)
      fatal(value_writer_mapping_string_uint, writer, "concurrency", cfg->build.concurrency->v);
    fatal(value_writer_pop_set, writer);
    fatal(value_writer_pop_mapping, writer);
  }

  if(cfg->workers.merge_significant)
  {
    fatal(value_writer_push_mapping, writer);
    fatal(value_writer_string, writer, "workers");
    fatal(value_writer_push_set, writer);
    if(cfg->workers.concurrency)
      fatal(value_writer_mapping_string_uint, writer, "concurrency", cfg->workers.concurrency->v);
    fatal(value_writer_pop_set, writer);
    fatal(value_writer_pop_mapping, writer);
  }

  if(cfg->walker.merge_significant)
  {
    fatal(value_writer_push_mapping, writer);
    fatal(value_writer_string, writer, "walker");
    fatal(value_writer_push_set, writer);

    if(cfg->walker.include.merge_significant)
    {
      fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "include");
      fatal(value_writer_push_set, writer);

      llist_foreach(&cfg->walker.include.list, pat, lln) {
        N = narrator_fixed_init(&fixed, space, sizeof(space));
        fatal(pattern_say, pat, N);
        fatal(value_writer_bytes, writer, fixed.s, fixed.l);
      }

      fatal(value_writer_pop_set, writer);
      fatal(value_writer_pop_mapping, writer);
    }

    if(cfg->walker.exclude.merge_significant)
    {
      fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "exclude");
      fatal(value_writer_push_set, writer);

      llist_foreach(&cfg->walker.exclude.list, pat, lln) {
        N = narrator_fixed_init(&fixed, space, sizeof(space));
        fatal(pattern_say, pat, N);
        fatal(value_writer_bytes, writer, fixed.s, fixed.l);
      }

      fatal(value_writer_pop_set, writer);
      fatal(value_writer_pop_mapping, writer);
    }

    fatal(value_writer_pop_set, writer);
    fatal(value_writer_pop_mapping, writer);
  }

  if(cfg->filesystems.merge_significant)
  {
    fatal(value_writer_push_mapping, writer);
    fatal(value_writer_string, writer, "filesystems");
    fatal(value_writer_push_set, writer);

    for(x = 0; x < cfg->filesystems.entries->table_size; x++)
    {
      key = map_table_key(cfg->filesystems.entries, x);
      if(!key)
        continue;
      fsent = map_table_value(cfg->filesystems.entries, x);

      fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, key);
        fatal(value_writer_push_set, writer);
          fatal(value_writer_mapping_string_string, writer, "invalidate", attrs16_name_byvalue(invalidate_attrs, INVALIDATE_OPT & fsent->invalidate->v));
        fatal(value_writer_pop_set, writer);
      fatal(value_writer_pop_mapping, writer);
    }

    fatal(value_writer_pop_set, writer);
    fatal(value_writer_pop_mapping, writer);
  }

  if(cfg->formula.merge_significant)
  {
    fatal(value_writer_push_mapping, writer);
    fatal(value_writer_string, writer, "formula");
    fatal(value_writer_push_set, writer);

    if(cfg->formula.path.merge_significant)
    {
      fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "path");
      fatal(value_writer_push_set, writer);
      if(cfg->formula.path.copy_from_env)
        fatal(value_writer_mapping_string_bool, writer, "copy-from-env", cfg->formula.path.copy_from_env->v);

      if(cfg->formula.path.dirs.merge_significant)
      {
        fatal(value_writer_push_mapping, writer);
        fatal(value_writer_string, writer, "dirs");
        fatal(value_writer_push_set, writer);
        for(x = 0; x < cfg->formula.path.dirs.entries->table_size; x++)
        {
          if(!(ent = set_table_get(cfg->formula.path.dirs.entries, x)))
            continue;

          fatal(value_writer_string, writer, ent->v);
        }
        fatal(value_writer_pop_set, writer);
        fatal(value_writer_pop_mapping, writer);
      }

      fatal(value_writer_pop_set, writer);
      fatal(value_writer_pop_mapping, writer);
    }

    fatal(value_writer_pop_set, writer);
  }

  finally : coda;
}

//
// public
//

xapi config_setup()
{
  enter;

  fatal(config_parser_create, &parser_staging);
  fatal(config_create, &config_active);

  /* create the immutable config nodes, in precedence order */
  fatal(bootstrap_global_node, &user_config_node, g_args.user_config_path);
  fatal(bootstrap_global_node, &system_config_node, g_args.system_config_path);

  finally : coda;
}

xapi config_cleanup()
{
  enter;

  config *v;
  llist *T;

  fatal(config_parser_xfree, parser_active);
  fatal(config_xfree, config_active);
  fatal(config_parser_xfree, parser_staging);
  fatal(config_xfree, config_staging);

  llist_foreach_safe(&config_list, v, vertex.owner, T) {
    config_dispose(v);
  }

  llist_foreach_safe(&config_freelist, v, vertex.owner, T) {
    free(v);
  }

  finally : coda;
}

xapi config_say(configblob * restrict cfg, narrator * restrict N)
{
  enter;

  value_writer writer;

  value_writer_init(&writer);
  fatal(value_writer_open, &writer, N);
  fatal(config_writer_write, cfg, &writer);
  fatal(value_writer_close, &writer);

finally:
  fatal(value_writer_destroy, &writer);
coda;
}

xapi config_active_say(narrator * restrict N)
{
  enter;

  config *cfg;

  fatal(narrator_xsays, N, "#\n");
  fatal(narrator_xsays, N, "# active config files in ascending order of precedence\n");
  fatal(narrator_xsays, N, "#\n");
  llist_foreach(&config_list, cfg, vertex.owner) {
    fatal(narrator_xsayf, N, "#  %.*s\n", (int)cfg->self_node_abspath_len, cfg->self_node_abspath);
  }
  fatal(narrator_xsays, N, "#\n");
  fatal(narrator_xsays, N, "\n");

  fatal(config_say, config_active, N);

  finally : coda;
}

static xapi reconfigure()
{
  enter;

  channel *chan;
  fabipc_message *msg;

  // validate the new config
  xapi exit = 0;
  if(  (exit = invoke(filesystem_reconfigure, config_staging, true))
    || (exit = invoke(fsent_reconfigure, config_staging, true))
    || (exit = invoke(walker_system_reconfigure, config_staging, true))
    || (exit = invoke(build_thread_reconfigure, config_staging, true))
    || (exit = invoke(path_cache_reconfigure, config_staging, true))
  )
  {
    if(xapi_exit_errtab(exit) != perrtab_CONFIG) {
      fail(0);
    }

    if(events_would(FABIPC_EVENT_SYSTEM_STATE, &chan, &msg)) {
      msg->code = EINVAL;
#if DEBUG || DEVEL
      msg->size = xapi_trace_full(msg->text, sizeof(msg->text), 0);
#else
      msg->size = xapi_trace_pithy(msg->text, sizeof(msg->text), 0);
#endif
      events_publish(chan, msg);
    }
    xapi_calltree_unwind();
  }
  else
  {
    // promote the staging config
    fatal(config_parser_ixfree, &parser_active);
    parser_active = parser_staging;
    parser_staging = 0;

    fatal(config_ixfree, &config_active);
    config_active = config_staging;
    config_staging = 0;

    // reconfigure subsystems
    fatal(filesystem_reconfigure, config_active, false);
    fatal(fsent_reconfigure, config_active, false);
    fatal(walker_system_reconfigure, config_active, false);
    fatal(build_thread_reconfigure, config_active, false);
    fatal(path_cache_reconfigure, config_active, false);
  }

  finally : coda;
}

xapi config_system_reconcile(bool * restrict work, graph_invalidation_context * restrict invalidation)
{
  enter;

  fsent *n;
  moria_vertex *dirv, *v;
  llist unused, *T;
  config *cfg;

  *work = false;

  /* bootstrap any config files in the ancestry of the project root */
  dirv = &g_project_root->vertex;
  while(dirv)
  {
    if((v = moria_vertex_downw(dirv, MMS(FSENT_NAME_CONFIG))))
    {
      n = containerof(v, fsent, vertex);
      fatal(fsent_config_bootstrap, n);
    }

    dirv = moria_vertex_up(dirv);
  }

  /* now, rebuild the active list, in precedence order */
  llist_delete_node(&system_config_node->self_config->vertex.owner);
  llist_delete_node(&user_config_node->self_config->vertex.owner);
  llist_init_node(&unused);
  llist_splice_head(&unused, &config_list);

  /* check for config files in the ancestry of the project root */
  dirv = &g_project_root->vertex;
  while(dirv)
  {
    if((v = moria_vertex_downw(dirv, MMS(FSENT_NAME_CONFIG))))
    {
      n = containerof(v, fsent, vertex);
      llist_delete_node(&n->self_config->vertex.owner);
      llist_prepend(&config_list, n->self_config, vertex.owner);
    }

    dirv = moria_vertex_up(dirv);
  }

  llist_prepend(&config_list, user_config_node->self_config, vertex.owner);
  llist_prepend(&config_list, system_config_node->self_config, vertex.owner);

  llist_foreach_node(&config_list, T) {
    cfg = containerof(T, config, vertex.owner);
  }

  llist_foreach_node(&config_list, T) {
    cfg = containerof(T, config, vertex.owner);
    if(fsent_invalid_get(cfg->self_node)) {
      break;
    }
  }

  /* no invalid or unused nodes - no work */
  if(T == &config_list && llist_empty(&unused)) {
    goto XAPI_FINALIZE;
  }

  *work = true;

  /* config nodes not seen by the above loop are now unused */
  llist_foreach_safe(&unused, cfg, vertex.owner, T) {
    config_dispose(cfg);
  }

  /* all config files must be parsed any time config is reloaded */
  fatal(reparse);
  if(system_error) {
    goto XAPI_FINALIZE;
  }

  if(!config_compare(config_staging, config_active)) {
    fatal(reconfigure);
  }

  if(system_error) {
    goto XAPI_FINALIZE;
  }

  llist_foreach(&config_list, cfg, vertex.owner) {
    fatal(fsent_ok, cfg->self_node);
  }

  config_reconfigured = true;

  finally : coda;
}

xapi config_collate_stats(void *dst, size_t sz, config *cfg, bool reset, size_t *zp)
{
  enter;

  size_t z;
  fab_config_stats *stats;
  fab_config_stats lstats;
  descriptor_field *field;
  int x;

  stats = &cfg->stats;
  if(reset)
  {
    memcpy(&lstats, &stats, sizeof(lstats));
    memset(stats, 0, sizeof(*stats));
    stats = &lstats;
  }

  z = 0;
  z += marshal_u32(dst + z, sz - z, descriptor_fab_config_stats.id);

  /* event counters */
  for(x = 0; x < descriptor_fab_config_stats.members_len; x++)
  {
    field = descriptor_fab_config_stats.members[x];

    if(field->size == 8) {
      z += marshal_u64(dst + z, sz - z, stats->u64[field->offset / 8]);
    } else if(field->size == 4) {
      z += marshal_u32(dst + z, sz - z, stats->u32[field->offset / 4]);
    } else if(field->size == 2) {
      z += marshal_u16(dst + z, sz - z, stats->u16[field->offset / 2]);
    } else if(field->size == 1) {
      z += marshal_u16(dst + z, sz - z, stats->u8[field->offset / 1]);
    } else {
      RUNTIME_ABORT();
    }
  }

  *zp += z;

  finally : coda;
}
