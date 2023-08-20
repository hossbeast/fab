/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "module.h"

struct graph_invalidation_context;
struct pattern;
struct rule_module_edge;

void module_block_variants(statement_block * restrict block, struct pattern * restrict references)
  __attribute__((nonnull));

void statement_block_xinit(statement_block * restrict block)
  __attribute__((nonnull));

void statement_block_xdestroy(statement_block * restrict block)
  __attribute__((nonnull));

/*
 * bootstrap the module at the specified node
 */
void module_bootstrap(
    struct fsent * restrict mod_dir_n
  , struct fsent * restrict mod_file_v
  , struct graph_invalidation_context * restrict rule_ctx
)
  __attribute__((nonnull));

void module_initialize(
    struct fsent * restrict mod_dir_n
  , struct fsent * restrict mod_file_v
  , struct graph_invalidation_context * restrict rule_ctx
)
  __attribute__((nonnull));
