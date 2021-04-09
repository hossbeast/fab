/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

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

#ifndef FABD_EXTERN_H
#define FABD_EXTERN_H

/*

regions of the filesystem specified in config under the extern key are loaded along with the module
directory, and are used in resolution of import/use/require statements.

*/

#include "xapi.h"
#include "types.h"

struct configblob;
struct graph_invalidation_context;

/*
 * reapply configuration to the extern module
 *
 * ctx    - reconfiguration context
 * config - root of the config tree
 * dry    - whether to perform a dry-run
 */
xapi extern_reconfigure(struct configblob * restrict cfg, bool dry)
  __attribute__((nonnull));

/*
 * reload extern trees
 */
xapi extern_system_reconcile(int walk_id, struct graph_invalidation_context * restrict invalidation)
  __attribute__((nonnull));

#endif
