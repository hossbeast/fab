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

#include "logger/stream.h"

#include "internal.h"
#include "logging.internal.h"

APIDATA logger_category * fabcore_categories = (logger_category []) {
    { name : "FABCORE"  , description : "libfabcore logging" }
  , { name : "ERROR"    , description : "errors leading to shutdown" }
  , { name : "INFO"     , description : "program flow" }
  , { name : "PARAMS"   , description : "program execution parameters" }
  , { name : "TIMING"   , description : "granular timing reports" }
  , { name : "USAGE"    , description : "resource usage reports" }
#if 0
  , { name : "BPINFO"   , description : "buildplan - flow" }
  , { name : "BPEXEC"   , description : "buildplan - execution" }
  , { name : "BPEVAL"   , description : "buildplan - pruning/evaluating" }
  , { name : "BPDUMP"   , description : "buildplan - log final buildplan" }
  , { name : "BPCMD"    , description : "buildplan - commands" }
  , { name : "DSCINFO"  , description : "dependency discovery - flow" }
  , { name : "DSCEXEC"  , description : "dependency discovery - execution" }
  , { name : "DSCCMD"   , description : "dependency discovery - commands" }
  , { name : "DSCRES"   , description : "dependency discovery - results" }
  , { name : "NODE"     , description : "dependency graph node details (+i)" }
  , { name : "INVALID"  , description : "dependency graph node invalidations" }
  , { name : "VARAUTO"  , description : "variable definitions - automatic" }
  , { name : "VARUSER"  , description : "variable definitions - user-defined" }
  , { name : "VAR"      , description : "variable definitions" }
  , { name : "INVOKE"   , description : "fabfile invocations" }
  , { name : "SELECT"   , description : "selector execution" }
  , { name : "LISTS"    , description : "node lists" }
#endif
#if DEBUG || DEVEL
  , { name : "IPC"      , description : "signal-exchange" }
#endif
  , { }
};

xapi logging_setup()
{
  enter;

  fatal(logger_category_register, fabcore_categories);

  finally : coda;
}
