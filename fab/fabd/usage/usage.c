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
#include "xlinux.h"
#include "narrator.h"
#include "narrator/units.h"
#include "logger.h"

#include "fabcore/params.h"
#include "fabcore/logging.h"

#include "global.h"
#include "usage.h"

//
// public
//

xapi usage_report()
{
  enter;

  char space[64];
  int fd = -1;
  int token = 0;

  if(log_would(L_USAGE))
  {
    // check memory usage
    fatal(uxopen, "/proc/self/statm", O_RDONLY, &fd);

    long pages = 0;
    if(fd != -1)
    {
      fatal(xread, fd, space, sizeof(space), 0);
      sscanf(space, "%*d %ld", &pages);
    }
    fatal(log_start, L_USAGE, &token);
    logs(0, "usage : mem(");
    fatal(bytesize_say, pages * g_params.pagesize, log_narrator());
    logs(0, ")");
    fatal(log_finish, &token);
  }

finally:
  fatal(log_finish, &token);
  fatal(ixclose, &fd);
coda;
}
