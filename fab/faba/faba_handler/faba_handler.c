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

#include "xapi.h"
#include "xlinux.h"

#include "global.h"
#include "faba_handler.h"
#include "executor.h"
#include "identity.h"

#include "sigbank.h"
#include "macros.h"

#define restrict __restrict

//
// static
//

static int uxfabdkill(int sig, int * r)
{
	fatal(identity_assume_fabsys);
	fatal(uxkill, g_params.fabd_pgid, sig, r);
	fatal(identity_assume_user);

	finally : coda;
}

//
// public
//

int faba_handler_handle_request(executor_context * const restrict ctx)
{
  char space[2048];

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
			fatal(uxfabdkill, FABSIG_BPGOOD, 0);
		}
		else
		{
			fatal(uxfabdkill, FABSIG_BPBAD, 0);

			// some command failed
			fail(FAB_CMDFAIL);
		}

    // wait for next stage to be prepared
    int sig;
    fatal(sigreceive, &sig);
		if(sig != FABSIG_BPSTART)
			break;
	}

finally:
	fatal(ixclose, &fd);
coda;
}
