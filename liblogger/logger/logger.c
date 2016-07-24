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
#include "logging.internal.h"
#include "category.internal.h"
#include "stream.internal.h"
#include "filter.internal.h"
#include "arguments.internal.h"
#include "log.internal.h"

#define restrict __restrict

//
// api
//

APIDATA int g_logger_default_stderr;

static int handles;
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
    fatal(logger_category_register, logger_categories);
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

API xapi logger_finalize()
{
  enter;

  // resolve registrations
  fatal(categories_activate);
  fatal(streams_activate);

  // reset filters for all streams
  fatal(logger_filter_clear, 0);

  // stream definition filters
  fatal(streams_finalize);

  // cmdline filters
  fatal(arguments_finalize);

  // reports to LOGGER
  fatal(logger_arguments_report);
  fatal(categories_report_verbose);
  fatal(logger_streams_report);

  finally : coda;
}
