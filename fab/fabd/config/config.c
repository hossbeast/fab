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

#include <string.h>

#include "types.h"
#include "xapi.h"

#include "xapi/trace.h"
#include "xapi/calltree.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"
#include "narrator.h"
#include "logger.h"
#include "valyria/set.h"
#include "valyria/map.h"
#include "valyria/pstring.h"
#include "valyria/list.h"
#include "value.h"
#include "value/writer.h"
#include "value/merge.h"

#include "config.internal.h"
#include "box.h"
#include "CONFIG.errtab.h"
#include "config_parser.internal.h"
#include "filesystem.h"
#include "logging.h"
#include "build_thread.h"
#include "extern.h"
#include "params.h"
#include "node.h"
#include "var.h"

#include "common/snarf.h"
#include "macros.h"
#include "common/hash.h"
#include "common/attrs.h"

#define SYSTEM_CONFIG_PATH    "/etc/fabconfig"  // absolute
#define USER_CONFIG_PATH      ".fab/config"     // relative to $HOME
#define PROJECT_CONFIG_PATH   ".fab/config"     // relative to project dir

static config * config_staging;
static config_parser * parser_staging;
static config * config_active;
static config_parser * parser_active;

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

static xapi stage(config ** cfg)
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

  new->changed = box_cmp(refas(new->concurrency, bx), old ? old->concurrency ? &old->concurrency->bx : 0 : 0);
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

static void config_compare_var(config_base * restrict _new, config_base * restrict _old)
{
  struct config_var * new = containerof(_new, struct config_var, cb);
  struct config_var * old = containerof(_old, struct config_var, cb);

  if((new->changed = INTCMP(old, new)))
    return;

  if(new)
    new->changed = value_cmp(new->value, old->value);
}

static void config_compare_formula_show_settings(config_base * restrict _new, config_base * restrict _old)
{
  struct config_formula_show_settings * new = containerof(_new, struct config_formula_show_settings, cb);
  struct config_formula_show_settings * old = containerof(_old, struct config_formula_show_settings, cb);

  new->changed =
       box_cmp(refas(new->show_arguments, bx), refas2(old, show_arguments, bx))
    || box_cmp(refas(new->show_path, bx), refas2(old, show_path, bx))
    || box_cmp(refas(new->show_cwd, bx), refas2(old, show_cwd, bx))
    || box_cmp(refas(new->show_command, bx), refas2(old, show_command, bx))
    || box_cmp(refas(new->show_sources, bx), refas2(old, show_sources, bx))
    || box_cmp(refas(new->show_targets, bx), refas2(old, show_targets, bx))
    || box_cmp(refas(new->show_environment, bx), refas2(old, show_environment, bx))
    || box_cmp(refas(new->show_status, bx), refas(old->show_status, bx))
    || box_cmp(refas(new->show_stdout, bx), refas(old->show_stdout, bx))
    || box_cmp(refas(new->show_stdout_limit_bytes, bx), refas(old->show_stdout_limit_bytes, bx))
    || box_cmp(refas(new->show_stdout_limit_lines, bx), refas(old->show_stdout_limit_lines, bx))
    || box_cmp(refas(new->show_stderr, bx), refas(old->show_stderr, bx))
    || box_cmp(refas(new->show_stderr_limit_bytes, bx), refas(old->show_stderr_limit_bytes, bx))
    || box_cmp(refas(new->show_stderr_limit_lines, bx), refas(old->show_stderr_limit_lines, bx))
    || box_cmp(refas(new->show_auxout, bx), refas(old->show_auxout, bx))
    || box_cmp(refas(new->show_auxout_limit_bytes, bx), refas(old->show_auxout_limit_bytes, bx))
    || box_cmp(refas(new->show_auxout_limit_lines, bx), refas(old->show_auxout_limit_lines, bx))
    ;
}

static void config_compare_formula(config_base * restrict _new, config_base * restrict _old)
{
  struct config_formula * new = containerof(_new, struct config_formula, cb);
  struct config_formula * old = containerof(_old, struct config_formula, cb);

  config_compare_formula_show_settings(&new->success.cb, refas(old, success.cb));
  config_compare_formula_show_settings(&new->error.cb, refas(old, error.cb));

  new->changed =
         new->success.changed
      || new->error.changed
      || box_cmp(refas(new->capture_stdout, bx), refas(old->capture_stdout, bx))
      || box_cmp(refas(new->stdout_buffer_size, bx), refas(old->stdout_buffer_size, bx))
      || box_cmp(refas(new->capture_stderr, bx), refas(old->capture_stderr, bx))
      || box_cmp(refas(new->stderr_buffer_size, bx), refas(old->stderr_buffer_size, bx))
      || box_cmp(refas(new->capture_auxout, bx), refas(old->capture_auxout, bx))
      || box_cmp(refas(new->auxout_buffer_size, bx), refas(old->auxout_buffer_size, bx))
      ;
}

