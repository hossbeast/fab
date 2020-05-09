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
#include "logger/config.h"

#include "logging.h"

logger_category * categories = (logger_category[]) {
    { name : "ERROR"      , description : "fatal errors" }
  , { name : "ARGS"       , description : "program arguments report" }
  , { name : "PARAMS"     , description : "program parameters report" }
  , { name : "CLIENT"     , description : "fab client", optional : 1 }
  , { name : "PROTOCOL"   , attr : L_NAMES | L_PID }
#if DEBUG || DEVEL || XAPI
  , { name : "IPC"        , description : "signal-exchange", attr : L_NAMES | L_PID | L_YELLOW }
#endif
  , { }
};

logger_stream * streams = (logger_stream []) {
    { name : "console"  , type : LOGGER_STREAM_FD , fd : 1  , expr : "+ERROR"
#if DEBUG || DEVEL || XAPI
" %CATEGORY %NAMES %PID %TID"
#endif
    }
  , { }
};

// while misconfigured, log any messages to stderr
int g_logger_default_stderr = 1;

xapi logging_setup(char ** envp)
{
  enter;

  fatal(logger_category_register, categories);
  fatal(logger_stream_register, streams);
  fatal(logger_arguments_setup, envp);
  fatal(logger_finalize);
  logger_set_process_name("fab");
  logger_set_process_categories(L_CLIENT);

  finally : coda;
}
