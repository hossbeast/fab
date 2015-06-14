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
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

#include "xapi.h"
#include "xlinux.h"
#include "pstring.h"

#include "global.h"
#include "faba_handler.h"
#include "executor.h"
#include "identity.h"
#include "args.def.h"

#include "sigbank.h"
#include "macros.h"

#define restrict __restrict

//
// static
//

//
// public
//

int faba_handler_handle_request(executor_context * const restrict ctx)
{
  char space[2048];
  struct stat stb;
  pstring * tmp = 0;

	int x;
	int fd = -1;

	int stagesl;
	int commandsl;

  // load info about the entire build
  snprintf(space, sizeof(space), XQUOTE(FABTMPDIR) "/pid/%d/bp/stages", g_params.pid);
  fatal(ixclose, &fd);
  fatal(xopen, space, O_RDONLY, &fd);
  fatal(axread, fd, &stagesl, fd);
  
  snprintf(space, sizeof(space), XQUOTE(FABTMPDIR) "/pid/%d/bp/commands", g_params.pid);
  fatal(ixclose, &fd);
  fatal(xopen, space, O_RDONLY, &fd);
  fatal(axread, fd, &commandsl, fd);

  logf(L_BPEXEC, "buildplan @ %s/%u/bp", XQUOTE(FABIPCDIR), g_params.canhash);

  // one stage at a time
  for(x = 0; x < stagesl; x++)
	{
		// execute the buildplan stage
		int good = 0;
    fatal(executor_execute_stage, ctx, stagesl, commandsl, x, &good);

    if(x == 0)
    {
      // touch stamp file to refresh the expiration on the bp directory
      snprintf(space, sizeof(space), XQUOTE(FABTMPDIR) "/pid/%d/stamp", g_params.pid);
      fatal(ixclose, &fd);
      fatal(uxopen_mode, space, O_CREAT | O_RDWR, FABIPC_DATA, &fd);
      if(fd != -1)
        fatal(xfutimens, fd, 0);
    }

		// notify fabd
		if(good)
		{
      int sig;
      fatal(sigexchange, -g_params.fabd_pgid, FABSIG_BPGOOD, (int[]) { FABSIG_BPSTART, FABSIG_DONE, 0 }, &sig);

      if(sig == FABSIG_DONE)
        break;
		}
		else
		{
			fatal(xkill, -g_params.fabd_pgid, FABSIG_BPBAD);

			// some command failed
			fail(FAB_CMDFAIL);
		}
  }

	// rewrite the buildscript for a bs build
	if(g_args->buildscript_path)
	{
		// open buildscript from the ipc-dir
    snprintf(space, sizeof(space), "%s/bs", g_params.ipcdir);
		fatal(ixclose, &fd);
		fatal(xopen, space, O_RDONLY, &fd);

		// get the filesize
		fatal(xfstat, fd, &stb);

		// read the whole file
		fatal(psgrow, &tmp, stb.st_size);
		fatal(axread, fd, tmp->s, stb.st_size);

		// open the dest path
		fatal(ixclose, &fd);
		fatal(xopen_mode, g_args->buildscript_path, O_CREAT | O_TRUNC | O_WRONLY, 0755, &fd);		// u+rwx go+rx

		// rewrite
		fatal(axwrite, fd, tmp->s, stb.st_size);
		logf(L_INFO, "wrote %s", g_args->buildscript_path);
	}

finally:
	fatal(ixclose, &fd);
  psfree(tmp);
coda;
}
