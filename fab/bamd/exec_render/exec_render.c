/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "bacon/value.h"
#include "bacon/set.h"
#include "valyria/hashtable.h"
#include "valyria/pstring.h"
#include "common/attrs.h"
#include "narrator/fixed.h"
#include "bacon/writer.h"
#include "bacon/builder.h"

#include "exec_render.h"
#include "exec.h"
#include "formula_value.internal.h"
#include "build_slot.h"
#include "path_cache.h"
#include "module.h"
#include "sysvar.h"
#include "channel.h"
#include "system_state.h"
#include "events.h"
#include "dependency.h"
#include "buildplan_entity.h"
#include "vertex.h"

#include "zcat.h"

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
  , const bacon_value * restrict vars0
  , const bacon_value * restrict vars
  , const build_slot * restrict bs
)
{
  ctx->base_builder = ctx->builder = builder;
  ctx->vars0 = vars0;
  ctx->vars = vars;
  ctx->bs = bs;
  ctx->cwd = cwd;
  ctx->mod = mod;

  ctx->selector_context.bpe = 0;
  if(bs) {
    ctx->selector_context.bpe = bs->bpe;
  }

  ctx->selector_context.mod = mod;
  ctx->selector_context.cwd = cwd; // mod->dir_node;
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
  bacon_value * v;

  if(ctx->vars == 0) {
    return;
  }

  exec_render_context_reset(ctx);

  ctx->builder_args.item = BUILDER_ENVS;
  ctx->builder_args.position = -1;
  ctx->builder_args.render_sep = BUILDER_STRING;
  ctx->builder_args.sep.s = " ";
  ctx->builder_args.mode = BUILDER_APPEND;

  rbtree_foreach(&ctx->vars->set, v, rbn) {
    if(v->type != BACON_TYPE_MAPPING) {
      continue;
    }

    RUNTIME_ASSERT(v->key->type == BACON_TYPE_STRING);  // should be a coerce to string instead

    ctx->builder_args.name = v->key->s;
    ctx->builder_args.name_len = v->key->len;
    ctx->builder_args.mode = BUILDER_APPEND;

    exec_render_value(ctx, v->val);
  }
}

void exec_render_file(exec_render_context * restrict ctx, const formula_value * restrict file)
{
  exec_render_context_reset(ctx);

  ctx->builder_args.item = BUILDER_FILE;
  exec_render_formula_value(ctx, file);
}

void exec_render_args(exec_render_context * restrict ctx, const formula_value * restrict args)
{
  RUNTIME_ASSERT(args->type == FORMULA_VALUE_LIST);

  exec_render_context_reset(ctx);

  ctx->builder_args.item = BUILDER_ARGS;
  ctx->builder_args.position = -1;
  exec_render_formula_value(ctx, args);
}

void exec_render_envs(exec_render_context * restrict ctx, const formula_value * restrict envs)
{
  formula_value *mapping;

  RUNTIME_ASSERT(envs->type == FORMULA_VALUE_SET);

  exec_render_context_reset(ctx);

  ctx->builder_args.item = BUILDER_ENVS;
  ctx->builder_args.position = -1;
  ctx->builder_args.render_sep = BUILDER_STRING;
  ctx->builder_args.sep.s = " ";
  ctx->builder_args.mode = BUILDER_APPEND;

  rbtree_foreach(&envs->set, mapping, rbn) {
    RUNTIME_ASSERT(mapping->type == FORMULA_VALUE_MAPPING);

    ctx->builder_args.name = mapping->m.name;
    ctx->builder_args.name_len = mapping->m.name_len;

    exec_render_formula_value(ctx, mapping->m.value);
  }
}

void exec_render_value(exec_render_context * restrict ctx, const bacon_value * restrict val)
{
  bacon_value *item;

  if(val->type == BACON_TYPE_LIST)
  {
    llist_foreach(&val->list, item, lln) {
      exec_render_value(ctx, item);
    }
  }
  else if(val->type == BACON_TYPE_SET)
  {
    rbtree_foreach(&val->set, item, rbn) {
      exec_render_value(ctx, item);
    }
  }
  else if((val->type & BACON_TYPE_CLASS) == BACON_TYPE_SCALAR)
  {
    ctx->builder_args.val.v = val;
    ctx->builder_args.render_val = BUILDER_VALUE;
    ctx->builder_args.mode = BUILDER_APPEND;
    exec_builder_add(ctx->builder, &ctx->builder_args);
  }
}

