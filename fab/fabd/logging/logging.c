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
    { name : "console"  , type : LOGGER_STREAM_FD , fd : 1, expr : "+INFO +WARN +ERROR +CONFIG +MODULE +FSEVENT" }
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

  fatal(logger_stream_register, streams);

  // process-level properties
  logger_set_process_name("fabd");

  fatal(logging_finalize);

  finally : coda;
}
