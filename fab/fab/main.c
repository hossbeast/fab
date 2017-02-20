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
#include "logger/arguments.h"

#include "fab/sigbank.h"
#include "fab/identity.h"
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

#define restrict __restrict

//
// public
//

int main(int argc, char** argv, char ** envp)
{
  enter;

  xapi R = 0;
  char space[512];
  char * fabw_path = 0;
  const command * cmd = 0;

  memblk * mb = 0;
  fab_client * client = 0;
  fab_request * request = 0;

  int fd = -1;
  size_t tracesz = 0;

  // libraries
  fatal(fab_load);
  fatal(logger_load);
  fatal(valyria_load);
  fatal(narrator_load);
  fatal(xlinux_load);


  // logging
  fatal(logging_setup);
  fatal(logger_arguments_setup, envp);
  fatal(logger_finalize);

  // modules
  fatal(params_setup);
  fatal(params_report);
  fatal(sigbank_setup, "fab");

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
#if DEBUG || DEVEL
  if(log_would(L_IPC))
  {
    xapi_infos("name", "fab");
    xapi_infof("pid", "%ld", (long)getpid());
    if(client)
      xapi_infos("hash", fab_client_gethash(client));
  }
#endif

  if(XAPI_UNWINDING)
  {
    if(XAPI_ERRVAL == FAB_FABDEXIT || XAPI_ERRVAL == FAB_UNSUCCESS)
    {
      // on orderly shutdown fabd has already backtraced to our stdout
    }
    else
    {
#if DEBUG || DEVEL || XAPI
      tracesz = xapi_trace_full(space, sizeof(space));
#else
      tracesz = xapi_trace_pithy(space, sizeof(space));
#endif

      xlogw(L_ERROR, L_RED, space, tracesz);
    }

    if(XAPI_ERRVAL == MAIN_BADARGS)
    {
      fatal(args_usage, cmd, 0, 0);
    }
  }

  // locals
  fatal(ixclose, &fd);
  fatal(fab_client_dispose, &client);
  memblk_free(mb);

  // module teardown
  sigbank_teardown();
  params_teardown();
  fatal(build_command_cleanup);

  // libraries
  fatal(fab_unload);
  fatal(logger_unload);
  fatal(valyria_unload);
  fatal(narrator_unload);
  fatal(xlinux_unload);

conclude(&R);
  xapi_teardown();

  return !!R;
}