void exec_render_formula_value(exec_render_context * restrict ctx, const formula_value * val)
{
  formula_value *sv;
  bacon_value *mapval;
  int x;
  exec * sequence_output;
  exec_builder_args base_add_args;
  selected *sn;
  const path_cache_entry *pe;
  bamipc_message *msg;
  channel *chan;
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;

  if(val->type == FORMULA_VALUE_LIST)
  {
    llist_foreach(&val->list, sv, lln) {
      exec_render_formula_value(ctx, sv);
      if(system_error) {
        return;
      }
    }
  }
  else if(val->type == FORMULA_VALUE_SET)
  {
    rbtree_foreach(&val->set, sv, rbn) {
      exec_render_formula_value(ctx, sv);
      if(system_error) {
        return;
      }
    }
  }
  else if(val->type == FORMULA_VALUE_VARIABLE)
  {
    if(ctx->vars0 && (mapval = bacon_set_lookupw(ctx->vars0, val->v.name, val->v.name_len))) {
      exec_render_value(ctx, mapval);
    } else if(ctx->vars && (mapval = bacon_set_lookupw(ctx->vars, val->v.name, val->v.name_len))) {
      exec_render_value(ctx, mapval);
    }
  }
  else if(val->type == FORMULA_VALUE_SELECT)
  {
    selector_exec(val->op.selector, &ctx->selector_context, SELECTION_ITERATION_TYPE_ORDER);
  }
  else if(val->type == FORMULA_VALUE_PROPERTY)
  {
    base_add_args = ctx->builder_args;

    ctx->builder_args.mode = BUILDER_APPEND;
    ctx->builder_args.render_val = BUILDER_PROPERTY;
    ctx->builder_args.val.prop = val->op.property;
    ctx->builder_args.val.pctx.cwd = ctx->cwd;
    llist_foreach(&ctx->selector_context.selection->list, sn, lln) {
      if(vertex_kind_get(sn->v) == VERTEX_KIND_FSENT) {
        ctx->builder_args.val.n = containerof(sn->v, fsent, vertex);
      } else {
        RUNTIME_ASSERT(vertex_special_get(sn->v) == VERTEX_SPECIAL_MODULE);
        ctx->builder_args.val.n = containerof(sn->v, module, vertex)->dir_node;
      }

      exec_builder_add(ctx->builder, &ctx->builder_args);
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
      exec_builder_add(ctx->builder, &ctx->builder_args);
    }

    ctx->builder_args = base_add_args;
  }
  else if(val->type == FORMULA_VALUE_PATH_SEARCH)
  {
    /* assumes string */
    path_cache_search(&pe, MMS(val->op.operand->s));
    if(pe->fd == -1)
    {
      msg = system_error_event(&chan);
      msg->code = ENOENT;
      narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
      bacon_writer_init(&writer);
      builder_writer = bacon_writer_open(&writer, &fixed.base);

      { MAPPING
        STRING("cannot execute formula")
        { SET
          { MAPPING
            STRING("path-search")
            STRING(val->op.operand->s)
          }
          { MAPPING
            STRING("dependency")
            { SET
              vertex_describe(&ctx->bs->bpe->vertex, builder_writer);
            }
          }
        }
      }
      bacon_writer_close(builder_writer);
      msg->size = fixed.l;
      events_publish(chan, msg);
      return;
    }

    ctx->builder_args.val.pe = pe;
    ctx->builder_args.mode = BUILDER_APPEND;
    ctx->builder_args.render_val = BUILDER_PATH_CACHE_ENTRY;
    exec_builder_add(ctx->builder, &ctx->builder_args);
  }
  else if(val->type == FORMULA_VALUE_SEQUENCE)
  {
    /* operations are always couched in a sequence operation */
    base_add_args = ctx->builder_args;
    exec_builder_reset(&ctx->operation_builder);
    ctx->builder = &ctx->operation_builder;

    ctx->builder_args.item = BUILDER_ARGS;
    llist_foreach(&val->op.list, sv, lln) {
      ctx->builder_args.position = -1;
      exec_render_formula_value(ctx, sv);
      if(system_error) {
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

    for(x = 0; x < sequence_output->args_size; x++)
    {
      ctx->builder_args.val.s = sequence_output->args[x];
      exec_builder_add(ctx->builder, &ctx->builder_args);
    }

    /* if the list is empty, add the key to the env anyway so that it will at least be defined */
    if(sequence_output->args_size == 0 && base_add_args.item == BUILDER_ENVS)
    {
      ctx->builder_args.val.s = "";
      exec_builder_add(ctx->builder, &ctx->builder_args);
    }

    ctx->builder_args = base_add_args;
  }
  else if((val->type & BACON_TYPE_CLASS) == BACON_TYPE_SCALAR)
  {
    ctx->builder_args.val.f = val;
    ctx->builder_args.mode = BUILDER_APPEND;
    ctx->builder_args.render_val = BUILDER_FORMULA_VALUE;
    exec_builder_add(ctx->builder, &ctx->builder_args);
  }
  else
  {
    RTA;
  }
}
