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

#include <unistd.h>

#include "types.h"
#include "xapi.h"

#include "xapi/trace.h"
#include "xapi/calltree.h"
#include "xlinux/xstdlib.h"
#include "narrator.h"
#include "valyria/set.h"
#include "valyria/map.h"
#include "valyria/list.h"
#include "value/writer.h"

#include "config.internal.h"
#include "yyutil/box.h"
#include "CONFIG.errtab.h"
#include "config_parser.internal.h"
#include "filesystem.h"
#include "logging.h"
#include "build_thread.h"
#include "extern.h"
#include "params.h"
#include "fsent.h"
#include "path_cache.h"
#include "walker.h"
#include "args.h"
#include "zbuffer.h"

#include "common/snarf.h"
#include "macros.h"
#include "common/hash.h"
#include "common/attrs.h"

static configblob * config_staging;
static config_parser * parser_staging;
static configblob * config_active;
static config_parser * parser_active;
bool config_reconfigure_result;

static llist config_list = LLIST_INITIALIZER(config_list);          // active
static llist config_freelist = LLIST_INITIALIZER(config_freelist);  // free

/* these config nodes always exist */
static fsent *system_config_node;
static fsent *user_config_node;
static fsent *project_config_node;

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

static xapi stage(configblob ** cfg)
{
  enter;

  if(config_staging == 0)
  {
    config_staging = *cfg;
  }
  else
  {
    fatal(config_merge, config_staging, *cfg);
    fatal(config_xfree, *cfg);
  }

  *cfg = 0;

  finally : coda;
}

static void config_compare_build(config_base * restrict _new, config_base * restrict _old)
{
  struct config_build * new = containerof(_new, struct config_build, cb);
  struct config_build * old = containerof(_old, struct config_build, cb);

  if(!old ^ !new) {
    new->changed = true;
  } else if (old && old->concurrency && new && new->concurrency) {
    new->changed = box_cmp(&new->concurrency->bx, &old->concurrency->bx);
  } else {
    new->changed = false;
  }
}

static void config_compare_special(config_base * restrict _new, config_base * restrict _old)
{
  struct config_special * new = containerof(_new, struct config_special, cb);
  struct config_special * old = containerof(_old, struct config_special, cb);

  if(!old ^ !new) {
    new->changed = true;
    return;
  }
  if(!old) {
    new->changed = false;
    return;
  }

  new->changed = 0
         || box_cmp(refas(new->model, bx), refas(old->model, bx))
         || box_cmp(refas(new->module, bx), refas(old->module, bx))
         || box_cmp(refas(new->var, bx), refas(old->var, bx))
         || box_cmp(refas(new->formula_suffix, bx), refas(old->formula_suffix, bx))
         ;
}

static void config_compare_workers(config_base * restrict _new, config_base * restrict _old)
{
  struct config_workers * new = containerof(_new, struct config_workers, cb);
  struct config_workers * old = containerof(_old, struct config_workers, cb);

  if(!old ^ !new) {
    new->changed = true;
  } else if (old && old->concurrency && new && new->concurrency) {
    new->changed = box_cmp(&new->concurrency->bx, &old->concurrency->bx);
  } else {
    new->changed = false;
  }
}

static void config_compare_extern(config_base * restrict _new, config_base * restrict _old)
{
  struct config_extern_section * new = containerof(_new, struct config_extern_section, cb);
  struct config_extern_section * old = containerof(_old, struct config_extern_section, cb);

  new->changed = !old || !set_equal(new->entries, old->entries);
}

static void config_compare_filesystems(config_base * restrict _new, config_base * restrict _old)
{
  struct config_filesystems * new = containerof(_new, struct config_filesystems, cb);
  struct config_filesystems * old = containerof(_old, struct config_filesystems, cb);

  new->changed = !old || !map_equal(new->entries, old->entries);
}

