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
#include <errno.h>

#include "types.h"

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
#include "config_parser.internal.h"
#include "filesystem.h"
#include "build.h"
#include "params.h"
#include "fsent.h"
#include "path_cache.h"
#include "walker.h"
#include "args.h"
#include "zbuffer.h"
#include "channel.h"
#include "pattern.h"
#include "stats.h"
#include "system_state.h"
#include "events.h"
#include "reconcile.h"

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

/* whether a reconfiguration has ever completed */
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

static void parse(config_parser * restrict parser, config * restrict cfgn, configblob **cfgp)
{
  char * text = 0;
  size_t text_len;
  channel *chan;
  fabipc_message *msg;
  int exit;

  STATS_INC(cfgn->stats.parsed_try);
  STATS_INC(g_stats.config_parsed_try);

  usnarfs(&text, &text_len, cfgn->self_node_abspath);
  if(text)
  {
    if((exit = config_parser_parse(parser, text, text_len + 2, cfgn->self_node_abspath, 0, cfgp)))
    {
      system_error = true;
      if(events_would(FABIPC_EVENT_SYSTEM_STATE, &chan, &msg)) {
        msg->code = EINVAL;
        msg->size = znloadw(msg->text, sizeof(msg->text), parser->yyu.error_str, parser->yyu.error_len);
        events_publish(chan, msg);
      }
      goto end;
    }
  }

  STATS_INC(cfgn->stats.parsed);
  STATS_INC(g_stats.config_parsed);

end:
  wfree(text);
}

static void reparse()
{
  configblob * cfg = 0;
  fsent *n;
  config *cfgn;

  // reset staging config
  config_parser_ixfree(&parser_staging);
  config_parser_create(&parser_staging);
  config_ixfree(&config_staging);

  /* all files must be re-parsed, not only those which have changed, because
   * config_merge is destructive to the source config */
  llist_foreach(&config_list, cfgn, vertex.owner) {
    n = cfgn->self_node;
    printf("staging config file @ %s\n", n->self_config->self_node_abspath);
    parse(parser_staging, cfgn, &cfg);

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
      config_merge(config_staging, cfg);
      config_xfree(cfg);
    }
    cfg = 0;
  }

  /* all files empty */
  if(!config_staging)
  {
    config_create(&config_staging);
  }

  config_xfree(cfg);
}

void config_alloc(config ** restrict vp, moria_graph * restrict g)
{
  config *v;

  if((v = llist_shift(&config_freelist, typeof(*v), vertex.owner)) == 0)
  {
    xmalloc(&v, sizeof(*v));
  }

  moria_vertex_init(&v->vertex, g, VERTEX_CONFIG);

  llist_prepend(&config_list, v, vertex.owner);
  *vp = v;
}

static void config_dispose(config * restrict vp)
{
  llist_delete_node(&vp->vertex.owner);
  llist_append(&config_freelist, vp, vertex.owner);
}

static void bootstrap_global_node(fsent ** restrict np, const char * restrict pathspec)
{
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

  fsent_graft(path, np, &invalidation);
  fsent_kind_set(*np, VERTEX_CONFIG_FILE);
  fsent_protect_set(*np);
  fsent_invalid_set(*np);

  config_alloc(&cfg, &g_graph);
  znloadw(cfg->self_node_abspath, sizeof(cfg->self_node_abspath), path, len);
  cfg->self_node_abspath_len = len;
  (*np)->self_config = cfg;
  cfg->self_node = *np;

  graph_invalidation_end(&invalidation);
}

//
// internal
//

/*
* mark sections of a new config struct as different from those of another config struct
*
* returns boolean value indicating whether old and new are equal
*/
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

void config_merge(configblob * restrict dst, configblob * restrict src)
{
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
    map_xfree(dst->filesystems.entries);
    dst->filesystems.entries = src->filesystems.entries;
    src->filesystems.entries = 0;
  }
  else
  {
    map_splice(dst->filesystems.entries, src->filesystems.entries);
  }
  dst->filesystems.merge_significant = dst->filesystems.entries->size;

  /* formula path */
  dst->formula.path.merge_significant = false;
  if(src->formula.merge_overwrite || src->formula.path.merge_overwrite || src->formula.path.dirs.merge_overwrite)
  {
    set_xfree(dst->formula.path.dirs.entries);
    dst->formula.path.dirs.entries = src->formula.path.dirs.entries;
    src->formula.path.dirs.entries = 0;
  }
  else
  {
    set_splice(dst->formula.path.dirs.entries, src->formula.path.dirs.entries);
  }
  dst->formula.path.dirs.merge_significant = dst->formula.path.dirs.entries->size;
  dst->formula.path.merge_significant |= dst->formula.path.dirs.merge_significant;

  empty = true;
  CFGCOPY(dst, src, formula, path.copy_from_env);
  dst->formula.path.merge_significant |= !empty;
  dst->formula.merge_significant = dst->formula.path.merge_significant;
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

