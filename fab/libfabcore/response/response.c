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
#include "xapi/exit.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "narrator.h"

#include "internal.h"
#include "response.internal.h"

#include "memblk.def.h"

#define restrict __restrict

//
// api
//
//

API xapi response_create(response ** const restrict res)
{
  enter;

  fatal(xmalloc, res, sizeof(**res));

  finally : coda;
}

API void response_free(response * const restrict res)
{
  if(res)
  {
    free(res->trace);
  }

  free(res);
}

API void response_ifree(response ** const restrict res)
{
  response_free(*res);
  *res = 0;
}

API void response_freeze(response * const restrict res, memblk * const restrict mb)
{
  memblk_freeze(mb, &res->trace);
}

API void response_thaw(response * const restrict res, char * const restrict mb)
{
  memblk_thaw(mb, &res->trace);
}

API xapi response_say(const response * const restrict res, narrator * const restrict N)
{
  enter;

  sayf("exit %08x", res->exit);
  if(res->exit)
  {
//    sayf(", errtab %s", res->errtab);
//    sayf(", errname %s", res->errname);
    sayf(", pithy %.*s", (int)res->tracesz, res->trace);
  }

  finally : coda;
}

API xapi response_error(response * const restrict resp, xapi exit, const char * const restrict trace, size_t tracesz)
{
  enter;

  resp->exit = exit;
  fatal(ixstrdup, &resp->trace, trace);
  resp->tracesz = tracesz;
  fatal(ixstrdup, &resp->errtab, xapi_exit_errtab_name(exit));
  fatal(ixstrdup, &resp->errname, xapi_exit_errname(exit));

  finally : coda;
}
