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

#include <sys/syscall.h>
#include <unistd.h>

#include "xapi.h"
#include "types.h"

#include "valyria/llist.h"

#include "work.h"
#include "params.h"
#include "worker_thread.h"

#include "locks.h"

void work_run(
    xapi (*fn)(worker_thread_context * restrict ctx, void *arg0, void *arg)
  , void *arg0
  , void *args
  , size_t arg_size
)
{
  uint32_t completion_sum;
  uint32_t completion_target;
  int x;

  completion_sum = 0;
  x = 0;
  stack_foreach(&g_workers, worker, stk) {
    if(llist_empty(&works[x])) {
      break;
    }

    /* arrange for notification after the last work item */
    last = llist_prev(&works[x], work, lln);
    last->completion_tid = g_tid;
    last->completion_sum = &completion_sum;

    x++;
  }

  completion_target = sum_first(x + 1);

  // join
  while(completion_sum != completion_target && !g_params.shutdown)
  {
    fatal(uxsigwaitinfo, 0, sigs, info);
  }

  return true;
}
