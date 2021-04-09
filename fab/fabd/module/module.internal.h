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

#ifndef FABD_MODULE_INTERNAL_H
#define FABD_MODULE_INTERNAL_H

#include "types.h"

#include "module.h"

struct graph_invalidation_context;
struct pattern;
struct rule_module_edge;

xapi module_block_variants(statement_block * restrict block, const struct pattern * restrict references)
  __attribute__((nonnull));

xapi statement_block_xinit(statement_block * restrict block)
  __attribute__((nonnull));

xapi statement_block_xdestroy(statement_block * restrict block)
  __attribute__((nonnull));

/*
 * bootstrap the module at the specified node
 */
xapi module_bootstrap(
    struct fsent * restrict mod_dir_n
  , struct fsent * restrict mod_file_v
  , struct graph_invalidation_context * restrict rule_ctx
)
  __attribute__((nonnull));

xapi module_initialize(
    struct fsent * restrict mod_dir_n
  , struct fsent * restrict mod_file_v
  , struct graph_invalidation_context * restrict rule_ctx
)
  __attribute__((nonnull));

#endif
