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
#include "types.h"

#include "xlinux/xstdlib.h"
#include "narrator.h"

#include "ff_node_artifact.internal.h"
#include "ff_node.internal.h"
#include "ff_node_artifact_pattern.internal.h"
#include "ff_node_string.internal.h"

#include "macros.h"

xapi ffn_artifact_say_tree(const ff_node_artifact * restrict n, int level, narrator * restrict N)
{
  enter;

  xsayf("%*spatterns\n", level * 2, "");
  fatal(ffn_say_tree_level, (ff_node*)n->pattern_list, level + 1, N);

  finally : coda;
}

xapi ffn_artifact_say_normal(const ff_node_artifact * restrict n, narrator * restrict N)
{
  enter;

  xsays("artifact ");
  fatal(ffn_say_normal_list, (ff_node*)n->pattern_list, N, 0);

  finally : coda;
}

xapi ffn_artifact_mknode(ff_node_artifact ** restrict n, va_list va)
{
  enter;

  fatal(xmalloc, n, sizeof(**n));

  (*n)->pattern_list = va_arg(va, typeof((*n)->pattern_list));

  finally : coda;
}

void ffn_artifact_destroy(ff_node_artifact * restrict n)
{
  ffn_free((ff_node*)n->pattern_list);
}
