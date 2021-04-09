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

#ifndef FABD_BUILDPLAN_H
#define FABD_BUILDPLAN_H

/*

The buildplan represents a set of formulas to be executed in order to satisfy the current goals.

*/

#include "xapi.h"
#include "selection.h"

#include "moria/traverse.h"

struct dependency;
struct moria_vertex;
struct llist;

enum buildplan_state {
    NOOP          // nothing to do
  , UNSATISFIED   // cant build, unsatisfied dependencies
  , READY         // DEW IT
};

extern selection buildplan_selection;
extern uint16_t buildplan_id;
extern enum buildplan_state buildplan_state;

xapi buildplan_create(void);
xapi buildplan_setup(void);
xapi buildplan_cleanup(void);
xapi buildplan_reset(void);
xapi buildplan_report(void);

/* add a dependency edge which has a formula to the current buildplan */
xapi buildplan_add(struct dependency * restrict bpe, int distance)
  __attribute__((nonnull));

xapi buildplan_finalize(void)
  __attribute__((nonnull));

/*
 * Add a node to the plan
 *
 * @distance - length of the path to this node from the target
 */
xapi buildplan_visitor_direct(struct moria_vertex * v, void * arg, moria_traversal_mode mode, int distance, int * restrict result)
  __attribute__((nonnull(1)));

/*
 * add each node in the selection to the plan, along with its dependencies, recursively
 */
xapi buildplan_select_transitive(struct llist * restrict selection)
  __attribute__((nonnull));

/*
 * add exactly those nodes in the selection to the plan
 */
xapi buildplan_select_direct(struct llist * restrict selection)
  __attribute__((nonnull));

#endif
