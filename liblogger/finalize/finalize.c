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
#include "category.internal.h"
#include "stream.internal.h"
#include "arguments.internal.h"
#include "expr.internal.h"

//
// api
//

xapi API logger_finalize()
{
  enter;

  // resolve registrations
  fatal(categories_activate);
  fatal(streams_activate);

  // apply exprs attached to streams
  fatal(streams_finalize);

  // process exprs on cmdline
  fatal(arguments_finalize);

  // report to LOGGER
  fatal(logger_arguments_report);
  fatal(categories_report_verbose);
  fatal(logger_streams_report);

  finally : coda;
}
