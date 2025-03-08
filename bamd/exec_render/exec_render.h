/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

exec_render - write strings to an exec_builder

*/

#include "types.h"

#include "exec_builder.h"
#include "selector.h"
#include "fsent.h"
#include "namespace.h"
#include "resolve.h"

struct module;
struct fsent;
struct narrator;
struct exec_value;
struct bacon_value;
struct build_slot;
struct lookup_store;

typedef struct exec_render_context {
  /* input */
  const struct bacon_value *modvars;      // mod->vars
  const struct bacon_value *sysvars;      // sysvars
  const struct build_slot *bs;
  const struct fsent *cwd;
  const struct module *mod;
  struct lookup_store *lookups;

  /* scratch */
  selector_context selector_context;
  struct lookup_context lctx;

  /* operation state */
  exec_builder operation_builder;

  exec_builder *base_builder;
  exec_builder *builder;
  exec_builder_args builder_args;
} exec_render_context;

void exec_render_context_init(exec_render_context * restrict ctx)
  __attribute__((nonnull));

void exec_render_context_reset(exec_render_context * restrict ctx)
  __attribute__((nonnull));

void exec_render_context_destroy(exec_render_context * restrict ctx)
  __attribute__((nonnull));

void exec_render_context_configure(
    exec_render_context * restrict ctx
  , exec_builder * restrict builder
  , const struct fsent * restrict cwd
  , const struct module * restrict mod
  , const struct bacon_value * restrict modvars     // mod->vars ; used   by exec_render_env_vars
  , const struct bacon_value * restrict sysvars     // sysvars ; not used by exec_render_env_vars
  , const struct build_slot * restrict bs
)
  __attribute__((nonnull(1, 2, 3)));

/*
 * render a formula value to the file parameter of an exec builder
 */
void exec_render_file(exec_render_context * restrict ctx, const struct bacon_value * restrict path)
  __attribute__((nonnull));

/*
 * render a formula value to the args parameter of an exec builder
 */
void exec_render_args(exec_render_context * restrict ctx, const struct bacon_value * restrict args)
  __attribute__((nonnull));

/*
 * render a formula value to the envs parameter of an exec builder
 */
void exec_render_envs(exec_render_context * restrict ctx, const struct bacon_value * restrict envs)
  __attribute__((nonnull));

/*
 * build an env list from the ctx->sysvars BACON_TYPE_SET
 */
void exec_render_env_vars(exec_render_context * restrict ctx)
  __attribute__((nonnull));

/*
 * write environment variable value text
 */
void exec_render_exec_value(exec_render_context * restrict ctx, const struct bacon_value * restrict val)
  __attribute__((nonnull));

//void exec_render_value(exec_render_context * restrict ctx, const struct bacon_value * restrict val)
//  __attribute__((nonnull));
//
//void exec_render_value(exec_render_context * restrict ctx, const bacon_value * restrict val)
//{
//  bacon_value *item;
//
//  if(val->type == BACON_TYPE_LIST)
//  {
//    llist_foreach(&val->list, item, lln) {
//      exec_render_value(ctx, item);
//    }
//  }
//  else if(val->type == BACON_TYPE_SET)
//  {
//    rbtree_foreach(&val->set, item, rbn) {
//      exec_render_value(ctx, item);
//    }
//  }
//  else if((val->type & BACON_TYPE_CLASS) == BACON_TYPE_SCALAR)
//  {
//    ctx->builder_args.val.v = val;
//    ctx->builder_args.render_val = BUILDER_VALUE;
//    ctx->builder_args.mode = BUILDER_APPEND;
//    exec_builder_add(ctx->builder, &ctx->builder_args);
//  }
//}
