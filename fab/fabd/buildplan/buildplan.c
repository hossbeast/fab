/* Copyright (c) 2012-2018 Todd Freed <todd.freed@gmail.com>

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

#include "moria/vertex.h"
#include "fab/ipc.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xsignal.h"
#include "xlinux/xfcntl.h"
#include "xlinux/xunistd.h"
#include "valyria/set.h"
#include "moria/edge.h"
#include "narrator.h"
#include "value.h"
#include "fab/sigutil.h"

#include "buildplan.h"
#include "buildplan_entity.h"
#include "server_thread.internal.h"
#include "config.internal.h"
#include "formula.h"
#include "logging.h"
#include "node.h"
#include "params.h"
#include "path.h"
#include "selection.h"

#include "common/assure.h"
#include "macros.h"

selection bp_selection;
uint16_t bp_plan_id;

xapi buildplan_setup()
{
  enter;

  fatal(selection_xinit, &bp_selection);
  bp_plan_id = 1;

  finally : coda;
}

xapi buildplan_cleanup()
{
  enter;

  fatal(selection_xdestroy, &bp_selection);

  finally : coda;
}

xapi buildplan_reset()
{
  enter;

  fatal(selection_reset, &bp_selection);
  bp_plan_id++;

  finally : coda;
}

xapi buildplan_add(buildplan_entity * restrict bpe, int distance)
{
  enter;

  fatal(selection_add_bpe, &bp_selection, bpe, distance);

  finally : coda;
}

xapi buildplan_finalize()
{
  enter;

  selection_finalize(&bp_selection);

  finally : coda;
}

xapi buildplan_report()
{
  enter;

  selected_node *sn;
  buildplan_entity *bpe;
  narrator *N;

  logf(L_BUILDPLAN, "%3zu executions in %2hu stages", bp_selection.selected_nodes->size, bp_selection.numranks);

  llist_foreach(&bp_selection.list, sn, lln) {
    bpe = sn->bpe;
    fatal(log_start, L_BUILDPLAN, &N);
    fatal(narrator_xsayf, N, " %-3d ", sn->rank);
    if(bpe->fml)
    {
      fatal(narrator_xsays, N, bpe->fml->name->name);
      fatal(narrator_xsays, N, " -> ");
    }
    fatal(bpe_say_targets, bpe, N);
    fatal(log_finish);
  }

  finally : coda;
}
