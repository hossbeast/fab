/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "bacon/value.h"
#include "bacon/set.h"

#include "exec.h"
#include "exec_value.h"
#include "build_slot.h"
#include "module.h"
#include "system.h"
#include "vertex.h"
#include "resolve.h"
#include "lookup.h"


void exec_render_context_init(struct exec_render_context * restrict ctx)
{
  memset(ctx, 0, sizeof(*ctx));
  exec_builder_init(&ctx->operation_builder);
  selector_context_init(&ctx->selector_context);
}

void exec_render_context_configure(
    exec_render_context * restrict ctx
  , exec_builder * restrict builder
  , const fsent * restrict cwd
  , const module * restrict mod
  , const bacon_value * restrict modvars
  , const bacon_value * restrict sysvars
  , const build_slot * restrict bs
)
{
  ctx->base_builder = ctx->builder = builder;
  ctx->modvars = modvars;
  ctx->sysvars = sysvars;
  ctx->bs = bs;
  ctx->cwd = cwd;
  ctx->mod = mod;

  ctx->selector_context.bpe = 0;
  if(bs) {
    ctx->selector_context.bpe = bs->bpe;
  }

  ctx->selector_context.mod = mod;
  ctx->selector_context.cwd = cwd;
}

void exec_render_context_reset(exec_render_context * restrict ctx)
{
  memset(&ctx->builder_args, 0, sizeof(ctx->builder_args));
}

void exec_render_context_destroy(struct exec_render_context * restrict ctx)
{
  exec_builder_destroy(&ctx->operation_builder);
  selector_context_destroy(&ctx->selector_context);
}

void exec_render_env_vars(exec_render_context * restrict ctx)
{
  if(ctx->sysvars) {
    exec_render_envs(ctx, ctx->sysvars);
  }
  if(ctx->modvars) {
    exec_render_envs(ctx, ctx->modvars);
  }
}

void exec_render_file(exec_render_context * restrict ctx, const bacon_value * restrict file)
{
  exec_render_context_reset(ctx);

  ctx->builder_args.item = BUILDER_FILE;
  exec_render_exec_value(ctx, file);
}

void exec_render_args(exec_render_context * restrict ctx, const bacon_value * restrict args)
{
  RUNTIME_ASSERT(args->type == BACON_TYPE_LIST);

  exec_render_context_reset(ctx);

  ctx->builder_args.item = BUILDER_ARGS;
  ctx->builder_args.position = -1;
  exec_render_exec_value(ctx, args);
}

void exec_render_envs(exec_render_context * restrict ctx, const bacon_value * restrict envs)
{
  struct exec_builder_args args;
  bacon_value *mapping;

  RUNTIME_ASSERT(envs->type == BACON_TYPE_SET);

  exec_render_context_reset(ctx);

  ctx->builder_args.item = BUILDER_ENVS;
  ctx->builder_args.position = -1;
  ctx->builder_args.render_sep = BUILDER_STRING;
  ctx->builder_args.sep.s = " ";
  ctx->builder_args.mode = BUILDER_APPEND;

  rbtree_foreach(&envs->set, mapping, rbn) {
    RUNTIME_ASSERT(mapping->type == BACON_TYPE_MAPPING);
    RUNTIME_ASSERT(mapping->key->type == BACON_TYPE_STRING);

    ctx->builder_args.name = mapping->key->s;
    ctx->builder_args.name_len = mapping->key->len;

    exec_render_exec_value(ctx, mapping->val);
    if(system_has_error) {
      return;
    }

    /* ensure the env will at least be defined */
    memcpy(&args, &ctx->builder_args, sizeof(ctx->builder_args));
    args.val.s = "";
    args.render_val = BUILDER_STRING;
    args.render_sep = 0;
    args.mode = BUILDER_APPEND;
    exec_builder_add(ctx->builder, &args);
  }
}

