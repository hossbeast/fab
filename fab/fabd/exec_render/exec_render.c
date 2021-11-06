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

#include "value.h"
#include "valyria/set.h"
#include "valyria/pstring.h"
#include "valyria/list.h"

#include "exec_render.h"
#include "exec.h"
#include "formula_value.internal.h"
#include "build_slot.h"
#include "path_cache.h"
#include "module.h"
#include "sysvar.h"
#include "channel.h"
#include "system_state.h"

#include "zbuffer.h"

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
  , module * restrict mod
  , const value * restrict vars
  , const build_slot * restrict bs
)
{
  ctx->base_builder = ctx->builder = builder;
  ctx->vars = vars;
  ctx->bs = bs;

  ctx->selector_context.bpe = 0;
  if(bs) {
    ctx->selector_context.bpe = bs->bpe;
  }

  ctx->selector_context.mod = mod;
}

xapi exec_render_context_xreset(exec_render_context * restrict ctx)
{
  enter;

  memset(&ctx->builder_args, 0, sizeof(ctx->builder_args));

  finally : coda;
}

xapi exec_render_context_xdestroy(struct exec_render_context * restrict ctx)
{
  enter;

  fatal(exec_builder_xdestroy, &ctx->operation_builder);
  fatal(selector_context_xdestroy, &ctx->selector_context);

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

  ctx->builder_args.item = BUILDER_ENVS;
  ctx->builder_args.position = -1;
  ctx->builder_args.render_sep = BUILDER_STRING;
  ctx->builder_args.sep.s = " ";
  ctx->builder_args.mode = BUILDER_APPEND;

  for(x = 0; x < ctx->vars->els->table_size; x++)
  {
    if((v = set_table_get(ctx->vars->els, x)) == 0)
      continue;

    RUNTIME_ASSERT(v->type == VALUE_TYPE_MAPPING);
    RUNTIME_ASSERT(v->key->type == VALUE_TYPE_STRING);

    ctx->builder_args.name = v->key->s->s;
    ctx->builder_args.name_len = v->key->s->size;
    ctx->builder_args.mode = BUILDER_APPEND;

    fatal(exec_render_value, ctx, v->val);
  }

  finally : coda;
}

xapi exec_render_file(exec_render_context * restrict ctx, const formula_value * restrict file)
{
  enter;

  fatal(exec_render_context_xreset, ctx);

  ctx->builder_args.item = BUILDER_FILE;
  fatal(exec_render_formula_value, ctx, file);

  finally : coda;
}

xapi exec_render_args(exec_render_context * restrict ctx, const formula_value * restrict args)
{
  enter;

  RUNTIME_ASSERT(args->type == FORMULA_VALUE_LIST);

  fatal(exec_render_context_xreset, ctx);

  ctx->builder_args.item = BUILDER_ARGS;
  ctx->builder_args.position = -1;
  fatal(exec_render_formula_value, ctx, args);

  finally : coda;
}

xapi exec_render_envs(exec_render_context * restrict ctx, const formula_value * restrict envs)
{
  enter;

  formula_value *mapping;

  RUNTIME_ASSERT(envs->type == FORMULA_VALUE_SET);

  fatal(exec_render_context_xreset, ctx);

  ctx->builder_args.item = BUILDER_ENVS;
  ctx->builder_args.position = -1;
  ctx->builder_args.render_sep = BUILDER_STRING;
  ctx->builder_args.sep.s = " ";
  ctx->builder_args.mode = BUILDER_APPEND;

  rbtree_foreach(envs->set, mapping, rbn) {
    RUNTIME_ASSERT(mapping->type == FORMULA_VALUE_MAPPING);

    ctx->builder_args.name = mapping->m.name;
    ctx->builder_args.name_len = mapping->m.name_len;

    fatal(exec_render_formula_value, ctx, mapping->m.value);
  }

  finally : coda;
}

xapi exec_render_value(exec_render_context * restrict ctx, const value * restrict val)
{
  enter;

  value *item;
  int x;

  if(val->type == VALUE_TYPE_LIST)
  {
    for(x = 0; x < val->items->size; x++)
    {
      fatal(exec_render_value, ctx, list_get(val->items, x));
    }
  }
  else if(val->type == VALUE_TYPE_SET)
  {
    for(x = 0; x < val->els->table_size; x++)
    {
      if((item = set_table_get(val->els, x)) == 0)
        continue;

      fatal(exec_render_value, ctx, item);
    }
  }
  else if(val->type & VALUE_TYPE_SCALAR)
  {
    ctx->builder_args.val.v = val;
    ctx->builder_args.render_val = BUILDER_VALUE;
    ctx->builder_args.mode = BUILDER_APPEND;
    fatal(exec_builder_add, ctx->builder, &ctx->builder_args);
  }

  finally : coda;
}

