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

#include "xapi.h"

#include "lorien/path_normalize.h"
#include "valyria/llist.h"
#include "valyria/set.h"
#include "xlinux/xstdlib.h"
#include "yyutil/box.h"

#include "extern.h"
#include "config.internal.h"
#include "fsent.h"
#include "walker.h"

static llist entry_list = LLIST_INITIALIZER(entry_list);         // active list
static llist entry_freelist = LLIST_INITIALIZER(entry_freelist); // freelist

typedef struct {
  fsent * n;
  llist lln;
} entry;

static xapi extern_list_append(fsent * n)
{
  enter;

  entry *e;

  if((e = llist_shift(&entry_freelist, typeof(*e), lln)) == 0)
  {
    fatal(xmalloc, &e, sizeof(*e));
    llist_init_node(&e->lln);
  }

  e->n = n;

  llist_append(&entry_list, e, lln);

  finally : coda;
}

xapi extern_reconfigure(configblob * restrict cfg, bool dry)
{
  enter;

  char space[512];
  fsent * base;
  box_string * elp;
  graph_invalidation_context invalidation = { 0 };
  int x;

  if(dry || !cfg->extern_section.changed) {
    goto XAPI_FINALLY;
  }

  fatal(graph_invalidation_begin, &invalidation);

  /* move the extern node list to the freelist */
  llist_splice_head(&entry_freelist, &entry_list);

  for(x = 0; x < cfg->extern_section.entries->table_size; x++)
  {
    if(!(elp = set_table_get(cfg->extern_section.entries, x)))
      continue;

    path_normalize(space, sizeof(space), elp->v);

    fatal(fsent_graft, space, &base, &invalidation);
    fatal(extern_list_append, base);
  }

finally:
  graph_invalidation_end(&invalidation);
coda;
}

xapi extern_system_reconcile(int walk_id, struct graph_invalidation_context * restrict invalidation)
{
  enter;

  entry *e;
  fsent *n;
  char path[512];

  llist_foreach(&entry_list, e, lln) {
    n = e->n;
    fsent_absolute_path_znload(path, sizeof(path), n);
    fatal(walker_descend, 0, n, 0, path, walk_id, invalidation);
    fatal(walker_ascend, n, walk_id, invalidation);
  }

  finally : coda;
}
