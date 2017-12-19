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

#include "moria/graph.h"
#include "moria/vertex.h"
#include "narrator.h"
#include "valyria/dictionary.h"
#include "valyria/list.h"
#include "xlinux/mempolicy.h"
#include "xlinux/xsignal.h"
#include "xlinux/xstdlib.h"

#include "fab/request.def.h"
#include "fab/command.def.h"
#include "fab/response.h"
#include "fab/ipc.h"

#include "handler.h"
#include "artifact.h"
#include "config.h"
#include "logging.h"
#include "module.h"
#include "node.h"
#include "notify_thread.h"
#include "params.h"
#include "reconfigure.h"
#include "walker.h"
#include "buildplan.h"

#include "memblk.def.h"
#include "memblk.h"
#include "macros.h"

static xapi update_graph(void)
{
  enter;

  // 1. allocate a walk_id
  // 1.1. call walker_walk for each source dir, for each module, with that walk_id

  /* int walk_id = walker_walk_begin(); */

  /* fatal(walker_walk, &g_root, 0, g_params.proj_dir); */

  finally : coda;
}

//
// static
//

static xapi handler_build_visitor(vertex * v, int distance, void * arg)
{
  enter;

  node * n = vertex_value(v);
  buildplan * bp = arg;

  fatal(buildplan_add, bp, n, distance);

  finally : coda;
}

//
// public
//

xapi handler_context_create(handler_context ** restrict ctx)
{
  enter;

  fatal(xmalloc, ctx, sizeof(**ctx));
  fatal(buildplan_create, &(*ctx)->bp);

  finally : coda;
}

xapi handler_context_xfree(handler_context * restrict ctx)
{
  enter;

  if(ctx)
  {
    fatal(buildplan_xfree, ctx->bp);
  }
  wfree(ctx);

  finally : coda;
}

xapi handler_context_ixfree(handler_context ** restrict ctx)
{
  enter;

  fatal(handler_context_xfree, *ctx);
  *ctx = 0;

  finally : coda;
}

xapi handler_build(handler_context * restrict ctx)
{
  enter;

  // create build plan to update all targets
  int traversal_id = graph_traversal_begin(g_node_graph);
  fatal(buildplan_reset, ctx->bp, traversal_id);
  
  int x;
  int y;
  for(x = 0; x < g_modules->table_size; x++)
  {
    module * mod = dictionary_table_value(g_modules, x);
    if(!mod)
      continue;

    for(y = 0; y < mod->artifacts->l; y++)
    {
      artifact * af = list_get(mod->artifacts, y);

      fatal(graph_traverse_vertices
        , g_node_graph
        , vertex_containerof(af->node)
        , handler_build_visitor
        , 0
        , (traversal_criteria[]) {{
              edge_travel : RELATION_TYPE_STRONG | RELATION_TYPE_WEAK   // travel weak edges
            , edge_visit : RELATION_TYPE_STRONG                         // callback on strong edges
          }}
        , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_POST
        , ctx->bp
      );
    }
  }

  fatal(buildplan_finalize, ctx->bp);
  fatal(buildplan_report, ctx->bp);

  finally : coda;
}

xapi handler_dispatch(handler_context * restrict ctx, fab_request * restrict request, memblk * restrict mb, fab_response ** restrict response)
{
  enter;

  int mpc = 0;

  fatal(update_graph);

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
      fatal(handler_build, ctx);
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