void config_create(configblob ** restrict rv)
{
  configblob * cfg = 0;

  xmalloc(&cfg, sizeof(*cfg));

  llist_init_node(&cfg->walker.exclude.list);
  llist_init_node(&cfg->walker.include.list);

  map_createx(&cfg->filesystems.entries, 0, fse_free);
  set_createx(
      &cfg->formula.path.dirs.entries
    , 0
    , box_string_ht_hash_fn
    , box_string_ht_cmp_fn
    , (void*)box_free
  );

  *rv = cfg;
}

void config_xfree(configblob * restrict cfg)
{
  if(cfg)
  {
    box_free(refas(cfg->build.concurrency, bx));
    box_free(refas(cfg->workers.concurrency, bx));

    map_xfree(cfg->filesystems.entries);
    set_xfree(cfg->formula.path.dirs.entries);

    box_free(refas(cfg->formula.path.copy_from_env, bx));

    pattern_list_free(&cfg->walker.exclude.list);
    pattern_list_free(&cfg->walker.include.list);
  }
  wfree(cfg);
}

void config_ixfree(configblob ** restrict cfg)
{
  config_xfree(*cfg);
  *cfg = 0;
}

void config_writer_write(configblob * restrict cfg, value_writer * const restrict writer)
{
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
    value_writer_push_mapping(writer);
    value_writer_string(writer, "build");
    value_writer_push_set(writer);
    if(cfg->build.concurrency)
      value_writer_mapping_string_uint(writer, "concurrency", cfg->build.concurrency->v);
    value_writer_pop_set(writer);
    value_writer_pop_mapping(writer);
  }

  if(cfg->workers.merge_significant)
  {
    value_writer_push_mapping(writer);
    value_writer_string(writer, "workers");
    value_writer_push_set(writer);
    if(cfg->workers.concurrency)
      value_writer_mapping_string_uint(writer, "concurrency", cfg->workers.concurrency->v);
    value_writer_pop_set(writer);
    value_writer_pop_mapping(writer);
  }

  if(cfg->walker.merge_significant)
  {
    value_writer_push_mapping(writer);
    value_writer_string(writer, "walker");
    value_writer_push_set(writer);

    if(cfg->walker.include.merge_significant)
    {
      value_writer_push_mapping(writer);
      value_writer_string(writer, "include");
      value_writer_push_set(writer);

      llist_foreach(&cfg->walker.include.list, pat, lln) {
        N = narrator_fixed_init(&fixed, space, sizeof(space));
        pattern_say(pat, N);
        value_writer_bytes(writer, fixed.s, fixed.l);
      }

      value_writer_pop_set(writer);
      value_writer_pop_mapping(writer);
    }

    if(cfg->walker.exclude.merge_significant)
    {
      value_writer_push_mapping(writer);
      value_writer_string(writer, "exclude");
      value_writer_push_set(writer);

      llist_foreach(&cfg->walker.exclude.list, pat, lln) {
        N = narrator_fixed_init(&fixed, space, sizeof(space));
        pattern_say(pat, N);
        value_writer_bytes(writer, fixed.s, fixed.l);
      }

      value_writer_pop_set(writer);
      value_writer_pop_mapping(writer);
    }

    value_writer_pop_set(writer);
    value_writer_pop_mapping(writer);
  }

  if(cfg->filesystems.merge_significant)
  {
    value_writer_push_mapping(writer);
    value_writer_string(writer, "filesystems");
    value_writer_push_set(writer);

    for(x = 0; x < cfg->filesystems.entries->table_size; x++)
    {
      key = map_table_key(cfg->filesystems.entries, x);
      if(!key)
        continue;
      fsent = map_table_value(cfg->filesystems.entries, x);

      value_writer_push_mapping(writer);
      value_writer_string(writer, key);
        value_writer_push_set(writer);
          value_writer_mapping_string_string(writer, "invalidate", attrs16_name_byvalue(invalidate_attrs, INVALIDATE_OPT & fsent->invalidate->v));
        value_writer_pop_set(writer);
      value_writer_pop_mapping(writer);
    }

    value_writer_pop_set(writer);
    value_writer_pop_mapping(writer);
  }

  if(cfg->formula.merge_significant)
  {
    value_writer_push_mapping(writer);
    value_writer_string(writer, "formula");
    value_writer_push_set(writer);

    if(cfg->formula.path.merge_significant)
    {
      value_writer_push_mapping(writer);
      value_writer_string(writer, "path");
      value_writer_push_set(writer);
      if(cfg->formula.path.copy_from_env)
        value_writer_mapping_string_bool(writer, "copy-from-env", cfg->formula.path.copy_from_env->v);

      if(cfg->formula.path.dirs.merge_significant)
      {
        value_writer_push_mapping(writer);
        value_writer_string(writer, "dirs");
        value_writer_push_set(writer);
        for(x = 0; x < cfg->formula.path.dirs.entries->table_size; x++)
        {
          if(!(ent = set_table_get(cfg->formula.path.dirs.entries, x)))
            continue;

          value_writer_string(writer, ent->v);
        }
        value_writer_pop_set(writer);
        value_writer_pop_mapping(writer);
      }

      value_writer_pop_set(writer);
      value_writer_pop_mapping(writer);
    }

    value_writer_pop_set(writer);
  }
}

