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

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "xapi.h"
#include "xapi/calltree.h"
#include "fab/load.h"
#include "logger/load.h"
#include "valyria/load.h"
#include "narrator/load.h"
#include "xlinux/load.h"

#include "xapi/trace.h"
#include "xapi/SYS.errtab.h"
#include "xapi/errtab.h"
#include "xlinux/xtime.h"
#include "xlinux/xsignal.h"
#include "xlinux/xunistd.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xstat.h"
#include "narrator.h"
#include "narrator/units.h"
#include "logger.h"

#include "fab/sigutil.h"
#include "fab/request.h"
#include "fab/client.h"
#include "fab/FAB.errtab.h"

#include "args.h"
#include "params.h"
#include "logging.h"
#include "build.h"
#include "MAIN.errtab.h"

#include "macros.h"
#include "memblk.h"
#include "memblk.def.h"
#include "parseint.h"
#include "cksum.h"
#include "identity.h"

#define restrict __restrict

//
// public
//

static xapi begin(int argc, char** argv, char ** envp)
{
  enter;

#if DEVEL
  char space[512];
#endif
  char * fabw_path = 0;
  const command * cmd = 0;

  memblk * mb = 0;
  fab_client * client = 0;
  fab_request * request = 0;

  int fd = -1;

#if DEBUG || DEVEL
  // this check is omitted in DEBUG/DEVEL mode because valgrind requires non-setgid and non-setuid executables
#else
  // this executable MUST BE OWNED by fabsys:fabsys and have u+s and g+s permissions
  if(strcmp(g_euid_name, "fabsys") || strcmp(g_egid_name, "fabsys"))
  {
    xapi_info_pushf("real", "r:%s/%d:%s/%d", g_ruid_name, g_ruid, g_rgid_name, g_rgid);
    xapi_info_pushf("effective", "e:%s/%d:%s/%d", g_euid_name, g_euid, g_egid_name, g_egid);
    fail(MAIN_EXEPERMS);
  }
#endif

  // parse cmdline arguments
  fatal(args_parse, &cmd);
  fatal(args_report, cmd);

  // ensure fabd can write to my stdout/stderr
  fatal(xfchmod, 1, 0777);
  fatal(xfchmod, 2, 0777);

#if DEVEL
  snprintf(space, sizeof(space), "%s/../fabw/fabw.devel", g_params.exedir);
  fabw_path = space;
#endif

  fatal(fab_client_create, &client, ".", XQUOTE(FABIPCDIR), fabw_path);

#if DEBUG || DEVEL
  logs(L_IPC, "started");
#endif

#if 0
  // possibly kill the existing fabd instance, if any
  if(changed credentials)
    fatal(client_terminate);
#endif

  fatal(fab_client_prepare, client);
  fatal(fab_client_launchp, client);

  fatal(memblk_mk, &mb);
  fatal(args_collate, cmd, mb, &request);
  fatal(fab_client_make_request, client, mb, request);

finally:
  if(XAPI_UNWINDING)
  {
    if(XAPI_ERRVAL == FAB_FABDEXIT || XAPI_ERRVAL == FAB_UNSUCCESS)
    {
      // on orderly shutdown fabd has already backtraced to our stdout
    }
    if(XAPI_ERRVAL == MAIN_BADARGS || XAPI_ERRVAL == MAIN_NOCOMMAND)
    {
      fatal(args_usage, cmd, 1, 1);
    }
  }

  // locals
  fatal(ixclose, &fd);
  fatal(fab_client_dispose, &client);
  memblk_free(mb);

  // module teardown
  fatal(build_command_cleanup);
coda;
}

int main(int argc, char ** argv, char ** envp)
{
  enter;

  xapi R;
  char space[4096];

  // load libraries
  fatal(fab_load);
  fatal(logger_load);
  fatal(logger_load);
  fatal(narrator_load);
  fatal(valyria_load);
  fatal(xlinux_load);

  // load modules
  fatal(identity_setup);
  fatal(logging_setup, envp);
  fatal(params_report);
  fatal(params_setup);
  fatal(sigutil_defaults);

  fatal(begin, argc, argv, envp);
finally:
  if(XAPI_UNWINDING)
  {
#if DEBUG || DEVEL || XAPI
    xapi_infos("name", "fab");
    xapi_infof("pgid", "%ld", (long)getpgid(0));
    xapi_infof("pid", "%ld", (long)getpid());
    xapi_infof("tid", "%ld", (long)gettid());

    xapi_trace_full(space, sizeof(space));
#else
    xapi_trace_pithy(space, sizeof(space));
#endif

    xlogs(L_ERROR, L_CATEGORY_OFF, space);
  }

  // modules
  params_teardown();

  // libraries
  fatal(fab_unload);
  fatal(logger_unload);
  fatal(narrator_unload);
  fatal(valyria_unload);
  fatal(xlinux_unload);
conclude(&R);
  xapi_teardown();

  return !!R;
}
