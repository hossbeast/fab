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

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdarg.h>

#include "xapi.h"
#include "xapi/SYS.errtab.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "logger.h"

#include "internal.h"
#include "params.h"
#include "logging.h"

#include "macros.h"

struct g_params g_params;

//
// public
//

xapi params_setup()
{
  enter;

  char space[512];

  //
  // parameters
  //
  g_params.pid = getpid();
  g_params.ppid = getppid();
  g_params.pgid = getpgid(0);

  // exedir is the path to directory containing the executing binary
  ssize_t r = 0;
  fatal(xreadlinks, "/proc/self/exe", space, sizeof(space), &r);
  while(space[r - 1] != '/')
    r--;
  fatal(ixstrndup, &g_params.exedir, space, r);

  // get available processors
  if((g_params.procs = sysconf(_SC_NPROCESSORS_ONLN)) == -1)
  {
    // unable to determine available CPU count
    g_params.procs = 0;
  }
  else if(g_params.procs < 1)
  {
    // shenanigans
    g_params.procs = 0;
  }

  // page size
  g_params.pagesize = sysconf(_SC_PAGESIZE);

  finally : coda;
}

void params_teardown()
{
  iwfree(&g_params.exedir);
}

xapi params_report()
{
  enter;

  // log execution parameters under PARAMS
  logf(L_PARAMS, "pid          =%u"  , g_params.pid);
  logf(L_PARAMS, "exedir       =%s"  , g_params.exedir);
  logf(L_PARAMS, "processors   =%ld" , g_params.procs);
  logf(L_PARAMS, "pagesize     =%ld" , g_params.pagesize);
  logf(L_PARAMS, "tmpdir       =%s"  , XQUOTE(FABTMPDIR));
  logf(L_PARAMS, "ipcdir       =%s"  , XQUOTE(FABIPCDIR));
  logf(L_PARAMS, "lwopdir      =%s"  , XQUOTE(FABLWOPDIR));
  logf(L_PARAMS, "invokedir    =%s"  , XQUOTE(FABINVOKEDIR));

  finally : coda;
}
