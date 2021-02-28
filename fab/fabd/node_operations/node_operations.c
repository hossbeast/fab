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

#include <stdlib.h>

#include "types.h"
#include "xapi.h"

#include "xlinux/xfcntl.h"
#include "xlinux/xmman.h"
#include "xlinux/xstat.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "xlinux/xunistd.h"
#include "moria/graph.h"
#include "moria/operations.h"
#include "moria/vertex.h"
#include "moria/edge.h"
#include "moria/operations.h"
#include "valyria/list.h"
#include "valyria/llist.h"
#include "valyria/set.h"
#include "valyria/map.h"
#include "fab/events.h"
#include "value.h"
#include "narrator.h"
#include "dependency.h"

#include "node_operations.h"
#include "logging.h"
#include "fsent.h"
#include "shadow.h"
#include "filesystem.h"
#include "graph.h"
#include "module.h"
#include "rule.h"
#include "params.h"
#include "stats.h"
#include "formula.h"
#include "events.h"
#include "handler.h"
#include "var.h"
#include "rule_system.h"
#include "rule_module.h"

#include "macros.h"
#include "common/attrs.h"
#include "common/hash.h"

//
// static
//

//
// public
//

xapi node_operations_create_vertex(
    moria_vertex ** const restrict rv
  , moria_graph * const restrict g
  , vertex_kind kind
  , uint32_t attrs
  , const char * const restrict label
  , uint16_t label_len
)
{
  enter;

  fsent * n;

  RUNTIME_ASSERT(label_len);
  RUNTIME_ASSERT((attrs & VERTEX_KIND_OPT) == 0);

  fatal(fsent_create, &n, kind, attrs, label, label_len);

  *rv = &n->vertex;

  finally : coda;
}
