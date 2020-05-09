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

#include "moria/graph.h"

#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"
#include "logger.h"
#include "narrator.h"
#include "narrator/units.h"
#include "valyria/llist.h"

#include "usage.h"
#include "logging.h"
#include "params.h"
#include "graph.h"

//
// public
//

xapi usage_report()
{
  enter;

  char space[64] = {};
  int fd = -1;
  narrator * N;

  if(log_would(L_USAGE))
  {
    // check memory usage
    fatal(uxopens, &fd, O_RDONLY, "/proc/self/statm");

    long pages = 0;
    if(fd != -1)
    {
      fatal(xread, fd, space, sizeof(space), 0);
      sscanf(space, "%*d %ld", &pages);
    }
    fatal(log_start, L_USAGE, &N);
    xsays("usage : mem(");
    fatal(bytesize_say, pages * g_params.pagesize, N);
    xsays(")");
    fatal(log_finish);

    logf(L_USAGE, "graph : %zu nodes, %zu edges", llist_count(graph_vertices(g_graph)), llist_count(graph_edges(g_graph)));
  }

finally:
  fatal(ixclose, &fd);
coda;
}
