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
#include "xlinux/xstdlib.h"
#include "valyria/set.h"

#include "buildplan.h"
#include "node.h"

#include "assure.h"
#include "macros.h"

xapi buildplan_create(buildplan ** restrict bp)
{
  enter;

  fatal(xmalloc, bp, sizeof(**bp));
  fatal(set_create, &(*bp)->nodes);

  finally : coda;
}

xapi buildplan_xfree(buildplan * restrict bp)
{
  enter;

  if(bp)
  {
    fatal(set_xfree, bp->nodes);

    int x;
    for(x = 0; x < bp->stages_a; x++)
      wfree(bp->stages[x]);
    wfree(bp->stages);
  }
  wfree(bp);

  finally : coda;
}

xapi buildplan_ixfree(buildplan ** restrict bp)
{
  enter;

  fatal(buildplan_xfree, *bp);
  *bp = 0;

  finally : coda;
}

xapi buildplan_reset(buildplan * restrict bp, int traversal)
{
  enter;

  fatal(set_recycle, bp->nodes);
  bp->traversal = traversal;

  memset(bp->stages_lens, 0, bp->stages_a * sizeof(*bp->stages_lens));
  bp->stages_len = 0;

  finally : coda;
}

xapi buildplan_add(buildplan * restrict bp, node * restrict n, int distance)
{
  enter;

  vertex * v = vertex_containerof(n);

  if(v->visited < bp->traversal)
    n->build_depth = 0;
  n->build_depth = MAX(n->build_depth, distance);

  fatal(set_put, bp->nodes, MM(n));

  finally : coda;
}

xapi buildplan_finalize(buildplan * restrict bp)
{
  enter;

  int max_stage = 0;

  int x;
  for(x = 0; x < bp->nodes->table_size; x++)
  {
    node ** np;
    if(!set_table_element(bp->nodes, x, &np, 0))
      continue;

    node * n = *np;

    int stage = n->build_depth;
    size_t stages_lens_a = bp->stages_a;
    size_t stages_as_a = bp->stages_a;
    fatal(assure, &bp->stages, sizeof(*bp->stages), stage + 1, &bp->stages_a);
    fatal(assure, &bp->stages_lens, sizeof(*bp->stages_lens), stage + 1, &stages_lens_a);
    fatal(assure, &bp->stages_as, sizeof(*bp->stages_as), stage + 1, &stages_as_a);
    int stage_index = bp->stages_lens[stage];

    fatal(assure, &bp->stages[stage], sizeof(**bp->stages), stage_index + 1, &bp->stages_as[stage]);
    bp->stages[stage][stage_index] = n;
    bp->stages_lens[stage]++;

    max_stage = MAX(max_stage, stage);
  }

  bp->stages_len = max_stage + 1;

  finally : coda;
}

xapi buildplan_report(buildplan * restrict bp)
{
  enter;

  char path[512];

  int x, y;
  for(x = bp->stages_len - 1; x >= 0; x--)
  {
    for(y = 0; y < bp->stages_lens[x]; y++)
    {
      node_get_absolute_path(bp->stages[x][y], path, sizeof(path));
      printf("[%2d][%2d] %s\n", x, y, path);
    }
  }

  finally : coda;
}
