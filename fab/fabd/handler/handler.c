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

#include "xlinux/KERNEL.errtab.h"
#include "xlinux/xstring.h"

#include "moria/graph.h"
#include "moria/vertex.h"
#include "moria/edge.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "narrator/fixed.h"
#include "valyria/dictionary.h"
#include "valyria/list.h"
#include "valyria/llist.h"
#include "valyria/set.h"
#include "xlinux/mempolicy.h"
#include "xlinux/xsignal.h"
#include "xlinux/xstdlib.h"
#include "value/writer.h"

#include "fab/ipc.h"

#include "handler.h"
#include "buildplan.h"
#include "config.h"
#include "formula.h"
#include "logging.h"
#include "module.internal.h"
#include "node.h"
#include "notify_thread.h"
#include "params.h"
#include "path.h"
#include "config.internal.h"
#include "walker.h"
#include "request.internal.h"
#include "selector.h"
#include "REQUEST.errtab.h"
#include "goals.h"
#include "server_thread.h"
#include "node_operations.h"

#include "macros.h"
#include "attrs.h"

//
// handlers
//

static xapi handler_select(handler_context * restrict ctx, command * restrict cmd)
{
  enter;

  ctx->sel_ctx.bpe = 0;
  ctx->sel_ctx.base = g_project_root->mod->dir_node;
  ctx->sel_ctx.mod = g_project_root->mod;

  fatal(selector_exec, cmd->selector, &ctx->sel_ctx);

  // active selection
  ctx->selection = ctx->sel_ctx.selection;

  finally : coda;
}

static xapi handler_reset_selection(handler_context * restrict ctx, command * restrict cmd)
{
  enter;

  ctx->selection = 0;

  finally : coda;
}

static xapi handler_list(handler_context * restrict ctx, command * restrict cmd, value_writer * restrict writer)
{
  enter;

  char space[512];
  char nstor[NARRATOR_STATIC_SIZE];
  narrator * N;
  node_property_context pctx;
  selected_node *sn;

  fatal(value_writer_push_mapping, writer);
  fatal(value_writer_string, writer, "list");
  fatal(value_writer_push_list, writer);

  pctx.mod = g_project_root->mod;
  llist_foreach(&ctx->selection->list, sn, lln) {
    N = narrator_fixed_init(nstor, space, sizeof(space));
    fatal(node_property_say, sn->n, cmd->property, &pctx, N);
    size_t len = narrator_growing_size(N);
    fatal(value_writer_bytes, writer, space, len);
  }

  fatal(value_writer_pop_list, writer);
  fatal(value_writer_pop_mapping, writer);

  finally : coda;
}

static xapi handler_invalidate(handler_context * restrict ctx, command * restrict cmd, value_writer * restrict writer)
{
  enter;

  selected_node *sn;

  llist_foreach(&ctx->selection->list, sn, lln) {
    fatal(node_invalidate, sn->n, ctx->invalidation);
  }

  finally : coda;
}

static xapi handler_describe(handler_context * restrict ctx, command * restrict cmd, value_writer * restrict writer)
{
  enter;

  char space[512];
  char nstor[NARRATOR_STATIC_SIZE];
  narrator * N;
  node_property_context pctx;
  selected_node *sn;
  size_t len;
  int x;

  fatal(value_writer_push_mapping, writer);
  fatal(value_writer_string, writer, "describe");
  fatal(value_writer_push_list, writer);

  pctx.mod = g_project_root->mod;
  llist_foreach(&ctx->selection->list, sn, lln) {
    fatal(value_writer_push_mapping, writer);
    fatal(value_writer_bytes, writer, sn->n->name->name, sn->n->name->namel);
    fatal(value_writer_push_set, writer);
    for(x = 0; x < node_property_attrs->num; x++)
    {
      N = narrator_fixed_init(nstor, space, sizeof(space));
      fatal(node_property_say, sn->n, node_property_attrs->members[x].value, &pctx, N);
      len = narrator_growing_size(N);

      fatal(value_writer_push_mapping, writer);
        fatal(value_writer_bytes, writer, node_property_attrs->members[x].name, node_property_attrs->members[x].namel);
        fatal(value_writer_bytes, writer, space, len);
      fatal(value_writer_pop_mapping, writer);
    }
    fatal(value_writer_pop_set, writer);
    fatal(value_writer_pop_mapping, writer);
  }

  fatal(value_writer_pop_list, writer);
  fatal(value_writer_pop_mapping, writer);

  finally : coda;
}

