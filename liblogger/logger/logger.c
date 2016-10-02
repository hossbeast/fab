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

#include "internal.h"
#include "category.internal.h"
#include "stream.internal.h"
#include "filter.internal.h"
#include "arguments.internal.h"

//
// api
//

APIDATA int g_logger_default_stderr;

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
