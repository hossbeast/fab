/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

regions of the filesystem specified in config under the extern key are loaded along with the module
directory, and are used in resolution of import/use/require statements.

*/

#include "types.h"

struct configblob;
struct graph_invalidation_context;

/*
 * reload extern trees
 */
void extern_system_reconcile(int walk_id, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));
