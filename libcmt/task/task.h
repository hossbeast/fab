/* Copyright (c) 2012-2020 Todd Freed <todd.freed@gmail.com>

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

#ifndef _CMT_TASK_H
#define _CMT_TASK_H

/*
 * enqueue the active task onto the runqueue, switch to the top task on the runqueue
 *
 * typically only called from the base task in a domain
 */
void task_yield(domain *restrict)
  __attribute__((nonnull));

/*
 * mark the active task as non-runnable and yield - the task becomes runnable again after
 * all tasks cloned from it have finalized
 *
 * may not be called from the base task in a domain
 */
void task_join(domain *restrict)
  __attribute__((nonnull));

#endif