static xapi handler_goals(handler_context * restrict ctx, command * restrict cmd)
{
  enter;

  fatal(goals_set, cmd->goals.build, cmd->goals.script, cmd->goals.target_direct, cmd->goals.target_transitive);

  finally : coda;
}


static xapi handler_autorun(handler_context * restrict ctx)
{
  enter;

  g_server_autorun = true;

  finally : coda;
}

static xapi handler_run(handler_context * restrict ctx)
{
  enter;

  g_server_autorun = false;

  finally : coda;
}

//
// public
//

xapi handler_context_create(handler_context ** restrict rv)
{
  enter;

  handler_context * ctx;
  fatal(xmalloc, &ctx, sizeof(*ctx));

  fatal(handler_context_reset, ctx);

  *rv = ctx;

  finally : coda;
}

xapi handler_context_reset(handler_context * restrict ctx)
{
  enter;

  ctx->selection = 0;

  finally : coda;
}

xapi handler_context_xfree(handler_context * restrict ctx)
{
  enter;

  if(ctx)
  {
    fatal(selector_context_xdestroy, &ctx->sel_ctx);
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

xapi handler_dispatch(handler_context * restrict ctx, request * restrict req, value_writer * restrict response_writer)
{
  enter;

  int x;
  bool staging;
  bool reconfiguring;

  // validation
  staging = false;
  reconfiguring = false;

  for(x = 0; x < req->commands->size; x++)
  {
    command * cmd = array_get(req->commands, x);
    if(cmd->type == COMMAND_STAGE_CONFIG)
    {
      if(reconfiguring)
        break;

      staging = true;
    }
    else if(cmd->type == COMMAND_RECONFIGURE)
    {
      if(reconfiguring || !staging)
        break;

      reconfiguring = true;
    }
  }

  if(x < req->commands->size)
    fail(REQUEST_ILLCONFIG);

  if(staging)
    fatal(config_begin_staging);

  for(x = 0; x < req->commands->size; x++)
  {
    command * cmd = array_get(req->commands, x);

    if(cmd->type == COMMAND_STAGE_CONFIG)
    {
      fatal(config_stage, &cmd->config);
    }
    else if(cmd->type == COMMAND_RECONFIGURE)
    {
      fatal(config_reconfigure);
    }
    else if(cmd->type == COMMAND_SELECT)
    {
      fatal(handler_select, ctx, cmd);
    }
    else if(cmd->type == COMMAND_RESET_SELECTION)
    {
      fatal(handler_reset_selection, ctx, cmd);
    }
    else if(cmd->type == COMMAND_LIST)
    {
      fatal(handler_list, ctx, cmd, response_writer);
    }
    else if(cmd->type == COMMAND_INVALIDATE)
    {
      fatal(handler_invalidate, ctx, cmd, response_writer);
    }
    else if(cmd->type == COMMAND_DESCRIBE)
    {
      fatal(handler_describe, ctx, cmd, response_writer);
    }
    else if(cmd->type == COMMAND_GOALS)
    {
      fatal(handler_goals, ctx, cmd);
    }

    /* only permitted as the final command in the request */
    else if(cmd->type == COMMAND_RUN)
    {
      fatal(handler_run, ctx);
    }
    else if(cmd->type == COMMAND_AUTORUN)
    {
      fatal(handler_autorun, ctx);
    }

    else
    {
      RUNTIME_ABORT();
    }
  }

  finally : coda;
}