void exec_render_exec_value(exec_render_context * restrict ctx, const bacon_value *val)
{
  exec_value *ev;
  bacon_value *sv;
  bacon_value *mapval;
  int x;
  exec *sequence_output;
  exec_builder_args base_add_args;
  selected *sn;
  const moria_vertex *v;
  const fsent *fse;

  /* system_has_error can happen here in selector_exec - if a selector fails to resolve
   * a namespace for example */

  switch((int)val->type) {
    case BACON_TYPE_LIST:
      llist_foreach(&val->list, sv, lln) {
        exec_render_exec_value(ctx, sv);
        if(system_has_error) {
          return;
        }
      }
      break;
    case BACON_TYPE_SET:
      rbtree_foreach(&val->set, sv, rbn) {
        exec_render_exec_value(ctx, sv);
        if(system_has_error) {
          return;
        }
      }
      break;
    case BACON_TYPE_VARIABLE:
      if(ctx->modvars && (mapval = bacon_set_lookupw(ctx->modvars, val->s, val->len))) {
        exec_render_exec_value(ctx, mapval);
      } else if(ctx->sysvars && (mapval = bacon_set_lookupw(ctx->sysvars, val->s, val->len))) {
        exec_render_exec_value(ctx, mapval);
      }
      break;
    case EXEC_VALUE_SELECT:
      ev = containerof(val, typeof(*ev), bacon);
      ctx->selector_context.iteration_type = SELECTION_ITERATION_TYPE_ORDER;
      selector_exec(ev->select, &ctx->selector_context);
      break;
    case EXEC_VALUE_PROPERTY:
      ev = containerof(val, typeof(*ev), bacon);
      base_add_args = ctx->builder_args;

      ctx->builder_args.mode = BUILDER_APPEND;
      ctx->builder_args.render_val = BUILDER_PROPERTY;
      ctx->builder_args.val.prop = ev->property;
      ctx->builder_args.val.pctx.cwd = ctx->cwd;
      llist_foreach(&ctx->selector_context.selection->list, sn, lln) {
        if(vertex_kind_get(sn->v) == VERTEX_KIND_FSENT) {
          ctx->builder_args.val.n = containerof(sn->v, fsent, vertex);
        } else {
          RUNTIME_ASSERT(vertex_special_get(sn->v) == VERTEX_SPECIAL_MODULE);
          ctx->builder_args.val.n = containerof(sn->v, module, vertex)->build_dir;
        }

        exec_builder_add(ctx->builder, &ctx->builder_args);
      }

      ctx->builder_args = base_add_args;
      break;
    case EXEC_VALUE_PREPEND:
      ev = containerof(val, typeof(*ev), bacon);
      base_add_args = ctx->builder_args;

      ctx->builder_args.render_val = BUILDER_EXEC_VALUE;
      ctx->builder_args.val.f = ev->prepend;
      ctx->builder_args.mode = BUILDER_PREPEND;
      ctx->builder_args.render_sep = 0;

      ctx->builder_args.item = BUILDER_ARGS;
      for(x = 0; x < ctx->builder->args_len; x++)
      {
        ctx->builder_args.position = x;
        exec_builder_add(ctx->builder, &ctx->builder_args);
      }

      ctx->builder_args = base_add_args;
      break;
    case EXEC_VALUE_LOOKUP:
      ev = containerof(val, typeof(*ev), bacon);

      /* all lookups must have been resolved previously */
      v = lookup_store_get(ctx->lookups, ev);
      RUNTIME_ASSERT(v);
      fse = containerof(v, typeof(*fse), vertex);

      ctx->builder_args.val.fse = fse;
      ctx->builder_args.mode = BUILDER_REPLACE;
      ctx->builder_args.render_val = BUILDER_FSENT;
      exec_builder_add(ctx->builder, &ctx->builder_args);
      break;
    case EXEC_VALUE_SEQUENCE:
      ev = containerof(val, typeof(*ev), bacon);
      /* operations are always couched in a sequence operation */
      base_add_args = ctx->builder_args;
      exec_builder_reset(&ctx->operation_builder);
      ctx->builder = &ctx->operation_builder;

      ctx->builder_args.item = BUILDER_ARGS;
      llist_foreach(&ev->sequence, sv, lln) {
        ctx->builder_args.position = -1;
        exec_render_exec_value(ctx, sv);
        if(system_has_error) {
          return;
        }
      }

      /* the output of the sequence is a list of strings */
      exec_builder_build(ctx->builder, &sequence_output);

      // restore
      ctx->builder = ctx->base_builder;
      ctx->builder_args = base_add_args;
      ctx->builder_args.render_val = BUILDER_STRING;
      ctx->builder_args.mode = BUILDER_APPEND;

      for(x = 0; x < sequence_output->args_len; x++)
      {
        ctx->builder_args.val.s = sequence_output->args[x];
        exec_builder_add(ctx->builder, &ctx->builder_args);
      }

      /* if the list is empty, add the key to the env anyway so that it will at least be defined */
      if(sequence_output->args_len == 0 && base_add_args.item == BUILDER_ENVS)
      {
        ctx->builder_args.val.s = "";
        exec_builder_add(ctx->builder, &ctx->builder_args);
      }

      ctx->builder_args = base_add_args;
      break;
    case BACON_TYPE_STRING:
    case BACON_TYPE_FLOAT:
    case BACON_TYPE_BOOLEAN:
    case BACON_TYPE_INTEGER:
    case BACON_TYPE_MAPPING:
      ctx->builder_args.val.f = val;
      ctx->builder_args.mode = BUILDER_APPEND;
      ctx->builder_args.render_val = BUILDER_VALUE;
      exec_builder_add(ctx->builder, &ctx->builder_args);
      break;
    default:
      RTA;
  }
}