static void config_compare_logging_exprs(config_base * restrict _new, config_base * restrict _old)
{
  struct config_logging_exprs * new = containerof(_new, struct config_logging_exprs, cb);
  struct config_logging_exprs * old = containerof(_old, struct config_logging_exprs, cb);

  new->changed = !old || !list_equal(new->items, old->items, 0);
}

static void config_compare_logging_section(config_base * restrict _new, config_base * restrict _old)
{
  struct config_logging_section * new = containerof(_new, struct config_logging_section, cb);
  struct config_logging_section * old = containerof(_old, struct config_logging_section, cb);

  config_compare_logging_exprs(&new->exprs.cb, refas(old, exprs.cb));

  new->changed = new->exprs.changed;
}

static void config_compare_logging(config_base * restrict _new, config_base * restrict _old)
{
  struct config_logging * new = containerof(_new, struct config_logging, cb);
  struct config_logging * old = containerof(_old, struct config_logging, cb);

  config_compare_logging_section(&new->console.cb, refas(old, console.cb));
  config_compare_logging_section(&new->logfile.cb, refas(old, logfile.cb));

  new->changed = new->console.changed || new->logfile.changed;
}

static void config_compare_formula_path(config_base * restrict _new, config_base * restrict _old)
{
  struct config_formula_path * new = containerof(_new, struct config_formula_path, cb);
  struct config_formula_path * old = containerof(_old, struct config_formula_path, cb);

  new->changed = !old || !set_equal(new->dirs.entries, old->dirs.entries);
  new->changed |= box_cmp(refas(new->copy_from_env, bx), refas2(old, copy_from_env, bx));
}

static void config_compare_formula(config_base * restrict _new, config_base * restrict _old)
{
  struct config_formula * new = containerof(_new, struct config_formula, cb);
  struct config_formula * old = containerof(_old, struct config_formula, cb);

  config_compare_formula_path(&new->path.cb, refas(old, path.cb));

  new->changed = 0
      || new->path.changed
      ;
}

static xapi parse(config_parser * restrict parser, const char * restrict path, char * restrict text, size_t text_len, configblob **cfg)
{
  enter;

  fatal(config_parser_parse, parser, text, text_len + 2, path, 0, cfg);

finally:
  xapi_infos("path", path);
coda;
}

static xapi reparse()
{
  enter;

  char * text = 0;
  size_t text_len;
  configblob * cfg = 0;

  // reset staging config
  fatal(config_parser_ixfree, &parser_staging);
  fatal(config_parser_create, &parser_staging);
  fatal(config_ixfree, &config_staging);

  // stage system-level config
  if(fsent_invalid_get(system_config_node))
  {
    fatal(usnarfs, &text, &text_len, system_config_node->self_config->self_node_abspath);
  }
  if(text)
  {
    logf(L_CONFIG, "staging system config @ %s", system_config_node->self_config->self_node_abspath);
    fatal(parse, parser_staging, system_config_node->self_config->self_node_abspath, text, text_len, &cfg);
    fatal(stage, &cfg);
    iwfree(&text);
  }
  else
  {
    logf(L_CONFIG, "no system config @ %s", system_config_node->self_config->self_node_abspath);
  }

  // stage user-level config
  if(fsent_invalid_get(user_config_node))
  {
    fatal(usnarfs, &text, &text_len, user_config_node->self_config->self_node_abspath);
  }
  if(text)
  {
    logf(L_CONFIG, "staging user config @ %s", user_config_node->self_config->self_node_abspath);
    fatal(parse, parser_staging, user_config_node->self_config->self_node_abspath, text, text_len, &cfg);
    fatal(stage, &cfg);
    iwfree(&text);
  }
  else
  {
    logf(L_CONFIG, "no user config @ %s", user_config_node->self_config->self_node_abspath);
  }

  // stage project-level config
  if(fsent_invalid_get(project_config_node))
  {
    fatal(usnarfs, &text, &text_len, project_config_node->self_config->self_node_abspath);
  }
  if(text)
  {
    logf(L_CONFIG, "staging project config @ %s", project_config_node->self_config->self_node_abspath);
    fatal(parse, parser_staging, project_config_node->self_config->self_node_abspath, text, text_len, &cfg);
    fatal(stage, &cfg);
    iwfree(&text);
  }
  else
  {
    logf(L_CONFIG, "no project config @ %s", project_config_node->self_config->self_node_abspath);
  }

  if(!config_staging)
  {
    fatal(config_create, &config_staging);
  }

finally:
  wfree(text);
  fatal(config_xfree, cfg);
coda;
}

