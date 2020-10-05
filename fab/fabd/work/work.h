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

#ifndef _WORK_H
#define _WORK_H

#include "xapi.h"
#include "types.h"
#include "valyria/llist.h"

struct worker_thread_context;

typedef struct work {
  llist lln;
  xapi (*fn)(struct work *, struct worker_thread_context * restrict);
  int32_t completion_tid;
  uint32_t *completion_sum;
} work;

void work_run(
    xapi (*fn)(struct worker_thread_context * restrict ctx, void *arg0, void *arg)
  , void *arg0
  , void *args
  , size_t arg_size
)
  __attribute__((nonnull));

#endif