//
// internal
//

/// config_compare
//
// SUMMARY
//  mark sections of a new config struct as different from those of another config struct
//
void config_compare(config * restrict new, config * restrict old)
{
  config_compare_build(&new->build.cb, refas(old, build.cb));
  config_compare_extern(&new->extern_section.cb, refas(old, extern_section.cb));
  config_compare_filesystems(&new->filesystems.cb, refas(old, filesystems.cb));
  config_compare_formula(&new->formula.cb, refas(old, formula.cb));
  config_compare_logging(&new->logging.cb, refas(old, logging.cb));
  config_compare_var(&new->var.cb, refas(old, var.cb));
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

xapi config_merge(config * restrict dst, config * restrict src)
{
  enter;

  bool empty;

  /* build */
  empty = true;
  CFGCOPY(dst, src, build, concurrency);
  dst->build.merge_significant = !empty;

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
  dst->filesystems.merge_significant = src->filesystems.entries->size;

  /* formula */
  empty = true;
  CFGCOPY(dst, src, formula, success.show_arguments);
  CFGCOPY(dst, src, formula, success.show_path);
  CFGCOPY(dst, src, formula, success.show_cwd);
  CFGCOPY(dst, src, formula, success.show_command);
  CFGCOPY(dst, src, formula, success.show_sources);
  CFGCOPY(dst, src, formula, success.show_targets);
  CFGCOPY(dst, src, formula, success.show_environment);
  CFGCOPY(dst, src, formula, success.show_status);
  CFGCOPY(dst, src, formula, success.show_stdout);
  CFGCOPY(dst, src, formula, success.show_stdout_limit_bytes);
  CFGCOPY(dst, src, formula, success.show_stdout_limit_lines);
  CFGCOPY(dst, src, formula, success.show_stderr);
  CFGCOPY(dst, src, formula, success.show_stderr_limit_bytes);
  CFGCOPY(dst, src, formula, success.show_stderr_limit_lines);
  CFGCOPY(dst, src, formula, success.show_auxout);
  CFGCOPY(dst, src, formula, success.show_auxout_limit_bytes);
  CFGCOPY(dst, src, formula, success.show_auxout_limit_lines);
  dst->formula.success.merge_significant = !empty;

  empty = true;
  CFGCOPY(dst, src, formula, error.show_arguments);
  CFGCOPY(dst, src, formula, error.show_path);
  CFGCOPY(dst, src, formula, error.show_cwd);
  CFGCOPY(dst, src, formula, error.show_command);
  CFGCOPY(dst, src, formula, error.show_sources);
  CFGCOPY(dst, src, formula, error.show_targets);
  CFGCOPY(dst, src, formula, error.show_environment);
  CFGCOPY(dst, src, formula, error.show_status);
  CFGCOPY(dst, src, formula, error.show_stdout);
  CFGCOPY(dst, src, formula, error.show_stdout_limit_bytes);
  CFGCOPY(dst, src, formula, error.show_stdout_limit_lines);
  CFGCOPY(dst, src, formula, error.show_stderr);
  CFGCOPY(dst, src, formula, error.show_stderr_limit_bytes);
  CFGCOPY(dst, src, formula, error.show_stderr_limit_lines);
  CFGCOPY(dst, src, formula, error.show_auxout);
  CFGCOPY(dst, src, formula, error.show_auxout_limit_bytes);
  CFGCOPY(dst, src, formula, error.show_auxout_limit_lines);
  dst->formula.error.merge_significant = !empty;

  empty = true;
  CFGCOPY(dst, src, formula, capture_stdout);
  CFGCOPY(dst, src, formula, stdout_buffer_size);
  CFGCOPY(dst, src, formula, capture_stderr);
  CFGCOPY(dst, src, formula, stderr_buffer_size);
  CFGCOPY(dst, src, formula, capture_auxout);
  CFGCOPY(dst, src, formula, auxout_buffer_size);
  dst->formula.merge_significant = !empty;

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

  /* var */
  fatal(value_merge, parser_staging->value_parser, &dst->var.value, src->var.value, src->var.merge_overwrite ? VALUE_MERGE_SET : 0);
  dst->var.merge_significant = dst->var.value != NULL;

  finally : coda;
}

xapi config_throw(box * restrict val)
{
  enter;

  xapi_info_pushf("location", "[%d,%d - %d,%d]", val->loc.f_lin, val->loc.f_col, val->loc.l_lin, val->loc.l_col);

  if(val->loc.fname)
    xapi_info_pushs("file", val->loc.fname);

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

xapi config_create(config ** restrict rv)
{
  enter;

  config * cfg = 0;

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

  *rv = cfg;
  cfg = 0;

finally:
  fatal(config_xfree, cfg);
coda;
}

xapi config_xfree(config * restrict cfg)
{
  enter;

  if(cfg)
  {
    box_free(refas(cfg->build.concurrency, bx));
    box_free(refas(cfg->formula.capture_stdout, bx));
    box_free(refas(cfg->formula.stdout_buffer_size, bx));
    box_free(refas(cfg->formula.capture_stderr, bx));
    box_free(refas(cfg->formula.stderr_buffer_size, bx));
    box_free(refas(cfg->formula.capture_auxout, bx));
    box_free(refas(cfg->formula.auxout_buffer_size, bx));

    fatal(set_xfree, cfg->extern_section.entries);
    fatal(map_xfree, cfg->filesystems.entries);

    box_free(refas(cfg->formula.success.show_arguments, bx));
    box_free(refas(cfg->formula.success.show_path, bx));
    box_free(refas(cfg->formula.success.show_cwd, bx));
    box_free(refas(cfg->formula.success.show_command, bx));
    box_free(refas(cfg->formula.success.show_sources, bx));
    box_free(refas(cfg->formula.success.show_targets, bx));
    box_free(refas(cfg->formula.success.show_environment, bx));
    box_free(refas(cfg->formula.success.show_status, bx));
    box_free(refas(cfg->formula.success.show_stdout, bx));
    box_free(refas(cfg->formula.success.show_stdout_limit_bytes, bx));
    box_free(refas(cfg->formula.success.show_stdout_limit_lines, bx));
    box_free(refas(cfg->formula.success.show_stderr, bx));
    box_free(refas(cfg->formula.success.show_stderr_limit_bytes, bx));
    box_free(refas(cfg->formula.success.show_stderr_limit_lines, bx));
    box_free(refas(cfg->formula.success.show_auxout, bx));
    box_free(refas(cfg->formula.success.show_auxout_limit_bytes, bx));
    box_free(refas(cfg->formula.success.show_auxout_limit_lines, bx));

    box_free(refas(cfg->formula.error.show_arguments, bx));
    box_free(refas(cfg->formula.error.show_path, bx));
    box_free(refas(cfg->formula.error.show_cwd, bx));
    box_free(refas(cfg->formula.error.show_command, bx));
    box_free(refas(cfg->formula.error.show_sources, bx));
    box_free(refas(cfg->formula.error.show_targets, bx));
    box_free(refas(cfg->formula.error.show_environment, bx));
    box_free(refas(cfg->formula.error.show_status, bx));
    box_free(refas(cfg->formula.error.show_stdout, bx));
    box_free(refas(cfg->formula.error.show_stdout_limit_bytes, bx));
    box_free(refas(cfg->formula.error.show_stdout_limit_lines, bx));
    box_free(refas(cfg->formula.error.show_stderr, bx));
    box_free(refas(cfg->formula.error.show_stderr_limit_bytes, bx));
    box_free(refas(cfg->formula.error.show_stderr_limit_lines, bx));
    box_free(refas(cfg->formula.error.show_auxout, bx));
    box_free(refas(cfg->formula.error.show_auxout_limit_bytes, bx));
    box_free(refas(cfg->formula.error.show_auxout_limit_lines, bx));

    fatal(list_xfree, cfg->logging.logfile.exprs.items);
    fatal(list_xfree, cfg->logging.console.exprs.items);
  }
  wfree(cfg);

  finally : coda;
}

xapi config_ixfree(config ** restrict cfg)
{
  enter;

  fatal(config_xfree, *cfg);
  *cfg = 0;

  finally : coda;
}

xapi config_writer_write(config * restrict cfg, value_writer * const restrict writer)
{
  enter;

  int x;

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
  if(cfg->extern_section.merge_significant)
  {
    fatal(value_writer_push_mapping, writer);
    fatal(value_writer_string, writer, "extern");
    fatal(value_writer_push_set, writer);

    for(x = 0; x < cfg->extern_section.entries->table_size; x++)
    {
      const box_string * ent;
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
      const char * key = map_table_key(cfg->filesystems.entries, x);
      if(!key)
        continue;
      const struct config_filesystem_entry * ent = map_table_value(cfg->filesystems.entries, x);

      fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, key);
        fatal(value_writer_push_set, writer);
          fatal(value_writer_mapping_string_string, writer, "invalidate", attrs16_name_byvalue(invalidate_attrs, INVALIDATE_OPT & ent->invalidate->v));
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

    if(cfg->formula.capture_stdout)
    {
      const char * name = attrs32_name_byvalue(stream_part_attrs, STREAM_PART_OPT & cfg->formula.capture_stdout->v);
      fatal(value_writer_mapping_string_string, writer, "capture-stdout", name);
    }
    if(cfg->formula.stdout_buffer_size)
    {
      fatal(value_writer_mapping_string_uint, writer, "stdout-buffer-size", cfg->formula.stdout_buffer_size->v);
    }
    if(cfg->formula.capture_stderr)
    {
      const char * name = attrs32_name_byvalue(stream_part_attrs, STREAM_PART_OPT & cfg->formula.capture_stderr->v);
      fatal(value_writer_mapping_string_string, writer, "capture-stderr", name);
    }
    if(cfg->formula.stderr_buffer_size)
    {
      fatal(value_writer_mapping_string_uint, writer, "stderr-buffer-size", cfg->formula.stderr_buffer_size->v);
    }
    if(cfg->formula.capture_auxout)
    {
      const char * name = attrs32_name_byvalue(stream_part_attrs, STREAM_PART_OPT & cfg->formula.capture_auxout->v);
      fatal(value_writer_mapping_string_string, writer, "capture-auxout", name);
    }
    if(cfg->formula.auxout_buffer_size)
    {
      fatal(value_writer_mapping_string_uint, writer, "auxout-buffer-size", cfg->formula.auxout_buffer_size->v);
    }
    if(cfg->formula.success.merge_significant)
    {
      fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "success");
      fatal(value_writer_push_set, writer);

      if(cfg->formula.success.show_arguments)
        fatal(value_writer_mapping_string_bool, writer, "show-arguments", cfg->formula.success.show_arguments->v);
      if(cfg->formula.success.show_path)
        fatal(value_writer_mapping_string_bool, writer, "show-path", cfg->formula.success.show_path->v);
      if(cfg->formula.success.show_cwd)
        fatal(value_writer_mapping_string_bool, writer, "show-cwd", cfg->formula.success.show_cwd->v);
      if(cfg->formula.success.show_command)
        fatal(value_writer_mapping_string_bool, writer, "show-command", cfg->formula.success.show_command->v);
      if(cfg->formula.success.show_sources)
        fatal(value_writer_mapping_string_bool, writer, "show-sources", cfg->formula.success.show_sources->v);
      if(cfg->formula.success.show_targets)
        fatal(value_writer_mapping_string_bool, writer, "show-targets", cfg->formula.success.show_targets->v);
      if(cfg->formula.success.show_environment)
        fatal(value_writer_mapping_string_bool, writer, "show-environment", cfg->formula.success.show_environment->v);
      if(cfg->formula.success.show_status)
        fatal(value_writer_mapping_string_bool, writer, "show-status", cfg->formula.success.show_status->v);
      if(cfg->formula.success.show_stdout)
        fatal(value_writer_mapping_string_bool, writer, "show-stdout", cfg->formula.success.show_stdout->v);
      if(cfg->formula.success.show_stdout_limit_bytes)
        fatal(value_writer_mapping_string_uint, writer, "show-stdout-limit-bytes", cfg->formula.success.show_stdout_limit_bytes->v);
      if(cfg->formula.success.show_stdout_limit_lines)
        fatal(value_writer_mapping_string_uint, writer, "show-stdout-limit-lines", cfg->formula.success.show_stdout_limit_lines->v);
      if(cfg->formula.success.show_stderr)
        fatal(value_writer_mapping_string_bool, writer, "show-stderr", cfg->formula.success.show_stderr->v);
      if(cfg->formula.success.show_stderr_limit_bytes)
        fatal(value_writer_mapping_string_uint, writer, "show-stderr-limit-bytes", cfg->formula.success.show_stderr_limit_bytes->v);
      if(cfg->formula.success.show_stderr_limit_lines)
        fatal(value_writer_mapping_string_uint, writer, "show-stderr-limit-lines", cfg->formula.success.show_stderr_limit_lines->v);
      if(cfg->formula.success.show_auxout)
        fatal(value_writer_mapping_string_bool, writer, "show-auxout", cfg->formula.success.show_auxout->v);
      if(cfg->formula.success.show_auxout_limit_bytes)
        fatal(value_writer_mapping_string_uint, writer, "show-auxout-limit-bytes", cfg->formula.success.show_auxout_limit_bytes->v);
      if(cfg->formula.success.show_auxout_limit_lines)
        fatal(value_writer_mapping_string_uint, writer, "show-auxout-limit-lines", cfg->formula.success.show_auxout_limit_lines->v);

      fatal(value_writer_pop_set, writer);
      fatal(value_writer_pop_mapping, writer);
    }
    if(cfg->formula.error.merge_significant)
    {
      fatal(value_writer_push_mapping, writer);
      fatal(value_writer_string, writer, "error");
      fatal(value_writer_push_set, writer);

      if(cfg->formula.error.show_arguments)
        fatal(value_writer_mapping_string_bool, writer, "show-arguments", cfg->formula.error.show_arguments->v);
      if(cfg->formula.error.show_path)
        fatal(value_writer_mapping_string_bool, writer, "show-path", cfg->formula.error.show_path->v);
      if(cfg->formula.error.show_cwd)
        fatal(value_writer_mapping_string_bool, writer, "show-cwd", cfg->formula.error.show_cwd->v);
      if(cfg->formula.error.show_command)
        fatal(value_writer_mapping_string_bool, writer, "show-command", cfg->formula.error.show_command->v);
      if(cfg->formula.error.show_sources)
        fatal(value_writer_mapping_string_bool, writer, "show-sources", cfg->formula.error.show_sources->v);
      if(cfg->formula.error.show_targets)
        fatal(value_writer_mapping_string_bool, writer, "show-targets", cfg->formula.error.show_targets->v);
      if(cfg->formula.error.show_environment)
        fatal(value_writer_mapping_string_bool, writer, "show-environment", cfg->formula.error.show_environment->v);
      if(cfg->formula.error.show_status)
        fatal(value_writer_mapping_string_bool, writer, "show-status", cfg->formula.error.show_status->v);
      if(cfg->formula.error.show_stdout)
        fatal(value_writer_mapping_string_bool, writer, "show-stdout", cfg->formula.error.show_stdout->v);
      if(cfg->formula.error.show_stdout_limit_bytes)
        fatal(value_writer_mapping_string_uint, writer, "show-stdout-limit-bytes", cfg->formula.error.show_stdout_limit_bytes->v);
      if(cfg->formula.error.show_stdout_limit_lines)
        fatal(value_writer_mapping_string_uint, writer, "show-stdout-limit-lines", cfg->formula.error.show_stdout_limit_lines->v);
      if(cfg->formula.error.show_stderr)
        fatal(value_writer_mapping_string_bool, writer, "show-stderr", cfg->formula.error.show_stderr->v);
      if(cfg->formula.error.show_stderr_limit_bytes)
        fatal(value_writer_mapping_string_uint, writer, "show-stderr-limit-bytes", cfg->formula.error.show_stderr_limit_bytes->v);
      if(cfg->formula.error.show_stderr_limit_lines)
        fatal(value_writer_mapping_string_uint, writer, "show-stderr-limit-lines", cfg->formula.error.show_stderr_limit_lines->v);
      if(cfg->formula.error.show_auxout)
        fatal(value_writer_mapping_string_bool, writer, "show-auxout", cfg->formula.error.show_auxout->v);
      if(cfg->formula.error.show_auxout_limit_bytes)
        fatal(value_writer_mapping_string_uint, writer, "show-auxout-limit-bytes", cfg->formula.error.show_auxout_limit_bytes->v);
      if(cfg->formula.error.show_auxout_limit_lines)
        fatal(value_writer_mapping_string_uint, writer, "show-auxout-limit-lines", cfg->formula.error.show_auxout_limit_lines->v);

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
          const box_string * item = list_get(cfg->logging.console.exprs.items, x);
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
          const box_string * item = list_get(cfg->logging.logfile.exprs.items, x);
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
  if(cfg->var.merge_significant)
  {
    fatal(value_writer_push_mapping, writer);
    fatal(value_writer_string, writer, "var");
    fatal(value_writer_value, writer, cfg->var.value);
    fatal(value_writer_pop_mapping, writer);
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

  fatal(config_parser_xfree, parser_active);
  fatal(config_xfree, config_active);
  fatal(config_parser_xfree, parser_staging);
  fatal(config_xfree, config_staging);

  finally : coda;
}

xapi config_report()
{
  enter;

  finally : coda;
}

xapi config_begin_staging()
{
  enter;

  char * text = 0;
  size_t text_len = 0;
  config * cfg = 0;

  // reset staging config
  fatal(config_parser_ixfree, &parser_staging);
  fatal(config_parser_create, &parser_staging);
  fatal(config_ixfree, &config_staging);

  // system-level config
  fatal(usnarfs, &text, &text_len, SYSTEM_CONFIG_PATH);
  if(text)
  {
    logf(L_CONFIG, "staging global config @ %s", SYSTEM_CONFIG_PATH);
    fatal(config_parser_parse, parser_staging, text, text_len + 2, SYSTEM_CONFIG_PATH, 0, &cfg);
    fatal(stage, &cfg);
  }
  else
  {
    logf(L_CONFIG, "no global config @ %s", SYSTEM_CONFIG_PATH);
  }

  // user-level config
  iwfree(&text);
  fatal(usnarff, &text, &text_len, "%s/%s", g_params.homedir, USER_CONFIG_PATH);
  if(text)
  {
    logf(L_CONFIG, "staging user config @ %s/%s", g_params.homedir, USER_CONFIG_PATH);
    fatal(config_parser_parse, parser_staging, text, text_len + 2, USER_CONFIG_PATH, 0, &cfg);
    fatal(stage, &cfg);
  }
  else
  {
    logf(L_CONFIG, "no user config @ %s/%s", g_params.homedir, USER_CONFIG_PATH);
  }

  // project-level config
  iwfree(&text);
  fatal(usnarfats, &text, &text_len, g_params.proj_dirfd, PROJECT_CONFIG_PATH);
  if(text)
  {
    logf(L_CONFIG, "staging project config @ %s/%s", g_params.proj_dir, PROJECT_CONFIG_PATH);
    fatal(config_parser_parse, parser_staging, text, text_len + 2, PROJECT_CONFIG_PATH, 0, &cfg);
    fatal(stage, &cfg);
  }
  else
  {
    logf(L_CONFIG, "no project config @ %s/%s", g_params.proj_dir, PROJECT_CONFIG_PATH);
  }

finally:
  wfree(text);
  fatal(config_xfree, cfg);
coda;
}

xapi config_stage(config ** restrict cfg)
{
  enter;

  logs(L_CONFIG, "staging config");
  fatal(stage, cfg);

  finally : coda;
}

xapi config_reconfigure()
{
  enter;

  char trace[4096];
  narrator * N;

  config_compare(config_staging, config_active);

  if(log_would(L_CONFIG))
  {
    fatal(log_start, L_CONFIG, &N);
    xsays("effective config\n");
    fatal(config_say, config_staging, N);
    fatal(log_finish);
  }

  // validate the new config
  xapi exit = 0;
  if(  (exit = invoke(logging_reconfigure, config_staging, true))
    || (exit = invoke(filesystem_reconfigure, config_staging, true))
    || (exit = invoke(node_reconfigure, config_staging, true))
    || (exit = invoke(extern_reconfigure, config_staging, true))
    || (exit = invoke(build_thread_reconfigure, config_staging, true))
    || (exit = invoke(var_reconfigure, config_staging, true))
  )
  {
    if(xapi_exit_errtab(exit) != perrtab_CONFIG)
      fail(0);

#if DEBUG || DEVEL || XAPI
    xapi_trace_full(trace, sizeof(trace), 0);
#else
    xapi_trace_pithy(trace, sizeof(trace), 0);
#endif
    xapi_calltree_unwind();

    xlogs(L_WARN, L_NOCATEGORY, trace);
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
    fatal(config_report);
    fatal(logging_reconfigure, config_active, false);
    fatal(filesystem_reconfigure, config_active, false);
    fatal(node_reconfigure, config_active, false);
    fatal(extern_reconfigure, config_active, false);
    fatal(build_thread_reconfigure, config_active, false);
    fatal(var_reconfigure, config_active, false);
  }

  finally : coda;
}

xapi config_say(config * restrict cfg, narrator * restrict N)
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