static xapi config_alloc(config ** restrict vp, moria_graph * restrict g)
{
  enter;

  config *v;

  if((v = llist_shift(&config_freelist, typeof(*v), vertex.owner)) == 0)
  {
    fatal(xmalloc, &v, sizeof(*v));
  }

  moria_vertex_init(&v->vertex, g, VERTEX_CONFIG);

  llist_append(&config_list, v, vertex.owner);
  *vp = v;

  finally : coda;
}

static void config_dispose(config * restrict vp)
{
  llist_delete_node(&vp->vertex.owner);
  llist_append(&config_freelist, vp, vertex.owner);
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
  config_compare_build(&new->build.cb, refas(old, build.cb));
  config_compare_special(&new->special.cb, refas(old, special.cb));
  config_compare_workers(&new->workers.cb, refas(old, workers.cb));
  config_compare_extern(&new->extern_section.cb, refas(old, extern_section.cb));
  config_compare_filesystems(&new->filesystems.cb, refas(old, filesystems.cb));
  config_compare_formula(&new->formula.cb, refas(old, formula.cb));
  config_compare_logging(&new->logging.cb, refas(old, logging.cb));

  new->changed = 0
    || new->build.changed
    || new->workers.changed
    || new->extern_section.changed
    || new->filesystems.changed
    || new->formula.changed
    || new->logging.changed
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

  /* extern */
  if(src->extern_section.merge_overwrite)
  {
    fatal(set_xfree, dst->extern_section.entries);
    dst->extern_section.entries = src->extern_section.entries;
    src->extern_section.entries = 0;
  }
  else
  {
    fatal(set_splice, dst->extern_section.entries, src->extern_section.entries);
  }
  dst->extern_section.merge_significant = dst->extern_section.entries->size;

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

  /* logging */
  dst->logging.merge_significant = false;
  if(src->logging.merge_overwrite || src->logging.logfile.merge_overwrite || src->logging.logfile.exprs.merge_overwrite)
  {
    fatal(list_xfree, dst->logging.logfile.exprs.items);
    dst->logging.logfile.exprs.items = src->logging.logfile.exprs.items;
    src->logging.logfile.exprs.items = 0;
  }
  else
  {
    fatal(list_splice, dst->logging.logfile.exprs.items, dst->logging.logfile.exprs.items->size, src->logging.logfile.exprs.items, 0, -1);
  }
  dst->logging.logfile.exprs.merge_significant = dst->logging.logfile.exprs.items->size;
  dst->logging.logfile.merge_significant = dst->logging.logfile.exprs.merge_significant;
  dst->logging.merge_significant |= dst->logging.logfile.exprs.merge_significant;

  if(src->logging.merge_overwrite || src->logging.console.merge_overwrite || src->logging.console.exprs.merge_overwrite)
  {
    fatal(list_xfree, dst->logging.console.exprs.items);
    dst->logging.console.exprs.items = src->logging.console.exprs.items;
    src->logging.console.exprs.items = 0;
  }
  else
  {
    fatal(list_splice, dst->logging.console.exprs.items, dst->logging.console.exprs.items->size, src->logging.console.exprs.items, 0, -1);
  }
  dst->logging.console.exprs.merge_significant = dst->logging.console.exprs.items->size;
  dst->logging.console.merge_significant = dst->logging.console.exprs.merge_significant;
  dst->logging.merge_significant |= dst->logging.console.exprs.merge_significant;

  finally : coda;
}

