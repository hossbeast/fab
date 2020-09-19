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

#include "stats.h"
#include "logging.h"

struct g_stats g_stats;

//
// public
//

xapi stats_setup()
{
  enter;

  finally : coda;
}

xapi stats_cleanup()
{
  enter;

  finally : coda;
}

xapi stats_report()
{
  enter;

#undef DEF
#define DEF(x, t) do {                                      \
  logf(L_USAGE, "%20s %"PRIu64, #x, (uint64_t)g_stats.x);   \
} while(0);
STATS_TABLE

  finally : coda;
}
