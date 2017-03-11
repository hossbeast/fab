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

#include "fab/request.def.h"
#include "fab/command.def.h"
#include "fab/response.h"
#include "fab/ipc.h"
#include "moria/vertex.h"
#include "moria/graph.h"

#include "xlinux/mempolicy.h"
#include "xlinux/xsignal.h"

#include "handler.h"
#include "config.h"
#include "reconfigure.h"
#include "params.h"
#include "notify_thread.h"
#include "walker.h"
#include "node.h"

#include "memblk.def.h"
#include "memblk.h"

//
// public
//

xapi handler_dispatch(fab_request * const restrict request, memblk * const restrict mb, fab_response ** const restrict response)
{
  enter;

  int mpc = 0;

  // update the graph
  fatal(walker_walk, &g_root, 0, g_params.proj_dir);

  int x;
  for(x = 0; x < request->commandsl; x++)
  {
    struct fab_command * cmd = request->commands[x];
    if(cmd->type == FAB_COMMAND_CONFIG_APPLY)
    {
      // reload the config files and apply to staging
      fatal(reconfigure_stage_files);
      break;
    }
  }

  for(x = 0; x < request->commandsl; x++)
  {
    struct fab_command * cmd = request->commands[x];
    if(cmd->type == FAB_COMMAND_CONFIG_STAGE)
    {
      fatal(reconfigure_stage, cmd->text);
    }
    else if(cmd->type == FAB_COMMAND_CONFIG_APPLY)
    {
      fatal(reconfigure);
    }
    else if(cmd->type == FAB_COMMAND_SELECT)
    {
      if((cmd->attr & FAB_ATTR_TARGET_OPT) == FAB_ATTR_TARGET)
      {
        // cmd->text
      }
    }
    else if(cmd->type == FAB_COMMAND_BUILD)
    {
    }
  }

  if(!*response)
  {
    fatal(mempolicy_push, memblk_getpolicy(mb), &mpc);
    fatal(fab_response_create, response, 0);
  }

finally:
  mempolicy_unwind(&mpc);
coda;
}
