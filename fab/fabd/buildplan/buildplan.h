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
#include "types.h"

struct set;   // libvalyria/set
struct node;

typedef struct buildplan {
  struct set * nodes;
  int traversal;

  struct node *** stages;
  size_t * stages_as;
  size_t * stages_lens;
  size_t stages_a;
  size_t stages_len;
} buildplan;

/// buildplan_create
//
// SUMMARY
//  create a buildplan
//
xapi buildplan_create(buildplan ** restrict bp)
  __attribute__((nonnull));

xapi buildplan_xfree(buildplan * restrict bp);

xapi buildplan_reset(buildplan * restrict bp, int traversal)
  __attribute__((nonnull));

xapi buildplan_ixfree(buildplan ** restrict bp)
  __attribute__((nonnull));

xapi buildplan_add(buildplan * restrict bp, struct node * restrict n, int distance)
  __attribute__((nonnull));

xapi buildplan_finalize(buildplan * restrict bp)
  __attribute__((nonnull));

xapi buildplan_report(buildplan * restrict bp)
  __attribute__((nonnull));

#endif
