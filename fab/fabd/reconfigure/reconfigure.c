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

#include "xapi.h"
#include "xapi/trace.h"
#include "xapi/calltree.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"
#include "narrator.h"
#include "logger.h"
#include "value.h"
#include "value/store.h"
#include "value/query.h"
#include "value/merge.h"
#include "value/parser.h"
#include "valyria/pstring.h"
#include "valyria/list.h"

#include "internal.h"
#include "reconfigure.h"
#include "node.h"
#include "config.internal.h"
#include "CONFIG.errtab.h"
#include "filesystem.h"
#include "logging.h"
#include "params.h"
#include "extern.h"

#include "snarf.h"
#include "strutil.h"

#define restrict __restrict

#define SYSTEM_CONFIG_PATH    "/etc/fabconfig"  // absolute
#define USER_CONFIG_PATH      ".fab/config"     // relative to $HOME
#define PROJECT_CONFIG_PATH   ".fab/config"     // relative to project dir

static value_parser * parser;
static value_store * store_staging;
static value * config_staging;

//
// static
//

static xapi __attribute__((nonnull)) parse(
    value_store * stor
  , const char * const restrict text
  , size_t len
  , const char * restrict fname
  , value ** restrict root
)
{
  enter;

  fatal(config_parse, &parser, &stor, text, len, fname, root);

  if(*root && (*root)->type != VALUE_TYPE_MAP)
    fatal(config_throw, CONFIG_NOTMAP, *root, 0);

  finally : coda;
}

static xapi applyw(const char * const restrict text, size_t textl, const char * restrict fname, value_store * const restrict store, value ** restrict dst)
{
  enter;

  value * val = 0;
  fatal(parse, store, text, textl, fname, &val);

  // merge with the existing config
  if(*dst == 0)
    *dst = val;
  else if(val)
    fatal(value_merge, *dst, val);

  finally : coda;
}

static xapi applys(const char * const restrict text, const char * restrict fname, value_store * const restrict store, value ** restrict dst)
{
  xproxy(applyw, text, strlen(text), fname, store, dst);
}

//
// public
//

xapi reconfigure_setup()
{
  enter;

  fatal(value_parser_create, &parser);

  finally : coda;
}

xapi reconfigure_cleanup()
{
  enter;

  fatal(value_parser_xfree, parser);
  fatal(value_store_xfree, store_staging);
  config_staging = 0;

  finally : coda;
}

xapi reconfigure_stage_files()
{
  enter;

  char * text = 0;

  // reset the staging config
  fatal(value_store_create, &store_staging);
  config_staging = 0;

  // apply system-level config
  fatal(usnarfs, &text, 0, SYSTEM_CONFIG_PATH);
  if(text)
  {
    logf(L_CONFIG, "staging global config @ %s", SYSTEM_CONFIG_PATH);
    fatal(applys, text, SYSTEM_CONFIG_PATH, store_staging, &config_staging);
  }
  else
  {
    logf(L_CONFIG, "no global config @ %s", SYSTEM_CONFIG_PATH);
  }

  // apply user-level config
  iwfree(&text);
  fatal(usnarff, &text, 0, "%s/%s", g_params.homedir, USER_CONFIG_PATH);
  if(text)
  {
    logf(L_CONFIG, "staging user config @ %s/%s", g_params.homedir, USER_CONFIG_PATH);
    fatal(applys, text, USER_CONFIG_PATH, store_staging, &config_staging);
  }
  else
  {
    logf(L_CONFIG, "no user config @ %s/%s", g_params.homedir, USER_CONFIG_PATH);
  }

  // apply project-level config
  iwfree(&text);
  fatal(usnarfats, &text, 0, g_params.proj_dirfd, PROJECT_CONFIG_PATH);
  if(text)
  {
    logf(L_CONFIG, "staging project config @ %s/%s", g_params.proj_dir, PROJECT_CONFIG_PATH);
    fatal(applys, text, PROJECT_CONFIG_PATH, store_staging, &config_staging);
  }
  else
  {
    logf(L_CONFIG, "no project config @ %s/%s", g_params.proj_dir, PROJECT_CONFIG_PATH);
  }

finally:
  wfree(text);
coda;
}

xapi reconfigure_stage(const char * const restrict text)
{
  enter;

  fatal(applys, text, 0, store_staging, &config_staging);

  finally : coda;
}

xapi reconfigure()
{
  enter;

  char trace[4096];
  reconfigure_context ctx;
  ctx.proj_dir = g_params.proj_dir;

  // validate the new config
  xapi exit = 0;
  if(  (exit = invoke(logging_reconfigure, &ctx, config_staging, ~0))
    || (exit = invoke(filesystem_reconfigure, &ctx, config_staging, ~0))
    || (exit = invoke(node_reconfigure, &ctx, config_staging, ~0))
    || (exit = invoke(extern_reconfigure, &ctx, config_staging, ~0))
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
    fatal(config_promote, store_staging, config_staging);
    store_staging = 0;
    config_staging = 0;

    // apply the new config to subsystems
    fatal(config_report);
    fatal(logging_reconfigure, &ctx, g_config, 0);
    fatal(filesystem_reconfigure, &ctx, g_config, 0);
    fatal(node_reconfigure, &ctx, g_config, 0);
    fatal(extern_reconfigure, &ctx, g_config, 0);
  }

  finally : coda;
}
