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
#include "channel.h"
#include "zbuffer.h"
#include "common/attrs.h"
#include "events.h"
#include "system_state.h"

selection buildplan_selection;
uint16_t buildplan_id;
enum buildplan_state buildplan_state;

static xapi plan_visitor_transitive(moria_vertex * v, void * arg, moria_traversal_mode mode, int distance, int * restrict result)
{
  enter;

  fatal(moria_traverse_vertices
    , &g_graph
    , v
    , buildplan_visitor_direct
    , 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_DEPENDS
        , edge_visit : EDGE_DEPENDS
        , vertex_visit : VERTEX_FILETYPE_REG
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE
    , 0
  );

  finally : coda;
}

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
  buildplan_state = NOOP;

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

/*
 * Add a node to the plan
 *
 * @distance - length of the path to this node from the target
 */
xapi buildplan_visitor_direct(moria_vertex * v, void * arg, moria_traversal_mode mode, int distance, int * restrict result)
{
  enter;

  fsent * n;
  channel * chan;
  char path[512];
  fabipc_message *msg;

  n = containerof(v, fsent, vertex);

  if(!fsent_invalid_get(n)) {
    goto XAPI_FINALIZE;
  }

  /* shadow nodes can act as placeholders, but are never actually built */
  if(!n->dep) { // && fsent_shadowtype_get(n)) {
    goto XAPI_FINALIZE;
  }

  /* this is an error case - node is not up to date, and theres no way to update it */
  else if(!n->dep)
  {
    if(n->bp_plan_id != buildplan_id)
    {
      buildplan_state = UNSATISFIED;
      fsent_path_znload(path, sizeof(path), n);

      system_error = true;
      if(events_would(FABIPC_EVENT_SYSTEM_STATE, &chan, &msg)) {
        msg->code = EINVAL;
        msg->size = znloadf(msg->text, sizeof(msg->text), "cannot update %s state %s"
          , path
          , attrs32_name_byvalue(graph_vertex_state_attrs, fsent_state_get(n))
        );
        events_publish(chan, msg);
      }
    }
    n->bp_plan_id = buildplan_id;
    goto XAPI_FINALIZE;
  }

  fatal(buildplan_add, n->dep, distance);

  finally : coda;
}

/*
 * add each node in the selection to the plan, along with its dependencies, recursively
 */
xapi buildplan_select_transitive(llist * restrict selection)
{
  enter;

  selected *sn;

  llist_foreach(selection, sn, lln) {
    fatal(plan_visitor_transitive, (void*)sn->v, 0, 0, 0, 0);
  }

  finally : coda;
}

/*
 * add exactly those nodes in the selection to the plan
 */
xapi buildplan_select_direct(llist * restrict selection)
{
  enter;

  selected *sn;

  llist_foreach(selection, sn, lln) {
    fatal(buildplan_visitor_direct, (void*)sn->v, 0, 0, 0, 0);
  }

  finally : coda;
}
