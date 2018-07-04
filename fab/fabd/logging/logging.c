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

#include "xapi.h"

#include "logger.h"
#include "logger/stream.h"
#include "logger/expr.h"
#include "logger/arguments.h"
#include "logger/config.h"
#include "value.h"
#include "value/query.h"
#include "valyria/list.h"
#include "valyria/pstring.h"
#include "fab/ipc.h"

#include "logging.h"
#include "reconfigure.h"
#include "config.internal.h"
#include "config_cursor.h"
#include "CONFIG.errtab.h"

#include "macros.h"

logger_category * categories = (logger_category []) {
#if DEBUG || DEVEL || XAPI
    { name : "IPC"      , description : "signal-exchange", attr : L_NAMES }
  , { name : "PROTOCOL" , description : "request/response exchange", attr : L_NAMES }
  ,
#endif
    { name : "ERROR"    , description : "fatal errors" }
  , { name : "WARN"     , description : "non-fatal errors", attr : L_YELLOW }
  , { name : "INFO"     , description : "high-level summary of actions" }
  , { name : "CONFIG"   , description : "configuration" }
  , { name : "FF"       , description : "fabfiles" }
  , { name : "PARAMS"   , description : "runtime parameters" }
  , { name : "USAGE"    , description : "resource usage reports" }
  , { name : "GRAPH"    , description : "dependency graph changes" }
  , { name : "FSEVENT"  , description : "monitored filesystem events" }
  , { name : "MODULE"   , description : "modules" }

  , { name : "NOTIFY"   , description : "notify thread" , optional : 1 }
  , { name : "SERVER"   , description : "server thread" , optional : 1 }
  , { name : "SWEEPER"  , description : "sweeper thread" , optional : 1 }
  , { name : "MONITOR"  , description : "monitor thread" , optional : 1 }
  , { name : "DAEMON"   , description : "daemon" , optional : 1 }
  , { }
};

logger_stream * streams = (logger_stream []) {
    { name : "console"  , type : LOGGER_STREAM_FD , expr : "+WARN +ERROR"
      , fd : 1 }
  , { name : "logfile"  , type : LOGGER_STREAM_ROLLING, attr : L_DATESTAMP | L_CATEGORY | L_NOCOLOR | L_NAMES | L_NOFILTER
      , file_mode : FABIPC_MODE_DATA, threshold : 1024 * 1024, max_files : 10, path_base : (char[256]) { } }
  , { }
};

// while misconfigured, log any messages to stderr
int g_logger_default_stderr = 1;

#if DEBUG || DEVEL
int g_logging_skip_reconfigure;
#endif

xapi logging_setup(uint32_t hash)
{
  enter;

  // per-ipc logfiles
  if(hash)
  {
    snprintf(streams[1].path_base, 256, "%s/%x/fabd/log", XQUOTE(FABIPCDIR), hash);
    fatal(logger_stream_register, streams);
  }

  // process-level properties
  logger_set_process_name("fabd");
  logger_set_process_categories(L_DAEMON);

  fatal(logging_finalize);

  finally : coda;
}

xapi logging_finalize()
{
  enter;

  fatal(logger_category_register, categories);
  fatal(logger_finalize);

  finally : coda;
}

xapi logging_reconfigure(reconfigure_context * ctx, const value * restrict config, uint32_t dry)
{
  enter;

  value * list;
  value * val;
  config_cursor cursor;
  int x;

  fatal(config_cursor_init, &cursor);

#if DEBUG || DEVEL
  if(g_logging_skip_reconfigure)
    goto XAPI_FINALIZE;
#endif

  if(!dry)
    fatal(logger_expr_clear, streams[0].id);

  fatal(config_cursor_sets, &cursor, "logging.console.exprs");
  fatal(config_query, config, config_cursor_path(&cursor), config_cursor_query(&cursor), VALUE_TYPE_LIST & dry, &list);
  if(list)
  {
    fatal(config_cursor_mark, &cursor);
    for(x = 0; x < list->items->l; x++)
    {
      fatal(config_cursor_pushd, &cursor, x);
      fatal(config_query, list, config_cursor_path(&cursor), config_cursor_query(&cursor), VALUE_TYPE_STRING & dry, &val);

      if(dry && !logger_expr_validate(val->s->s))
      {
        xapi_info_pushs("expr", val->s->s);
        fatal(config_throw, CONFIG_INVALID, val, config_cursor_path(&cursor));
      }

      if(!dry)
      {
        fatal(logger_expr_push, streams[0].id, val->s->s);
      }
    }
  }

#if 0
  if(!dry)
    fatal(logger_expr_clear, streams[1].id);

  fatal(config_cursor_sets, &cursor, "logging.logfile.exprs");
  fatal(config_query, config, config_cursor_path(&cursor), config_cursor_query(&cursor), VALUE_TYPE_LIST & dry, &list);
  if(list)
  {
    fatal(config_cursor_mark, &cursor);
    for(x = 0; x < list->items->l; x++)
    {
      fatal(config_cursor_pushd, &cursor, x);
      fatal(config_query, list, config_cursor_path(&cursor), config_cursor_query(&cursor), VALUE_TYPE_STRING & dry, &val);

      if(dry && !logger_expr_validates(val->s->s))
        fatal(reconfigure_throw, CONFIG_INVALID, val, config_cursor_path(&cursor));

      if(!dry)
        fatal(logger_expr_pushs, streams[1].id, val->s->s);
    }
  }
#endif

finally:
  config_cursor_destroy(&cursor);
coda;
}
