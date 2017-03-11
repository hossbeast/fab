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
#include "logging.h"
#include "params.h"
#include "CONFIG.errtab.h"

#include "strutil.h"

#define restrict __restrict

static value_store * store_active;
value * g_config;

//
// static
//

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

    fatal(config_throw, CONFIG_INCOMPAT, val, path);
  }
  else if(notnull && !val)
  {
    xapi_fail_intent();
    xapi_info_adds("expected", "(any)");
    xapi_info_adds("actual", "(none)");

    fatal(config_throw, CONFIG_MISSING, val, path);
  }

  finally : coda;
}

//
// protected
//

xapi config_promote(value_store * store_staging, value * config_staging)
{
  enter;

  fatal(value_store_ixfree, &store_active);
  store_active = store_staging;
  g_config = config_staging;

  finally : coda;
}

//
// public
//

xapi config_setup()
{
  enter;

  finally : coda;
}

xapi config_cleanup()
{
  enter;

  fatal(value_store_xfree, store_active);
  g_config = 0;

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

xapi config_query(const value * restrict base, const char * restrict path, const char * restrict query, uint32_t opts, value ** restrict res)
{
  enter;

  value * val = value_query(base, query);
  fatal(validate, val, path, opts);
  *res = val;

  finally : coda;
}

xapi config_throw(xapi error, value * restrict val, const char * restrict path)
{
  enter;

  xapi_fail_intent();

  if(path)
    xapi_info_adds("path", path);

  if(val)
    xapi_info_addf("location", "[%d,%d - %d,%d]", val->loc.f_lin, val->loc.f_col, val->loc.l_lin, val->loc.l_col);

  if(val->loc.fname)
    xapi_info_adds("file", val->loc.fname);

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

  fail(error);

  finally : coda;
}
