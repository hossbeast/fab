/* Copyright (c) 2011-2015 Todd Freed <todd.freed@gmail.com>

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

#ifndef FABD_RULE_SYSTEM_H
#define FABD_RULE_SYSTEM_H

#include "types.h"
#include "xapi.h"

struct rule_module_edge;
struct rule_run_context;
struct channel;

/* auto-inc reconciliation round */
extern uint32_t rule_system_reconciliation_id;

xapi rule_system_setup(void);

/*
 * run invalidated rules to quiescence
 *
 * reconciled - (returns) false if any rules could not be run
 */
xapi rule_system_reconcile(struct rule_run_context * restrict ctx, struct channel * restrict chan)
  __attribute__((nonnull));

/*
 * enqueue this rma to be executed during the next reconciliation
 */
void rule_system_rma_enqueue(struct rule_module_edge * restrict rma)
  __attribute__((nonnull));

/*
 * track that a rule was run and had matches
 */
void rule_system_rma_hit(struct rule_run_context * restrict ctx, struct rule_module_edge * restrict rma)
  __attribute__((nonnull));

/*
 * track that a rule was run and had no matches
 */
void rule_system_rma_nohit(struct rule_run_context * restrict ctx, struct rule_module_edge * restrict rma)
  __attribute__((nonnull));

#endif
