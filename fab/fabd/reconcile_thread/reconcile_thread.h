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

#ifndef _RECONCILE_THREAD_H
#define _RECONCILE_THREAD_H

#include "xapi.h"
#include "types.h"

#include "locks.h"
#include "fab/metadata.h"

struct handler_context;

extern struct trylock reconcile_lock;
extern enum bam_system_state global_system_state;
extern uint16_t reconciliation_id;

/*
 * launch the reconcile thread, run global reconciliation
 */
xapi reconcile_thread_launch(struct handler_context *ctx)
  __attribute__((nonnull));

#endif