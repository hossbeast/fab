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

struct module;
struct fsent;
struct narrator;
struct formula_value;
struct bacon_value;
struct build_slot;

typedef struct exec_render_context {
  /* input */
  const struct bacon_value *vars0;
  const struct bacon_value *vars;
  const struct build_slot *bs;
  const struct fsent *cwd;
  const struct module *mod;
  selector_context selector_context;

  /* operation state */
  exec_builder operation_builder;

  exec_builder * base_builder;
  exec_builder * builder;
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
  , const struct bacon_value * restrict vars    // used by exec_render_env_vars
  , const struct bacon_value * restrict vars0   // not used by exec_render_env_vars
  , const struct build_slot * restrict bs
)
  __attribute__((nonnull(1, 2, 3)));

/*
 * render a formula value to the file parameter of an exec builder
 */
void exec_render_file(exec_render_context * restrict ctx, const struct formula_value * restrict path)
  __attribute__((nonnull));

/*
 * render a formula value to the args parameter of an exec builder
 */
void exec_render_args(exec_render_context * restrict ctx, const struct formula_value * restrict args)
  __attribute__((nonnull));

/*
 * render a formula value to the envs parameter of an exec builder
 */
void exec_render_envs(exec_render_context * restrict ctx, const struct formula_value * restrict envs)
  __attribute__((nonnull));

/*
 * build an env list from the ctx->vars BACON_TYPE_SET
 */
void exec_render_env_vars(exec_render_context * restrict ctx)
  __attribute__((nonnull));

/*
 * write environment variable value text
 */
void exec_render_value(exec_render_context * restrict ctx, const struct bacon_value * restrict val)
  __attribute__((nonnull));

void exec_render_formula_value(exec_render_context * restrict ctx, const struct formula_value * restrict val)
  __attribute__((nonnull));
