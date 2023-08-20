/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

struct exec_builder;
struct exec_render_context;
struct build_slot;

void sysvar_builder_variant(struct exec_builder * restrict builder, const struct build_slot * restrict bs)
  __attribute__((nonnull));

void sysvar_builder_targets(struct exec_builder * restrict builder, const struct build_slot * restrict bs)
  __attribute__((nonnull));

void sysvar_builder_sources(struct exec_builder * restrict builder, const struct build_slot * restrict bs)
  __attribute__((nonnull));

void exec_render_sysvar_sources(struct exec_render_context * restrict ctx, const struct build_slot * restrict bs)
  __attribute__((nonnull));

void exec_render_sysvar_targets(struct exec_render_context * restrict ctx, const struct build_slot * restrict bs)
  __attribute__((nonnull));

void exec_render_sysvar_variant(struct exec_render_context * restrict ctx, const struct build_slot * restrict bs)
  __attribute__((nonnull));
