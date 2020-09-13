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
#include "yyutil/box.h"
#include "common/snarf.h"
#include "common/hash.h"
#include "common/attrs.h"

#include "config.internal.h"
#include "CONFIG.errtab.h"
#include "config_parser.internal.h"
#include "params.h"
#include "logging.h"
#include "build.h"

#include "macros.h"

#if DEVEL
#define SYSTEM_CONFIG_PATH    "/etc/bam/client_config+devel" // absolute
#define USER_CONFIG_PATH      ".bam/client_config+devel"     // relative to $HOME
#define PROJECT_CONFIG_PATH   ".bam/client_config+devel"     // relative to project dir
#else
#define SYSTEM_CONFIG_PATH    "/etc/bam/client_config"  // absolute
#define USER_CONFIG_PATH      ".bam/client_config"      // relative to $HOME
#define PROJECT_CONFIG_PATH   ".bam/client_config"      // relative to project dir
#endif

static config * config_active;
static config_parser * parser_active;

attrs32 * stream_part_attrs = (attrs32[]) {{
#define STREAM_PART_DEF(x, n, r, y) + 1
    num : 0 STREAM_PART_TABLE
  , members : (member32[]) {
#undef STREAM_PART_DEF
#define STREAM_PART_DEF(x, n, r, y) { name : #n, value : UINT32_C(y), range : UINT32_C(r) },
STREAM_PART_TABLE
  }
}};

//
// static
//

static xapi stage(config ** cfg)
{
  enter;

  if(config_active == 0)
  {
    config_active = *cfg;
  }
  else
  {
    fatal(config_merge, config_active, *cfg);
    fatal(config_xfree, *cfg);
  }

  *cfg = 0;

  finally : coda;
}

//
// internal
//

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

  /* formula success */
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

  /* formula error */
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

  /* formula */
  empty = true;
  CFGCOPY(dst, src, formula, capture_stdout);
  CFGCOPY(dst, src, formula, stdout_buffer_size);
  CFGCOPY(dst, src, formula, capture_stderr);
  CFGCOPY(dst, src, formula, stderr_buffer_size);
  CFGCOPY(dst, src, formula, capture_auxout);
  CFGCOPY(dst, src, formula, auxout_buffer_size);
  dst->formula.merge_significant = !empty;

  finally : coda;
}

xapi config_throw(box * restrict val)
{
  enter;

  xapi_info_pushf("location", "[%d,%d - %d,%d]", val->loc.f_lin, val->loc.f_col, val->loc.l_lin, val->loc.l_col);

  fail(CONFIG_INVALID);

  finally : coda;
}

xapi config_create(config ** restrict rv)
{
  enter;

  config * cfg = 0;

  fatal(xmalloc, &cfg, sizeof(*cfg));

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
    box_free(refas(cfg->formula.capture_stdout, bx));
    box_free(refas(cfg->formula.stdout_buffer_size, bx));
    box_free(refas(cfg->formula.capture_stderr, bx));
    box_free(refas(cfg->formula.stderr_buffer_size, bx));
    box_free(refas(cfg->formula.capture_auxout, bx));
    box_free(refas(cfg->formula.auxout_buffer_size, bx));

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

  const char *name;

  if(cfg->formula.merge_significant)
  {
    fatal(value_writer_push_mapping, writer);
    fatal(value_writer_string, writer, "formula");
    fatal(value_writer_push_set, writer);

    if(cfg->formula.capture_stdout)
    {
      name = attrs32_name_byvalue(stream_part_attrs, STREAM_PART_OPT & cfg->formula.capture_stdout->v);
      fatal(value_writer_mapping_string_string, writer, "capture-stdout", name);
    }
    if(cfg->formula.stdout_buffer_size)
    {
      fatal(value_writer_mapping_string_uint, writer, "stdout-buffer-size", cfg->formula.stdout_buffer_size->v);
    }
    if(cfg->formula.capture_stderr)
    {
      name = attrs32_name_byvalue(stream_part_attrs, STREAM_PART_OPT & cfg->formula.capture_stderr->v);
      fatal(value_writer_mapping_string_string, writer, "capture-stderr", name);
    }
    if(cfg->formula.stderr_buffer_size)
    {
      fatal(value_writer_mapping_string_uint, writer, "stderr-buffer-size", cfg->formula.stderr_buffer_size->v);
    }
    if(cfg->formula.capture_auxout)
    {
      name = attrs32_name_byvalue(stream_part_attrs, STREAM_PART_OPT & cfg->formula.capture_auxout->v);
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

  finally : coda;
}

//
// public
//

xapi config_setup()
{
  enter;

  fatal(config_parser_create, &parser_active);

  finally : coda;
}

xapi config_cleanup()
{
  enter;

  fatal(config_parser_xfree, parser_active);
  fatal(config_xfree, config_active);

  finally : coda;
}

xapi config_report()
{
  enter;

  finally : coda;
}

static xapi begin_staging()
{
  enter;

  char * text = 0;
  size_t text_len = 0;
  config * cfg = 0;

  // system-level config
  fatal(usnarfs, &text, &text_len, SYSTEM_CONFIG_PATH);
  if(text)
  {
    logf(L_CONFIG, "staging global config @ %s", SYSTEM_CONFIG_PATH);
    fatal(config_parser_parse, parser_active, text, text_len + 2, SYSTEM_CONFIG_PATH, 0, &cfg);
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
    fatal(config_parser_parse, parser_active, text, text_len + 2, USER_CONFIG_PATH, 0, &cfg);
    fatal(stage, &cfg);
  }
  else
  {
    logf(L_CONFIG, "no user config @ %s/%s", g_params.homedir, USER_CONFIG_PATH);
  }

  // project-level config
  iwfree(&text);
  fatal(usnarff, &text, &text_len, "%s/%s", g_params.proj_dir, PROJECT_CONFIG_PATH);
  if(text)
  {
    logf(L_CONFIG, "staging project config @ %s/%s", g_params.proj_dir, PROJECT_CONFIG_PATH);
    fatal(config_parser_parse, parser_active, text, text_len + 2, PROJECT_CONFIG_PATH, 0, &cfg);
    fatal(stage, &cfg);
  }
  else
  {
    logf(L_CONFIG, "no project config @ %s/%s", g_params.proj_dir, PROJECT_CONFIG_PATH);
  }

  if(!config_active)
  {
    fatal(config_create, &config_active);
  }

finally:
  wfree(text);
  fatal(config_xfree, cfg);
coda;
}

xapi config_reconfigure()
{
  enter;

  narrator * N;

  fatal(begin_staging);

  if(log_would(L_CONFIG))
  {
    fatal(log_start, L_CONFIG, &N);
    xsays("effective config\n");
    fatal(config_say, config_active, N);
    fatal(log_finish);
  }

  fatal(build_command_reconfigure, config_active);
  fatal(config_report);

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
