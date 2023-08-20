/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "lorien/path_normalize.h"
#include "valyria/llist.h"
#include "valyria/set.h"
#include "xlinux/xstdlib.h"
#include "yyutil/box.h"

#include "extern.h"
#include "config.internal.h"
#include "fsent.h"
#include "walker.h"

void extern_system_reconcile(int walk_id, struct graph_invalidation_context * restrict invalidation)
{
}
