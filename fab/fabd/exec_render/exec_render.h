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

#ifndef EXEC_RENDER_H
#define EXEC_RENDER_H

/*

exec_render - write strings to an exec_builder

*/

#include "types.h"

#include "exec_builder.h"
#include "selector.h"
#include "fsent.h"

struct fsent;
struct narrator;
struct formula_value;
struct value;
struct build_slot;

typedef struct exec_render_context {
  /* input */
  const struct value * vars;
  const struct build_slot * bs;
  selector_context selector_context;

  /* operation state */
  exec_builder operation_builder;

  exec_builder * base_builder;
  exec_builder * builder;
  exec_builder_args builder_args;
} exec_render_context;

void exec_render_context_xinit(exec_render_context * restrict ctx)
  __attribute__((nonnull));

void exec_render_context_xreset(exec_render_context * restrict ctx)
  __attribute__((nonnull));

void exec_render_context_xdestroy(exec_render_context * restrict ctx)
  __attribute__((nonnull));

void exec_render_context_configure(
    exec_render_context * restrict ctx
  , exec_builder * restrict builder
  , struct module * restrict mod
  , const struct value * restrict vars
  , const struct build_slot * restrict bs
)
  __attribute__((nonnull(1, 2)));

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
 *
 */
void exec_render_env_sysvars(exec_render_context * restrict ctx, const struct build_slot * restrict bs)
  __attribute__((nonnull));

/*
 * build an env list from a VALUE_TYPE_SET
 */
void exec_render_env_vars(exec_render_context * restrict ctx)
  __attribute__((nonnull));

/*
 * write environment variable value text
 */
void exec_render_value(exec_render_context * restrict ctx, const struct value * restrict val)
  __attribute__((nonnull));

void exec_render_formula_value(exec_render_context * restrict ctx, const struct formula_value * restrict val)
  __attribute__((nonnull));

#endif
