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

#include "types.h"
#include "xapi.h"

#include "moria/edge.h"

#include "sysvar.h"
#include "exec_builder.internal.h"
#include "buildplan_entity.h"
#include "exec_render.h"
#include "build_slot.h"
#include "module.h"

static void bpe_split(const buildplan_entity * restrict bpe, const node ** restrict n, const node_edge ** restrict ne, const edge ** restrict e)
{
  *n = 0;
  *ne = 0;
  *e = 0;

  if(bpe->typemark == BPE_NODE)
  {
    *n = containerof(bpe, typeof(**n), self_bpe);
  }
  else if(bpe->typemark == BPE_NODE_EDGE)
  {
    *ne = containerof(bpe, typeof(**ne), bpe);
    *e = edge_containerof(*ne);
  }
}

//
// public
//

xapi sysvar_builder_variant(exec_builder * restrict builder, const build_slot * restrict bs)
{
  enter;

  int x;
  const node * n;
  const node_edge * ne;
  const edge * e;
  builder_add_args args = { 0 };

  bpe_split(bs->bpe, &n, &ne, &e);

  if(n)
  {
    args.val.n = n;
  }
  else if(e && !(e->attrs & MORIA_EDGE_HYPER))
  {
    if(ne->dir == EDGE_TGT_SRC)
      args.val.n = vertex_value(e->A);
    else
      args.val.n = vertex_value(e->B);
  }
  else if(e && ne->dir == EDGE_TGT_SRC)
  {
    if(e->Alen)
    {
      args.val.n = vertex_value(e->Alist[0].v);
    }
    for(x = 1; x < e->Alen; x++)
    {
      n = vertex_value(e->Alist[x].v);
      if(n->var != args.val.n->var)
      {
        args.val.n = 0;
        break;
      }
    }
  }
  else if(e && ne->dir == EDGE_SRC_TGT)
  {
    if(e->Blen)
    {
      args.val.n = vertex_value(e->Blist[0].v);
    }
    for(x = 1; x < e->Blen; x++)
    {
      n = vertex_value(e->Blist[x].v);
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
    args.item = ENVS;
    args.position = -1;
    args.name = "bm_variant";
    args.name_len = strlen("bm_variant");
    args.render_val = RENDER_PROPERTY;
    args.val.prop = NODE_PROPERTY_VARIANT;
    args.mode = APPEND;
    fatal(builder_add, builder, &args);
  }

  finally : coda;
}

xapi sysvar_builder_targets(exec_builder * restrict builder, const build_slot * restrict bs)
{
  enter;

  int x;
  char space[64];
  builder_add_args args = { 0 };
  const node * n;
  const node_edge * ne;
  const edge * e;

  bpe_split(bs->bpe, &n, &ne, &e);

  if(n)
  {
    args.val.n = n;
  }
  else if(e && !(e->attrs & MORIA_EDGE_HYPER))
  {
    if(ne->dir == EDGE_TGT_SRC)
      args.val.n = vertex_value(e->A);
    else
      args.val.n = vertex_value(e->B);
  }
  else if(e && e->Alen == 1 && ne->dir == EDGE_TGT_SRC)
  {
    args.val.n = vertex_value(e->Alist[0].v);
  }
  else if(e && e->Blen == 1 && ne->dir == EDGE_SRC_TGT)
  {
    args.val.n = vertex_value(e->Blist[0].v);
  }

  args.item = ENVS;
  args.position = -1;
  args.render_val = RENDER_PROPERTY;
  args.val.prop = NODE_PROPERTY_RELPATH;
  RUNTIME_ASSERT(bs->mod);
  /* paths are relative to the module */
  args.val.pctx.mod = bs->mod;
  args.mode = APPEND;

  if(args.val.n)
  {
    args.name = "bm_target";
    args.name_len = strlen("bm_target");
    fatal(builder_add, builder, &args);
  }
  else if(ne->dir == EDGE_TGT_SRC)
  {
    fatal(exec_builder_env_addf, builder, "bm_targets_len", "%d", e->Alen);

    args.name = space;
    for(x = 0; x < e->Alen; x++)
    {
      args.val.n = vertex_value(e->Alist[x].v);
      args.name_len = snprintf(space, sizeof(space), "bm_target%d", x);
      fatal(builder_add, builder, &args);
    }
  }
  else if(ne->dir == EDGE_SRC_TGT)
  {
    fatal(exec_builder_env_addf, builder, "bm_targets_len", "%d", e->Blen);

    args.name = space;
    for(x = 0; x < e->Blen; x++)
    {
      args.val.n = vertex_value(e->Blist[x].v);
      args.name_len = snprintf(space, sizeof(space), "bm_target%d", x);
      fatal(builder_add, builder, &args);
    }
  }

  finally : coda;
}

xapi sysvar_builder_sources(exec_builder * restrict builder, const build_slot * restrict bs)
{
  enter;

  int x;
  char space[64];
  builder_add_args args = { 0 };
  const node * n;
  const node_edge * ne;
  const edge * e;

  bpe_split(bs->bpe, &n, &ne, &e);

  if(e && !(e->attrs & MORIA_EDGE_HYPER))
  {
    if(ne->dir == EDGE_TGT_SRC)
      args.val.n = vertex_value(e->B);
    else
      args.val.n = vertex_value(e->A);
  }
  else if(e && e->Blen == 1 && ne->dir == EDGE_TGT_SRC)
  {
    args.val.n = vertex_value(e->Blist[0].v);
  }
  else if(e && e->Alen == 1 && ne->dir == EDGE_SRC_TGT)
  {
    args.val.n = vertex_value(e->Alist[0].v);
  }

  args.item = ENVS;
  args.position = -1;
  args.render_val = RENDER_PROPERTY;
  args.val.prop = NODE_PROPERTY_RELPATH;
  args.val.pctx.mod = bs->mod;
  args.mode = APPEND;

  if(args.val.n)
  {
    args.name = "bm_source";
    args.name_len = strlen("bm_source");
    fatal(builder_add, builder, &args);
  }
  else if(e && ne->dir == EDGE_TGT_SRC)
  {
    fatal(exec_builder_env_addf, builder, "bm_sources_len", "%d", e->Blen);
    args.name = space;
    for(x = 0; x < e->Blen; x++)
    {
      args.val.n = vertex_value(e->Blist[x].v);
      args.name_len = snprintf(space, sizeof(space), "bm_source%d", x);
      fatal(builder_add, builder, &args);
    }
  }
  else if(e && ne->dir == EDGE_SRC_TGT)
  {
    fatal(exec_builder_env_addf, builder, "bm_sources_len", "%d", e->Alen);
    args.name = space;
    for(x = 0; x < e->Alen; x++)
    {
      args.val.n = vertex_value(e->Alist[x].v);
      args.name_len = snprintf(space, sizeof(space), "bm_source%d", x);

      fatal(builder_add, builder, &args);
    }
  }

  finally : coda;
}

xapi exec_render_sysvar_sources(exec_render_context * restrict ctx, const build_slot * restrict bs)
{
  enter;

  int x;
  const node * n;
  const node_edge * ne;
  const edge * e;

  bpe_split(bs->bpe, &n, &ne, &e);

  ctx->builder_add_args.val.n = 0;
  if(e && !(e->attrs & MORIA_EDGE_HYPER))
  {
    if(ne->dir == EDGE_TGT_SRC)
      ctx->builder_add_args.val.n = vertex_value(e->B);
    else
      ctx->builder_add_args.val.n = vertex_value(e->A);
  }
  else if(e && e->Blen == 1 && ne->dir == EDGE_TGT_SRC)
  {
    ctx->builder_add_args.val.n = vertex_value(e->Blist[0].v);
  }
  else if(e && e->Alen == 1 && ne->dir == EDGE_SRC_TGT)
  {
    ctx->builder_add_args.val.n = vertex_value(e->Alist[0].v);
  }

  ctx->builder_add_args.render_val = RENDER_PROPERTY;
  ctx->builder_add_args.val.prop = NODE_PROPERTY_RELPATH;
  ctx->builder_add_args.val.pctx.mod = bs->mod;

  if(ctx->builder_add_args.val.n)
  {
    fatal(builder_add, ctx->builder, &ctx->builder_add_args);
  }
  else if(e && ne->dir == EDGE_TGT_SRC)
  {
    for(x = 0; x < e->Blen; x++)
    {
      ctx->builder_add_args.val.n = vertex_value(e->Blist[x].v);
      fatal(builder_add, ctx->builder, &ctx->builder_add_args);
    }
  }
  else if(e && ne->dir == EDGE_SRC_TGT)
  {
    for(x = 0; x < e->Alen; x++)
    {
      ctx->builder_add_args.val.n = vertex_value(e->Alist[x].v);
      fatal(builder_add, ctx->builder, &ctx->builder_add_args);
    }
  }

  finally : coda;
}

xapi exec_render_sysvar_targets(exec_render_context * restrict ctx, const build_slot * restrict bs)
{
  enter;

  int x;
  const node * n;
  const node_edge * ne;
  const edge * e;

  bpe_split(bs->bpe, &n, &ne, &e);

  ctx->builder_add_args.val.n = 0;
  if(n)
  {
    ctx->builder_add_args.val.n = n;
  }
  else if(e && !(e->attrs & MORIA_EDGE_HYPER))
  {
    if(ne->dir == EDGE_TGT_SRC)
      ctx->builder_add_args.val.n = vertex_value(e->A);
    else
      ctx->builder_add_args.val.n = vertex_value(e->B);
  }
  else if(e && e->Alen == 1 && ne->dir == EDGE_TGT_SRC)
  {
    ctx->builder_add_args.val.n = vertex_value(e->Alist[0].v);
  }
  else if(e && e->Blen == 1 && ne->dir == EDGE_SRC_TGT)
  {
    ctx->builder_add_args.val.n = vertex_value(e->Alist[0].v);
  }

  ctx->builder_add_args.render_val = RENDER_PROPERTY;
  ctx->builder_add_args.val.prop = NODE_PROPERTY_RELPATH;
  ctx->builder_add_args.val.pctx.mod = bs->mod;

  if(ctx->builder_add_args.val.n)
  {
    fatal(builder_add, ctx->builder, &ctx->builder_add_args);
  }
  else if(e && ne->dir == EDGE_TGT_SRC)
  {
    for(x = 0; x < e->Alen; x++)
    {
      ctx->builder_add_args.val.n = vertex_value(e->Alist[x].v);
      fatal(builder_add, ctx->builder, &ctx->builder_add_args);
    }
  }
  else if(e && ne->dir == EDGE_SRC_TGT)
  {
    for(x = 0; x < e->Blen; x++)
    {
      ctx->builder_add_args.val.n = vertex_value(e->Blist[x].v);
      fatal(builder_add, ctx->builder, &ctx->builder_add_args);
    }
  }

  finally : coda;
}

xapi exec_render_sysvar_variant(exec_render_context * restrict ctx, const build_slot * restrict bs)
{
  enter;

  int x;
  const node * n;
  const node_edge * ne;
  const edge * e;

  bpe_split(bs->bpe, &n, &ne, &e);

  ctx->builder_add_args.val.n = 0;
  if(n)
  {
    ctx->builder_add_args.val.n = n;
  }
  else if(e && !(e->attrs & MORIA_EDGE_HYPER))
  {
    if(ne->dir == EDGE_TGT_SRC)
      ctx->builder_add_args.val.n = vertex_value(e->A);
    else
      ctx->builder_add_args.val.n = vertex_value(e->B);
  }
  else if(e && ne->dir == EDGE_TGT_SRC)
  {
    if(e->Alen)
    {
      ctx->builder_add_args.val.n = vertex_value(e->Alist[0].v);
    }
    for(x = 1; x < e->Alen; x++)
    {
      n = vertex_value(e->Alist[x].v);
      if(n->var != ctx->builder_add_args.val.n->var)
      {
        ctx->builder_add_args.val.n = 0;
        break;
      }
    }
  }
  else if(e && ne->dir == EDGE_SRC_TGT)
  {
    if(e->Blen)
    {
      ctx->builder_add_args.val.n = vertex_value(e->Blist[0].v);
    }
    for(x = 1; x < e->Blen; x++)
    {
      n = vertex_value(e->Blist[x].v);
      if(n->var != ctx->builder_add_args.val.n->var)
      {
        ctx->builder_add_args.val.n = 0;
        break;
      }
    }
  }

  if(ctx->builder_add_args.val.n)
  {
    ctx->builder_add_args.render_val = RENDER_PROPERTY;
    ctx->builder_add_args.val.prop = NODE_PROPERTY_VARIANT;
    ctx->builder_add_args.mode = APPEND;

    fatal(builder_add, ctx->builder, &ctx->builder_add_args);
  }

  finally : coda;
}
