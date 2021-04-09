/* Copyright (c) 2012-2019 Todd Freed <todd.freed@gmail.com>

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

#include "sysvar.h"
#include "build_slot.h"
#include "dependency.h"
#include "exec_builder.h"
#include "exec_render.h"

//
// public
//

xapi sysvar_builder_variant(exec_builder * restrict builder, const build_slot * restrict bs)
{
  enter;

  int x;
  const fsent *n;
  const moria_edge * e;
  exec_builder_args args = { };

  e = &bs->bpe->edge;
  RUNTIME_ASSERT(e);

  if(!(e->attrs & MORIA_EDGE_HYPER))
  {
    args.val.n = containerof(e->A, fsent, vertex);
  }
  else
  {
    if(e->Alen)
    {
      args.val.n = containerof(e->Alist[0].v, fsent, vertex);
    }
    for(x = 1; x < e->Alen; x++)
    {
      n = containerof(e->Alist[x].v, fsent, vertex);
      if(n->var != args.val.n->var)
      {
        args.val.n = 0;
        break;
      }
    }
  }

  /* if there's only one target, or if all targets have the same variant */
  if(args.val.n)
  {
    args.item = BUILDER_ENVS;
    args.position = -1;
    args.name = "bm_variant";
    args.name_len = strlen("bm_variant");
    args.render_val = BUILDER_PROPERTY;
    args.val.prop = FSENT_PROPERTY_VARIANT;
    args.mode = BUILDER_APPEND;
    fatal(exec_builder_add, builder, &args);
  }

  finally : coda;
}

xapi sysvar_builder_targets(exec_builder * restrict builder, const build_slot * restrict bs)
{
  enter;

  int x;
  char space[64];
  exec_builder_args args = { };
  const moria_edge * e;

  e = &bs->bpe->edge;

  if(!(e->attrs & MORIA_EDGE_HYPER))
  {
    args.val.n = containerof(e->A, fsent, vertex);
  }
  else if(e->Alen == 1)
  {
    args.val.n = containerof(e->Alist[0].v, fsent, vertex);
  }

  args.item = BUILDER_ENVS;
  args.position = -1;
  args.render_val = BUILDER_PROPERTY;
  args.val.prop = FSENT_PROPERTY_RELPATH;
  RUNTIME_ASSERT(bs->mod);
  /* paths are relative to the module */
  args.val.pctx.mod = bs->mod;

  args.mode = BUILDER_APPEND;

  if(args.val.n)
  {
    args.name = "bm_target";
    args.name_len = strlen("bm_target");
    fatal(exec_builder_add, builder, &args);
  }
  else
  {
    fatal(exec_builder_env_addf, builder, "bm_targets_len", "%d", e->Alen);

    args.name = space;
    for(x = 0; x < e->Alen; x++)
    {
      args.val.n = containerof(e->Alist[x].v, fsent, vertex);
      args.name_len = snprintf(space, sizeof(space), "bm_target%d", x);
      fatal(exec_builder_add, builder, &args);
    }
  }

  finally : coda;
}

xapi sysvar_builder_sources(exec_builder * restrict builder, const build_slot * restrict bs)
{
  enter;

  int x;
  char space[64];
  exec_builder_args args = { };
  const moria_edge * e;

  e = &bs->bpe->edge;

  if(!(e->attrs & MORIA_EDGE_HYPER))
  {
    args.val.n = containerof(e->B, fsent, vertex);
  }
  else if(e->Blen == 1)
  {
    args.val.n = containerof(e->Blist[0].v, fsent, vertex);
  }

  args.item = BUILDER_ENVS;
  args.position = -1;
  args.render_val = BUILDER_PROPERTY;
  args.val.prop = FSENT_PROPERTY_RELPATH;
  args.val.pctx.mod = bs->mod;
  args.mode = BUILDER_APPEND;

  if(args.val.n)
  {
    args.name = "bm_source";
    args.name_len = strlen("bm_source");
    fatal(exec_builder_add, builder, &args);
  }
  else
  {
    fatal(exec_builder_env_addf, builder, "bm_sources_len", "%d", e->Blen);
    args.name = space;
    for(x = 0; x < e->Blen; x++)
    {
      args.val.n = containerof(e->Blist[x].v, fsent, vertex);
      args.name_len = snprintf(space, sizeof(space), "bm_source%d", x);

      fatal(exec_builder_add, builder, &args);
    }
  }

  finally : coda;
}

