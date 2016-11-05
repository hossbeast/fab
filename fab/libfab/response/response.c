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

API xapi fab_response_create(fab_response ** const restrict response)
{
  enter;

  fatal(xmalloc, response, sizeof(**response));

  finally : coda;
}

API void fab_response_free(fab_response * const restrict response)
{
  if(response)
  {
    wfree(response->trace);
  }

  wfree(response);
}

API void fab_response_ifree(fab_response ** const restrict response)
{
  fab_response_free(*response);
  *response = 0;
}

API void fab_response_freeze(fab_response * const restrict response, memblk * const restrict mb)
{
  memblk_freeze(mb, &response->trace);
}

API void fab_response_thaw(fab_response * const restrict response, char * const restrict mb)
{
  memblk_thaw(mb, &response->trace);
}

API xapi fab_response_say(const fab_response * const restrict response, narrator * const restrict N)
{
  enter;

  sayf("exit %08x", response->exit);
  if(response->exit)
  {
//    sayf(", errtab %s", response->errtab);
//    sayf(", errname %s", response->errname);
    sayf(", pithy %.*s", (int)response->tracesz, response->trace);
  }

  finally : coda;
}

API xapi fab_response_error(fab_response * const restrict response, xapi exit, const char * const restrict trace, size_t tracesz)
{
  enter;

  response->exit = exit;
  fatal(ixstrdup, &response->trace, trace);
  response->tracesz = tracesz;
  fatal(ixstrdup, &response->errtab, xapi_exit_errtab_name(exit));
  fatal(ixstrdup, &response->errname, xapi_exit_errname(exit));

  finally : coda;
}

API xapi fab_response_success(fab_response * const restrict response)
{
  enter;

  response->exit = 0;

  finally : coda;
}