xapi exec_render_formula_value(exec_render_context * restrict ctx, const formula_value * val)
{
  enter;

  const chain *T;
  formula_value *sv;
  value *mapval;
  int x;
  exec * sequence_output;
  exec_builder_args base_add_args;
  selected *sn;
  const path_cache_entry *pe;

  if(val->type == FORMULA_VALUE_LIST)
  {
    chain_foreach(T, sv, chn, val->list_head) {
      fatal(exec_render_formula_value, ctx, sv);
      if(system_error) {
        goto XAPI_FINALIZE;
      }
    }
  }
  else if(val->type == FORMULA_VALUE_SET)
  {
    rbtree_foreach(val->set, sv, rbn) {
      fatal(exec_render_formula_value, ctx, sv);
      if(system_error) {
        goto XAPI_FINALIZE;
      }
    }
  }
  else if(val->type == FORMULA_VALUE_VARIABLE)
  {
    if(ctx->vars && (mapval = value_lookupw(ctx->vars, val->v.name, val->v.name_len))) {
      fatal(exec_render_value, ctx, mapval);
    }
  }
  else if(val->type == FORMULA_VALUE_SYSVAR)
  {
    if(val->sv == FORMULA_SYSVAR_VARIANT)
    {
      fatal(exec_render_sysvar_variant, ctx, ctx->bs);
    }
    else if(val->sv == FORMULA_SYSVAR_SOURCE || val->sv == FORMULA_SYSVAR_SOURCES)
    {
      fatal(exec_render_sysvar_sources, ctx, ctx->bs);
    }
    else if(val->sv == FORMULA_SYSVAR_TARGET || val->sv == FORMULA_SYSVAR_TARGETS)
    {
      fatal(exec_render_sysvar_targets, ctx, ctx->bs);
    }
  }
  else if(val->type == FORMULA_VALUE_SELECT)
  {
    fatal(selector_exec, val->op.selector, &ctx->selector_context, SELECTION_ITERATION_TYPE_ORDER);
  }
  else if(val->type == FORMULA_VALUE_PROPERTY)
  {
    base_add_args = ctx->builder_args;

    ctx->builder_args.mode = BUILDER_APPEND;
    ctx->builder_args.render_val = BUILDER_PROPERTY;
    ctx->builder_args.val.prop = val->op.property;
    ctx->builder_args.val.pctx.mod = ctx->bs->mod;
    llist_foreach(&ctx->selector_context.selection->list, sn, lln) {
      if((sn->v->attrs & VERTEX_TYPE_OPT) == VERTEX_TYPE_FSENT) {
        ctx->builder_args.val.n = containerof(sn->v, fsent, vertex);
      } else {
        RUNTIME_ASSERT((sn->v->attrs & VERTEX_TYPE_OPT) == VERTEX_TYPE_MODULE);
        ctx->builder_args.val.n = containerof(sn->v, module, vertex)->dir_node;
      }

      fatal(exec_builder_add, ctx->builder, &ctx->builder_args);
    }

    ctx->builder_args = base_add_args;
  }
  else if(val->type == FORMULA_VALUE_PREPEND)
  {
    base_add_args = ctx->builder_args;

    ctx->builder_args.render_val = BUILDER_FORMULA_VALUE;
    ctx->builder_args.val.f = val->op.operand;
    ctx->builder_args.mode = BUILDER_PREPEND;
    ctx->builder_args.render_sep = 0;

    ctx->builder_args.item = BUILDER_ARGS;
    for(x = 0; x < ctx->builder->args_len; x++)
    {
      ctx->builder_args.position = x;
      fatal(exec_builder_add, ctx->builder, &ctx->builder_args);
    }

    ctx->builder_args = base_add_args;
  }
  else if(val->type == FORMULA_VALUE_PATH_SEARCH)
  {
    /* assumes string */
    fatal(path_cache_search, &pe, MMS(val->op.operand->s));
    if(pe->fd == -1)
    {
      fprintf(stderr, "wtf?\n");
      goto XAPI_FINALLY;
    }

    ctx->builder_args.val.pe = pe;
    ctx->builder_args.mode = BUILDER_APPEND;
    ctx->builder_args.render_val = BUILDER_PATH_CACHE_ENTRY;
    fatal(exec_builder_add, ctx->builder, &ctx->builder_args);
  }
  else if(val->type == FORMULA_VALUE_SEQUENCE)
  {
    /* operations are always couched in a sequence operation */
    base_add_args = ctx->builder_args;
    fatal(exec_builder_xreset, &ctx->operation_builder);
    ctx->builder = &ctx->operation_builder;

    ctx->builder_args.item = BUILDER_ARGS;
    chain_foreach(T, sv, chn, val->op.list_head) {
      ctx->builder_args.position = -1;
      fatal(exec_render_formula_value, ctx, sv);
      if(system_error) {
        goto XAPI_FINALIZE;
      }
    }

    /* the output of the sequence is a list of strings */
    fatal(exec_builder_build, ctx->builder, &sequence_output);

    // restore
    ctx->builder = ctx->base_builder;
    ctx->builder_args = base_add_args;

    ctx->builder_args.render_val = BUILDER_STRING;
    ctx->builder_args.mode = BUILDER_APPEND;

    for(x = 0; x < sequence_output->args_size; x++)
    {
      ctx->builder_args.val.s = sequence_output->args[x];
      fatal(exec_builder_add, ctx->builder, &ctx->builder_args);
    }

    /* if the list is empty, add the key to the env anyway so that it will at least be defined */
    if(sequence_output->args_size == 0 && base_add_args.item == BUILDER_ENVS)
    {
      ctx->builder_args.val.s = "";
      fatal(exec_builder_add, ctx->builder, &ctx->builder_args);
    }

    ctx->builder_args = base_add_args;
  }
  else if(val->type & VALUE_TYPE_SCALAR)
  {
    ctx->builder_args.val.f = val;
    ctx->builder_args.mode = BUILDER_APPEND;
    ctx->builder_args.render_val = BUILDER_FORMULA_VALUE;
    fatal(exec_builder_add, ctx->builder, &ctx->builder_args);
  }
  else
  {
    RUNTIME_ABORT();
  }

  finally : coda;
}
