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

#include "xapi.h"

#include "valyria/set.h"
#include "narrator.h"

#include "buildplan.h"
#include "dependency.h"
#include "logging.h"
#include "fsent.h"

selection buildplan_selection;
uint16_t buildplan_id;

xapi buildplan_setup()
{
  enter;

  fatal(selection_xinit, &buildplan_selection);
  buildplan_id = 1;

  finally : coda;
}

xapi buildplan_cleanup()
{
  enter;

  fatal(selection_xdestroy, &buildplan_selection);

  finally : coda;
}

xapi buildplan_reset()
{
  enter;

  fatal(selection_reset, &buildplan_selection, SELECTION_ITERATION_TYPE_RANK);
  buildplan_id++;

  finally : coda;
}

xapi buildplan_add(dependency * restrict bpe, int distance)
{
  enter;

  fatal(selection_add_dependency, &buildplan_selection, bpe, distance);

  finally : coda;
}

xapi buildplan_finalize()
{
  enter;

  selection_finalize(&buildplan_selection);

  finally : coda;
}

xapi buildplan_report()
{
  enter;

  selected *sn;
  const dependency *bpe;
  narrator *N;

  logf(L_BUILDPLAN, "%3zu executions in %2hu stages", buildplan_selection.selected_entities->size, buildplan_selection.numranks);

  llist_foreach(&buildplan_selection.list, sn, lln) {
    bpe = sn->bpe;
    fatal(log_start, L_BUILDPLAN, &N);
    fatal(narrator_xsayf, N, " %-3d ", sn->rank);
    if(bpe->fml)
    {
      fatal(narrator_xsays, N, bpe->fml->name.name);
      fatal(narrator_xsays, N, " -> ");
    }
    fatal(dependency_say_targets, bpe, N);
    fatal(log_finish);
  }

  finally : coda;
}
