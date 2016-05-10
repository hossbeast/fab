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

#if __linux__
#include <sys/auxv.h>
#endif

#include <stdio.h>
#include <string.h>

#include "xapi.h"
#include "xapi/errtab.h"
#include "xlinux.h"
#include "xlinux/LIB.errtab.h"
#include "narrator.h"

#include "internal.h"
#include "LOGGER.errtab.h"
#include "logs.internal.h"
#include "category.internal.h"
#include "stream.internal.h"
#include "filter.internal.h"
#include "arguments.internal.h"
#include "log.internal.h"

#define restrict __restrict

static int handles;

//
// api
//

API xapi logger_load()
{
  enter;

  if(handles == 0)
  {
    // dependencies
    fatal(xlinux_load);
    fatal(narrator_load);

    // modules
    fatal(category_setup);
    fatal(stream_setup);
    fatal(logger_category_register, logs);
#ifndef XAPI_MODE_ERRORCODE
    fatal(xapi_errtab_register, perrtab_LOGGER);
#endif
  }
  handles++;

  finally : coda;
}

API xapi logger_unload()
{
  enter;

  if(--handles == 0)
  {
    // modules
    category_teardown();
    stream_teardown();
    arguments_teardown();
    log_teardown();

    // dependencies
    fatal(xlinux_unload);
    fatal(narrator_unload);
  }
  else if(handles < 0)
  {
    tfails(perrtab_LIB, LIB_AUNLOAD, "library", "liblogger");
  }

  finally : coda;
}

API xapi logger_initialize(char ** restrict envp)
{
  enter;

  // resolve category registration
  fatal(logger_category_activate);

  // activate streams, including embedded filter definitions
  fatal(stream_activate);

  // prepare program arguments
  fatal(arguments_initialize, envp);

  // reports to LOGGER
  fatal(arguments_report);
  fatal(category_report_verbose);
  fatal(streams_report);

  finally : coda;
}
