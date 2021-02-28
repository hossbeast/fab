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
#include "valyria/list.h"
#include "valyria/pstring.h"
#include "fab/ipc.h"

#include "logging.h"
#include "config.internal.h"
#include "CONFIG.errtab.h"
#include "yyutil/box.h"

#include "macros.h"

logger_category * categories = (logger_category []) {
    { name : "ERROR"      , description : "fatal errors" }
  , { name : "WARN"       , description : "non-fatal errors", attr : L_YELLOW }
  , { name : "INFO"       , description : "high-level summary of actions" }
  , { name : "PROTOCOL"   , description : "request/response exchange" }
  , { name : "CONFIG"     , description : "configuration" }
  , { name : "PARAMS"     , description : "runtime parameters" }
  , { name : "USAGE"      , description : "resource usage reports" }
  , { name : "FSEVENT"    , description : "monitored filesystem events" }
  , { name : "MODULE"     , description : "modules" }
  , { name : "BUILDPLAN"  , description : "buildplan" }
  , { name : "BUILD"      , description : "build execution" }
  , { name : "REQUEST"    , description : "request parsing" }
  , { name : "FORMULA"    , description : "formula parsing" }
  , { name : "SELECTOR"   , description : "selector parsing" }
  , { name : "PATTERN"    , description : "pattern parsing" }
  , { name : "RULE"       , description : "rule match/generate results" }
  , { name : "NODE"       , description : "graph node creation/deletion" }
  , { name : "FSGRAPH"    , description : "filesystem graph changes" }
  , { name : "DEPGRAPH"   , description : "dependency graph changes" }
  , { name : "RULEGRAPH"  , description : "rules graph changes" }
#if DEBUG || DEVEL || XAPI
  , { name : "IPC"        , description : "signal-exchange", attr : L_NAMES | L_PID | L_TID }
#endif
  , { name : "NOTIFY"     , description : "notify thread" , optional : 1 }
  , { name : "SERVER"     , description : "server thread" , optional : 1 }
  , { name : "SWEEPER"    , description : "sweeper thread" , optional : 1 }
  , { name : "BUILDER"    , description : "builder thread" , optional : 1 }
  , { name : "MONITOR"    , description : "monitor thread" , optional : 1 }
  , { name : "HANDLER"    , description : "handler thread" , optional : 1 }
  , { name : "BEHOLDER"   , description : "beholder thread" , optional : 1 }
  , { name : "LOADER"     , description : "loader thread" , optional : 1 }
  , { name : "WORKER"     , description : "worker thread" , optional : 1 }
  , { }
};

/*
 * logging stream categories are applied during first reconfiguration
 */
logger_stream * streams = (logger_stream []) {
    { name : "logfile"  , type : LOGGER_STREAM_ROLLING, expr : "+INFO +WARN +ERROR"
      , file_mode : 0666, threshold : 1024 * 1024, max_files : 10, path_base : (char[256]) { } }
#if DEVEL
  , { name : "console"  , type : LOGGER_STREAM_FD , fd : 1, expr : "+INFO +WARN +ERROR" } //  +FSEVENT +BUILDPLAN" }
#endif
  , { }
};

// while misconfigured, log any messages to stderr
int g_logger_default_stderr = 1;

xapi logging_finalize()
{
  enter;

  /* this function is only called from unit tests */
  fatal(logger_category_register, categories);
  fatal(logger_finalize);

  finally : coda;
}

xapi logging_setup(uint64_t hash)
{
  enter;

  snprintf(streams[0].path_base, 256, "%s/%016"PRIx64"/fabd/log", XQUOTE(FABIPCDIR), hash);
  fatal(logger_stream_register, streams);

  // process-level properties
  logger_set_process_name("fabd");

  fatal(logging_finalize);

  finally : coda;
}

xapi logging_reconfigure(configblob * restrict cfg, bool dry)
{
  enter;

  box_string * expr;
  int x;

goto XAPI_FINALLY;

  if(dry)
  {
    // validate logging exprs
    for(x = 0; x < cfg->logging.console.exprs.items->size; x++)
    {
       expr = list_get(cfg->logging.console.exprs.items, x);
      if(!logger_expr_validate(expr->v))
        fatal(config_throw, &expr->bx);
    }

    for(x = 0; x < cfg->logging.logfile.exprs.items->size; x++)
    {
      expr = list_get(cfg->logging.logfile.exprs.items, x);
      if(!logger_expr_validate(expr->v))
        fatal(config_throw, &expr->bx);
    }
  }
  else
  {
    // apply new logger configuration
    if(cfg->logging.logfile.changed)
    {
      fatal(logger_expr_reset, streams[0].id);

      /* exprs from config */
      for(x = 0; x < cfg->logging.logfile.exprs.items->size; x++)
      {
        expr = list_get(cfg->logging.logfile.exprs.items, x);
        fatal(logger_expr_push, streams[0].id, expr->v);
      }

      /* exprs from stream defs */
      if(streams[0].expr)
      {
        fatal(logger_expr_push, streams[0].id, streams[0].expr);
      }
      if(streams[0].exprs)
      {
        char ** exprs = streams[0].exprs;
        while(*exprs)
        {
          fatal(logger_expr_push, streams[0].id, *exprs);
          exprs++;
        }
      }

      /* exprs from cmdline */
      for(x = 0; x < g_logc; x++)
      {
        fatal(logger_expr_push, streams[0].id, g_logv[x]);
      }
    }

    if(cfg->logging.console.changed)
    {
      fatal(logger_expr_reset, streams[1].id);

      /* exprs from config */
      for(x = 0; x < cfg->logging.console.exprs.items->size; x++)
      {
        expr = list_get(cfg->logging.console.exprs.items, x);
        fatal(logger_expr_push, streams[1].id, expr->v);
      }

      /* exprs from stream defs */
      if(streams[1].expr)
      {
        fatal(logger_expr_push, streams[1].id, streams[1].expr);
      }
      if(streams[1].exprs)
      {
        char ** exprs = streams[1].exprs;
        while(*exprs)
        {
          fatal(logger_expr_push, streams[1].id, *exprs);
          exprs++;
        }
      }

      /* exprs from cmdline */
      for(x = 0; x < g_logc; x++)
      {
        fatal(logger_expr_push, streams[1].id, g_logv[x]);
      }
    }

    if(cfg->logging.changed)
    {
      fatal(logger_streams_report);
    }
  }

  finally : coda;
}
