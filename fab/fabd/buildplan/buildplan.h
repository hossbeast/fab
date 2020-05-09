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

#include "xapi.h"

#include "selection.h"

struct buildplan_entity;

extern selection bp_selection;
extern uint16_t bp_plan_id;

xapi buildplan_setup(void);

xapi buildplan_cleanup(void);

xapi buildplan_reset(void);

xapi buildplan_add(struct buildplan_entity * restrict bpe, int distance)
  __attribute__((nonnull));

xapi buildplan_finalize(void)
  __attribute__((nonnull));

xapi buildplan_report(void);

#endif