//
// public
//

void config_setup()
{
  config_parser_create(&parser_staging);
  config_create(&config_active);
}

void config_system_bootstrap()
{
  /* create the immutable config nodes, in precedence order */
  bootstrap_global_node(&user_config_node, g_args.user_config_path);
  bootstrap_global_node(&system_config_node, g_args.system_config_path);
}

void config_cleanup()
{
  config *v;
  llist *T;

  config_parser_xfree(parser_active);
  config_xfree(config_active);
  config_parser_xfree(parser_staging);
  config_xfree(config_staging);

  llist_foreach_safe(&config_list, v, vertex.owner, T) {
    config_dispose(v);
  }

  llist_foreach_safe(&config_freelist, v, vertex.owner, T) {
    free(v);
  }
}

void config_say(configblob * restrict cfg, narrator * restrict N)
{
  value_writer writer;

  value_writer_init(&writer);
  value_writer_open(&writer, N);

  config_writer_write(cfg, &writer);

  value_writer_close(&writer);
  value_writer_destroy(&writer);
}

void config_active_say(narrator * restrict N)
{
  config *cfg;

  narrator_xsays(N, "#\n");
  narrator_xsays(N, "# active config files in ascending order of precedence\n");
  narrator_xsays(N, "#\n");
  llist_foreach(&config_list, cfg, vertex.owner) {
    narrator_xsayf(N, "#  %.*s\n", (int)cfg->self_node_abspath_len, cfg->self_node_abspath);
  }
  narrator_xsays(N, "#\n");
  narrator_xsays(N, "\n");

  config_say(config_active, N);
}

static void reconfigure()
{
  channel *chan;
  fabipc_message *msg;
  int r;
  char error[512];

  // validate the new config
  r = 0;
  if(  (r = filesystem_reconfigure(config_staging, error, sizeof(error)))
    || (r = fsent_reconfigure(config_staging, error, sizeof(error)))
    || (r = walker_system_reconfigure(config_staging, error, sizeof(error)))
    || (r = build_reconfigure(config_staging, error, sizeof(error)))
    || (r = path_cache_reconfigure(config_staging, error, sizeof(error)))
  )
  {
    system_error = true;
    if(events_would(FABIPC_EVENT_SYSTEM_STATE, &chan, &msg)) {
      msg->code = EINVAL;
      msg->size = znloadw(msg->text, sizeof(msg->text), error, r);
      events_publish(chan, msg);
    }
  }
  else
  {
    // promote the staging config
    config_parser_ixfree(&parser_active);
    parser_active = parser_staging;
    parser_staging = 0;

    config_ixfree(&config_active);
    config_active = config_staging;
    config_staging = 0;

    // reconfigure subsystems
    filesystem_reconfigure(config_active, 0, 0);
    fsent_reconfigure(config_active, 0, 0);
    walker_system_reconfigure(config_active, 0, 0);
    build_reconfigure(config_active, 0, 0);
    path_cache_reconfigure(config_active, 0, 0);
  }
}

void config_system_reconcile(bool * restrict work, graph_invalidation_context * restrict invalidation)
{
  fsent *n;
  moria_vertex *dirv, *v;
  llist unused, *T;
  config *cfg;

  *work = false;

  /* bootstrap any config files in the ancestry of the project root */
  dirv = &g_project_root->vertex;
  while(dirv)
  {
    if((v = moria_vertex_downw(dirv, MMS(fsent_name_config))))
    {
      n = containerof(v, fsent, vertex);
      fsent_config_bootstrap(n);
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
    if((v = moria_vertex_downw(dirv, MMS(fsent_name_config))))
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
    return;
  }

  *work = true;

  /* config nodes not seen by the above loop are now unused */
  llist_foreach_safe(&unused, cfg, vertex.owner, T) {
    config_dispose(cfg);
  }

  /* all config files must be parsed any time config is reloaded */
  reparse();
  if(system_error) {
    return;
  }

  if(!config_compare(config_staging, config_active)) {
    reconfigure();
    if(system_error) {
      return;
    }
  }

  llist_foreach(&config_list, cfg, vertex.owner) {
    fsent_ok(cfg->self_node);
  }

  config_reconfigured = true;
}

void config_collate_stats(void *dst, size_t sz, config *cfg, bool reset, size_t *zp)
{
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
}
