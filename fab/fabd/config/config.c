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
#include "valyria/pstring.h"
#include "valyria/list.h"

#include "internal.h"
#include "config.h"
#include "config_parser.h"
#include "errtab/CONFIG.errtab.h"
#include "logging.h"
#include "filesystem.h"

#include "snarf.h"
#include "strutil.h"

#define restrict __restrict

#define SYSTEM_CONFIG_PATH    "/etc/fabconfig"
#define USER_CONFIG_PATH      "~/.fabconfig"
#define PROJECT_CONFIG_PATH   ".fab/config"

static config_parser * parser;

static value_store * store_staging;
static value * config_staging;

static value_store * store_active;
value * g_config;

//
// static
//

static xapi __attribute__((nonnull)) parse(
    value_store * stor
  , const char * const restrict text
  , size_t len
  , value ** restrict root
)
{
  enter;

  fatal(config_parser_parse, &parser, &stor, text, len, root);

  if(*root && (*root)->type != VALUE_TYPE_MAP)
    fail(CONFIG_NOTMAP);

  finally : coda;
}

static xapi applyw(const char * const restrict text, size_t textl, value_store * const restrict store, value ** restrict dst)
{
  enter;

  value * val = 0;
  fatal(parse, store, text, textl, &val);

  // merge with the existing config
  if(*dst == 0)
    *dst = val;
  else
    fatal(value_merge, *dst, val);

  finally : coda;
}

static xapi applys(const char * const restrict text, value_store * const restrict store, value ** restrict dst)
{
  xproxy(applyw, text, strlen(text), store, dst);
}

static xapi validate(value * restrict val, const char * restrict path, uint32_t opts)
{
  enter;

  uint8_t type = opts & 0xffff;
  int notnull = opts & CONFIG_QUERY_NOTNULL;

  if(type && val && val->type != type)
  {
    xapi_fail_intent();
    xapi_info_adds("expected", VALUE_TYPE_STRING(type));
    xapi_info_adds("actual", VALUE_TYPE_STRING(val->type));

    fatal(config_invalid, val, path);
  }
  else if(notnull && !val)
  {
    xapi_fail_intent();
    xapi_info_adds("expected", "(any)");
    xapi_info_adds("actual", "(none)");

    fatal(config_invalid, val, path);
  }

  finally : coda;
}

//
// public
//

xapi config_setup()
{
  enter;

  fatal(config_parser_create, &parser);

  finally : coda;
}

xapi config_cleanup()
{
  enter;

  fatal(config_parser_xfree, parser);
  fatal(value_store_xfree, store_active);
  fatal(value_store_xfree, store_staging);
  g_config = 0;
  config_staging = 0;

  finally : coda;
}

xapi config_report()
{
  enter;

  int token = 0;
  if(log_would(L_CONFIG))
  {
    fatal(log_start, L_CONFIG, &token);
    fatal(value_say, g_config, log_narrator(&token));   
    fatal(log_finish, &token);
  }

  finally : coda;
}

xapi config_apply(const char * const restrict text)
{
  enter;

  fatal(applys, text, store_staging, &config_staging);

  finally : coda;
}

xapi config_load()
{
  enter;

  char * text = 0;

  // reset the staging config
  fatal(value_store_ixfree, &store_staging);
  fatal(value_store_create, &store_staging);
  config_staging = 0;

  // apply system-level config
  fatal(usnarfs, SYSTEM_CONFIG_PATH, &text, 0);
  if(text)
    fatal(applys, text, store_staging, &config_staging);

  // apply user-level config
  iwfree(&text);
  fatal(usnarfs, USER_CONFIG_PATH, &text, 0);
  if(text)
    fatal(applys, text, store_staging, &config_staging);

  // apply project-level config
  iwfree(&text);
  fatal(usnarfs, PROJECT_CONFIG_PATH, &text, 0);
  if(text)
    fatal(applys, text, store_staging, &config_staging);

finally:
  wfree(text);
coda;
}

xapi config_reconfigure(xapi * res)
{
  enter;

  char trace[256];
  size_t tracesz;

  // validate the new config
  *res = 0;
  if(  (*res = invoke(logging_reconfigure, config_staging, ~0))
    || (*res = invoke(filesystem_reconfigure, config_staging, ~0)))
  {
    if(xapi_exit_errtab(*res) != perrtab_CONFIG)
      fail(0);

#if DEBUG || DEVEL || XAPI
    tracesz = xapi_trace_full(trace, sizeof(trace));
#else
    tracesz = xapi_trace_pithy(trace, sizeof(trace));
#endif
    xapi_calltree_unwind();

    xlogw(L_ERROR, L_RED, trace, tracesz);
  }
  else
  {
    // promote the staging config to active
    fatal(value_store_ixfree, &store_active);
    store_active = store_staging;
    g_config = config_staging;
    fatal(value_store_create, &store_staging);
    config_staging = 0;
    fatal(config_report);

    // apply the new config to subsystems
    fatal(logging_reconfigure, g_config, 0);
    fatal(filesystem_reconfigure, g_config, 0);
  }

  finally : coda;
}

xapi config_query(const value * restrict base, const char * restrict path, const char * restrict query, uint32_t opts, value ** restrict res)
{
  enter;

  value * val = value_query(base, query);
  fatal(validate, val, path, opts);
  *res = val;

  finally : coda;
}

xapi config_invalid(value * restrict val, const char * restrict path)
{
  enter;

  xapi_fail_intent();

  xapi_info_adds("path", path);

  if(val)
    xapi_info_addf("location", "[%d,%d - %d,%d]", val->loc.f_lin, val->loc.f_col, val->loc.l_lin, val->loc.l_col);

#if 0
  if(type)
    xapi_info_adds("expected", VALUE_TYPE_STRING(type));
  else
    xapi_info_adds("expected", "(any)");

  if(val)
    xapi_info_adds("actual", VALUE_TYPE_STRING(val->type));
  else
    xapi_info_adds("actual", "(none)");
#endif

  fail(CONFIG_ILLEGAL);

  finally : coda;
}

#if 0
xapi config_list_get(value * const restrict list, size_t index, uint32_t opts, value ** const restrict res)
{
  enter;

  value * val = list_get(list->els, index);
  fatal(validate, query, val, opts);
  *res = val;

  finally : coda;
}
#endif
