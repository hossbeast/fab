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

#include "value.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "narrator/fixed.h"
#include "valyria/set.h"
#include "valyria/list.h"
#include "valyria/pstring.h"
#include "valyria/map.h"
#include "value/parser.h"
#include "moria/edge.h"
#include "xlinux/xstdlib.h"

#include "exec_render.h"
#include "exec_builder.internal.h"
#include "node.h"
#include "path.h"
#include "logging.h"
#include "variant.h"
#include "formula.internal.h"
#include "formula_value.internal.h"
#include "build_thread.h"
#include "build_slot.h"
#include "module.h"
#include "selector.h"
#include "params.h"
#include "sysvar.h"

#include "snarf.h"
#include "assure.h"
#include "grow.h"

//
// internal
//

xapi exec_builder_env_addf(exec_builder * restrict builder, const char * restrict name_fmt, const char * restrict val_fmt, ...)
{
  enter;

  va_list va;
  char name[64];
  size_t namel;
  struct builder_add_args args = { 0 };

  va_start(va, val_fmt);

  namel = vsnprintf(name, sizeof(name), name_fmt, va);

  args.item = ENVS;
  args.position = -1;
  args.name = name;
  args.name_len = namel;
  args.render_val = RENDER_FMT;
  args.val.fmt = val_fmt;
  args.val.va = &va;
  args.mode = APPEND;

  args.render_sep = RENDER_STRING;
  args.sep.s = " ";
  fatal(builder_add, builder, &args);

  finally : coda;
}

xapi exec_render_context_xinit(struct exec_render_context * restrict ctx)
{
  enter;

  memset(ctx, 0, sizeof(*ctx));
  fatal(exec_builder_xinit, &ctx->operation_builder);

  finally : coda;
}

void exec_render_context_configure(
    exec_render_context * restrict ctx
  , exec_builder * restrict builder
  , const module * restrict mod
  , const value * restrict vars
  , const build_slot * restrict bs
)
{
  ctx->base_builder = ctx->builder = builder;
  ctx->vars = vars;
  ctx->bs = bs;

  ctx->selector_context.bpe = 0;
  if(bs)
    ctx->selector_context.bpe = bs->bpe;

  ctx->selector_context.mod = mod;
  ctx->selector_context.base = 0;
  if(mod)
    ctx->selector_context.base = mod->dir_node;
}

xapi exec_render_context_xreset(exec_render_context * restrict ctx)
{
  enter;

  fatal(exec_builder_xreset, &ctx->operation_builder);
  memset(&ctx->builder_add_args, 0, sizeof(ctx->builder_add_args));

  finally : coda;
}

xapi exec_render_context_xdestroy(struct exec_render_context * restrict ctx)
{
  enter;

  fatal(exec_builder_xdestroy, &ctx->operation_builder);
  fatal(selector_context_xdestroy, &ctx->selector_context);

  finally : coda;
}

//
//
//

xapi exec_render_path(exec_render_context * restrict ctx, const formula_value * restrict path)
{
  enter;

  fatal(exec_render_context_xreset, ctx);

  ctx->builder_add_args.item = PATH;
  fatal(exec_render_formula_value, path, ctx);

  finally : coda;
}

xapi exec_render_args(exec_render_context * restrict ctx, const formula_value * restrict args)
{
  enter;

  RUNTIME_ASSERT(args->type == FORMULA_VALUE_LIST);

  fatal(exec_render_context_xreset, ctx);

  ctx->builder_add_args.item = ARGS;
  ctx->builder_add_args.position = -1;
  fatal(exec_render_formula_value, args, ctx);

  finally : coda;
}

xapi exec_render_envs(exec_render_context * restrict ctx, const formula_value * restrict envs)
{
  enter;

  formula_value *mapping;

  RUNTIME_ASSERT(envs->type == FORMULA_VALUE_SET);

  fatal(exec_render_context_xreset, ctx);

  ctx->builder_add_args.item = ENVS;
  ctx->builder_add_args.position = -1;
  ctx->builder_add_args.render_sep = RENDER_STRING;
  ctx->builder_add_args.sep.s = " ";
  ctx->builder_add_args.mode = APPEND;

  rbtree_foreach(envs->set, mapping, rbn) {
    RUNTIME_ASSERT(mapping->type == FORMULA_VALUE_MAPPING);

    ctx->builder_add_args.name = mapping->m.name;
    ctx->builder_add_args.name_len = mapping->m.name_len;

    fatal(exec_render_formula_value, mapping->m.value, ctx);
  }

  finally : coda;
}

xapi exec_render_env_sysvars(exec_render_context * restrict ctx, const build_slot * restrict bs)
{
  enter;

  fatal(exec_render_context_xreset, ctx);

  /* targets variant */
  fatal(sysvar_builder_variant, ctx->builder, bs);

  /* targets paths */
  fatal(sysvar_builder_targets, ctx->builder, bs);

  /* sources paths */
  fatal(sysvar_builder_sources, ctx->builder, bs);

  finally : coda;
}

xapi exec_render_env_vars(exec_render_context * restrict ctx)
{
  enter;

  int x;
  value * v;

  fatal(exec_render_context_xreset, ctx);

  ctx->builder_add_args.item = ENVS;
  ctx->builder_add_args.position = -1;
  ctx->builder_add_args.render_sep = RENDER_STRING;
  ctx->builder_add_args.sep.s = " ";
  ctx->builder_add_args.mode = APPEND;

  for(x = 0; x < ctx->vars->els->table_size; x++)
  {
    if((v = set_table_get(ctx->vars->els, x)) == 0)
      continue;

    RUNTIME_ASSERT(v->type == VALUE_TYPE_MAPPING);
    RUNTIME_ASSERT(v->key->type == VALUE_TYPE_STRING);

    ctx->builder_add_args.name = v->key->s->s;
    ctx->builder_add_args.name_len = v->key->s->size;
    ctx->builder_add_args.mode = APPEND;

    fatal(exec_render_value, v->val, ctx);
  }

  finally : coda;
}