xapi exec_render_sysvar_sources(exec_render_context * restrict ctx, const build_slot * restrict bs)
{
  enter;

  int x;
  const moria_edge * e;

  e = &bs->bpe->edge;

  ctx->builder_args.val.n = 0;
  if(!(e->attrs & MORIA_EDGE_HYPER))
  {
    ctx->builder_args.val.n = containerof(e->B, fsent, vertex);
  }
  else if(e->Blen == 1)
  {
    ctx->builder_args.val.n = containerof(e->Blist[0].v, fsent, vertex);
  }

  ctx->builder_args.render_val = BUILDER_PROPERTY;
  ctx->builder_args.val.prop = FSENT_PROPERTY_RELPATH;
  ctx->builder_args.val.pctx.mod = bs->mod;
  ctx->builder_args.mode = BUILDER_APPEND;

  if(ctx->builder_args.val.n)
  {
    fatal(exec_builder_add, ctx->builder, &ctx->builder_args);
  }
  else
  {
    for(x = 0; x < e->Blen; x++)
    {
      ctx->builder_args.val.n = containerof(e->Blist[x].v, fsent, vertex);
      fatal(exec_builder_add, ctx->builder, &ctx->builder_args);
    }
  }

  finally : coda;
}

xapi exec_render_sysvar_targets(exec_render_context * restrict ctx, const build_slot * restrict bs)
{
  enter;

  int x;
  const moria_edge * e;

  e = &bs->bpe->edge;

  ctx->builder_args.val.n = 0;
  if(!(e->attrs & MORIA_EDGE_HYPER))
  {
    ctx->builder_args.val.n = containerof(e->A, fsent, vertex);
  }
  else if(e->Alen == 1)
  {
    ctx->builder_args.val.n = containerof(e->Alist[0].v, fsent, vertex);
  }

  ctx->builder_args.render_val = BUILDER_PROPERTY;
  ctx->builder_args.val.prop = FSENT_PROPERTY_RELPATH;
  ctx->builder_args.val.pctx.mod = bs->mod;

  if(ctx->builder_args.val.n)
  {
    fatal(exec_builder_add, ctx->builder, &ctx->builder_args);
  }
  else
  {
    for(x = 0; x < e->Alen; x++)
    {
      ctx->builder_args.val.n = containerof(e->Alist[x].v, fsent, vertex);
      fatal(exec_builder_add, ctx->builder, &ctx->builder_args);
    }
  }

  finally : coda;
}

xapi exec_render_sysvar_variant(exec_render_context * restrict ctx, const build_slot * restrict bs)
{
  enter;

  int x;
  const fsent *n;
  const moria_edge * e;

  e = &bs->bpe->edge;

  ctx->builder_args.val.n = 0;
  if(!(e->attrs & MORIA_EDGE_HYPER))
  {
    ctx->builder_args.val.n = containerof(e->A, fsent, vertex);
  }
  else
  {
    if(e->Alen)
    {
      ctx->builder_args.val.n = containerof(e->Alist[0].v, fsent, vertex);
    }
    for(x = 1; x < e->Alen; x++)
    {
      n = containerof(e->Alist[x].v, fsent, vertex);
      if(n->var != ctx->builder_args.val.n->var)
      {
        ctx->builder_args.val.n = 0;
        break;
      }
    }
  }

  /* if there's only one target, or if all targets have the same variant */
  if(ctx->builder_args.val.n)
  {
    ctx->builder_args.render_val = BUILDER_PROPERTY;
    ctx->builder_args.val.prop = FSENT_PROPERTY_VARIANT;
    ctx->builder_args.mode = BUILDER_APPEND;

    fatal(exec_builder_add, ctx->builder, &ctx->builder_args);
  }

  finally : coda;
}
