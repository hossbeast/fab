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

#ifndef FABD_GOALS_H
#define FABD_GOALS_H

/*

The goals describe the set of nodes which are to be brought up to date by building.

*/

#include "types.h"
#include "xapi.h"

struct handler_context;
struct narrator;
struct selector;

extern bool goals_autorun;

xapi goals_set(
    uint64_t msg_id
  , bool autorun
  , bool build
  , bool script
  , struct selector * restrict target_direct
  , struct selector * restrict target_transitive
);

xapi goals_kickoff(struct handler_context * restrict handler)
  __attribute__((nonnull));

xapi goals_setup(void);
xapi goals_cleanup(void);
xapi goals_say(struct narrator * restrict N)
  __attribute__((nonnull));

#endif
