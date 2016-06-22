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
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "lorien/canon.h"
#include "logger.h"

#include "internal.h"
#include "params.internal.h"
#include "errtab/FAB.errtab.h"
#include "logging.internal.h"

#include "macros.h"

APIDATA struct g_params_t g_params = { 0 };

//
// public
//

xapi params_setup()
{
  enter;

  char space[512];
  char space2[512];

  //
  // parameters
  //
  g_params.pid = getpid();
  g_params.ppid = getppid();
  g_params.pgid = getpgid(0);
  g_params.cwd = getcwd(0, 0);
  g_params.cwdl = strlen(g_params.cwd);

  // exedir is the canonical path to directory containing the executing binary
  ssize_t r = 0;
  fatal(xreadlink, "/proc/self/exe", space, sizeof(space), &r);
  r += snprintf(space + r, sizeof(space) - r, "/..");

  fatal(canon, space, r, g_params.cwd, g_params.cwdl, space2, sizeof(space2), 0, CAN_REALPATH);
  fatal(ixstrdup, &g_params.exedir, space2);
  if((g_params.exedirl = strlen(g_params.exedir)) < 1)
    fail(FAB_NXPARAMS);

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
  free(g_params.cwd);
  free(g_params.exedir);
}

xapi params_report()
{
  enter;

  // log execution parameters under PARAMS
  logf(L_FABCORE | L_PARAMS, "%11spid                    =%u"           , ""  , g_params.pid);
  logf(L_FABCORE | L_PARAMS, "%11seid                    =%s/%d:%s/%d"  , ""  , g_params.euid_name, g_params.euid, g_params.egid_name, g_params.egid);
  logf(L_FABCORE | L_PARAMS, "%11srid                    =%s/%d:%s/%d"  , ""  , g_params.ruid_name, g_params.ruid, g_params.rgid_name, g_params.rgid);
  logf(L_FABCORE | L_PARAMS, "%11scwd                    =%s"           , ""  , g_params.cwd);
  logf(L_FABCORE | L_PARAMS, "%11sexedir                 =%s"           , ""  , g_params.exedir);
  logf(L_FABCORE | L_PARAMS, "%11sprocessors             =%ld"          , ""  , g_params.procs);
  logf(L_FABCORE | L_PARAMS, "%11spagesize               =%ld"          , ""  , g_params.pagesize);
  logf(L_FABCORE | L_PARAMS, "%11stmpdir                 =%s"           , ""  , XQUOTE(FABTMPDIR));
  logf(L_FABCORE | L_PARAMS, "%11sipcdir                 =%s"           , ""  , XQUOTE(FABIPCDIR));
  logf(L_FABCORE | L_PARAMS, "%11slwopdir                =%s"           , ""  , XQUOTE(FABLWOPDIR));
  logf(L_FABCORE | L_PARAMS, "%11sinvokedir              =%s"           , ""  , XQUOTE(FABINVOKEDIR));

  finally : coda;
}