xapi config_throw(box * restrict val)
{
  enter;

  xapi_info_pushf("location", "[%d,%d - %d,%d]", val->loc.f_lin, val->loc.f_col, val->loc.l_lin, val->loc.l_col);

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

  fatal(set_createx
    , &cfg->extern_section.entries
    , 0
    , box_string_ht_hash_fn
    , box_string_ht_cmp_fn
    , (void*)box_free
    , 0
  );
  fatal(map_createx, &cfg->filesystems.entries, 0, fse_free, 0);
  fatal(list_createx, &cfg->logging.logfile.exprs.items, 0, 0, box_free, 0);
  fatal(list_createx, &cfg->logging.console.exprs.items, 0, 0, box_free, 0);
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
    box_free(refas(cfg->special.module, bx));
    box_free(refas(cfg->special.model, bx));
    box_free(refas(cfg->special.var, bx));
    box_free(refas(cfg->special.formula_suffix, bx));
    box_free(refas(cfg->workers.concurrency, bx));

    fatal(set_xfree, cfg->extern_section.entries);
    fatal(map_xfree, cfg->filesystems.entries);
    fatal(set_xfree, cfg->formula.path.dirs.entries);

    box_free(refas(cfg->formula.path.copy_from_env, bx));

    fatal(list_xfree, cfg->logging.logfile.exprs.items);
    fatal(list_xfree, cfg->logging.console.exprs.items);
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
  const box_string *item;
  const char *key;
  const struct config_filesystem_entry *fsent;

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

  if(cfg->extern_section.merge_significant)
  {
    fatal(value_writer_push_mapping, writer);
    fatal(value_writer_string, writer, "extern");
    fatal(value_writer_push_set, writer);

    for(x = 0; x < cfg->extern_section.entries->table_size; x++)
    {
      if(!(ent = set_table_get(cfg->extern_section.entries, x)))
        continue;

      fatal(value_writer_string, writer, ent->v);
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

  if(cfg->logging.merge_significant)
  {
    fatal(value_writer_push_mapping, writer);
    fatal(value_writer_string, writer, "logging");
    fatal(value_writer_push_set, writer);

    if(cfg->logging.console.merge_significant)
    {
      fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "console");
      fatal(value_writer_push_set, writer);

      if(cfg->logging.console.exprs.merge_significant)
      {
        fatal(value_writer_push_mapping, writer);
        fatal(value_writer_string, writer, "exprs");
        fatal(value_writer_push_list, writer);
        for(x = 0; x < cfg->logging.console.exprs.items->size; x++)
        {
          item = list_get(cfg->logging.console.exprs.items, x);
          fatal(value_writer_string, writer, item->v);
        }
        fatal(value_writer_pop_list, writer);
        fatal(value_writer_pop_mapping, writer);
      }

      fatal(value_writer_pop_set, writer);
      fatal(value_writer_pop_mapping, writer);
    }

    if(cfg->logging.logfile.merge_significant)
    {
      fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "logfile");
      fatal(value_writer_push_set, writer);

      if(cfg->logging.logfile.exprs.merge_significant)
      {
        fatal(value_writer_push_mapping, writer);
        fatal(value_writer_string, writer, "exprs");
        fatal(value_writer_push_list, writer);
        for(x = 0; x < cfg->logging.logfile.exprs.items->size; x++)
        {
          item = list_get(cfg->logging.logfile.exprs.items, x);
          fatal(value_writer_string, writer, item->v);
        }
        fatal(value_writer_pop_list, writer);
        fatal(value_writer_pop_mapping, writer);
      }

      fatal(value_writer_pop_set, writer);
      fatal(value_writer_pop_mapping, writer);
    }

    fatal(value_writer_pop_set, writer);
    fatal(value_writer_pop_mapping, writer);
  }

  if(cfg->special.merge_significant)
  {
    fatal(value_writer_push_mapping, writer);
    fatal(value_writer_string, writer, "special");
    fatal(value_writer_push_set, writer);
    if(cfg->special.module) {
      fatal(value_writer_mapping_string_string, writer, "module", cfg->special.module->v);
    }
    if(cfg->special.model) {
      fatal(value_writer_mapping_string_string, writer, "model", cfg->special.model->v);
    }
    if(cfg->special.var) {
      fatal(value_writer_mapping_string_string, writer, "var", cfg->special.var->v);
    }
    if(cfg->special.formula_suffix) {
      fatal(value_writer_mapping_string_string, writer, "formula-suffix", cfg->special.formula_suffix->v);
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

  fatal(config_say, config_active, N);

  finally : coda;
}

static xapi reconfigure(bool * restrict reconciled)
{
  enter;

  char trace[4096 << 1];
  size_t trace_len;

  // validate the new config
  xapi exit = 0;
  if(  (exit = invoke(logging_reconfigure, config_staging, true))
    || (exit = invoke(filesystem_reconfigure, config_staging, true))
    || (exit = invoke(fsent_reconfigure, config_staging, true))
    || (exit = invoke(extern_reconfigure, config_staging, true))
    || (exit = invoke(build_thread_reconfigure, config_staging, true))
    || (exit = invoke(path_cache_reconfigure, config_staging, true))
  )
  {
    if(xapi_exit_errtab(exit) != perrtab_CONFIG)
      fail(0);

#if DEBUG || DEVEL || XAPI
    trace_len = xapi_trace_full(trace, sizeof(trace), 0);
#else
    trace_len = xapi_trace_pithy(trace, sizeof(trace), 0);
#endif
    xapi_calltree_unwind();

    /* write error to stderr */
    write(2, trace, trace_len);
    config_reconfigure_result = false;
    *reconciled = false;
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
    fatal(logging_reconfigure, config_active, false);
    fatal(filesystem_reconfigure, config_active, false);
    fatal(fsent_reconfigure, config_active, false);
    fatal(extern_reconfigure, config_active, false);
    fatal(build_thread_reconfigure, config_active, false);
    fatal(path_cache_reconfigure, config_active, false);
    config_reconfigure_result = true;

    fatal(fsent_ok, system_config_node);
    fatal(fsent_ok, user_config_node);
    fatal(fsent_ok, project_config_node);
  }

  finally : coda;
}

xapi config_system_reconcile(int walk_id, graph_invalidation_context * restrict invalidation, bool * restrict reconciled)
{
  enter;

  fatal(walker_ascend, system_config_node, walk_id, invalidation);
  fatal(walker_ascend, user_config_node, walk_id, invalidation);
  fatal(walker_ascend, project_config_node, walk_id, invalidation);

  if(!fsent_invalid_get(system_config_node) && !fsent_invalid_get(user_config_node) && !fsent_invalid_get(project_config_node)) {
    goto XAPI_FINALIZE;
  }

  fatal(reparse);
  if(config_compare(config_staging, config_active)) {
    goto XAPI_FINALIZE;
  }

  fatal(reconfigure, reconciled);

  finally : coda;
}

static xapi bootstrap_node(fsent ** restrict np, const char * restrict pathspec)
{
  enter;

  graph_invalidation_context invalidation = { 0 };
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
  (*np)->self_config = cfg;
  cfg->self_node = *np;

finally:
  graph_invalidation_end(&invalidation);
coda;
}

xapi config_system_bootstrap()
{
  enter;

  /* create the immutable config nodes */
  fatal(bootstrap_node, &system_config_node, g_args.system_config_path);
  fatal(bootstrap_node, &user_config_node, g_args.user_config_path);
  fatal(bootstrap_node, &project_config_node, g_args.project_config_path);

  finally : coda;
}
