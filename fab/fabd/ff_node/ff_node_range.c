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

#include "ff_node_range.internal.h"
#include "ff_node.internal.h"
#include "ff_node_pattern.internal.h"
#include "ff_node_char.internal.h"
#include "node.h"
#include "match.internal.h"
#include "path.h"

//
// public
//

xapi ffn_range_say_tree(const ff_node_range * restrict n, int level, narrator * restrict N)
{
  enter;

  xsayf("%*sstart\n", level * 2, "");
  fatal(ffn_say_tree_level, (ff_node*)n->start, level + 1, N);
  xsayf("%*send\n", level * 2, "");
  fatal(ffn_say_tree_level, (ff_node*)n->end, level + 1, N);

  finally : coda;
}

xapi ffn_range_say_normal(const ff_node_range * restrict n, narrator * restrict N)
{
  enter;

  fatal(ffn_say_normal, (ff_node*)n->start, N);
  xsayc('-');
  fatal(ffn_say_normal, (ff_node*)n->end, N);

  finally : coda;
}

xapi ffn_range_mknode(ff_node_range ** restrict n, va_list va)
{
  enter;

  fatal(xmalloc, n, sizeof(**n));

  (*n)->start = va_arg(va, ff_node_char*);
  (*n)->end = va_arg(va, ff_node_char*);

  finally : coda;
}

void ffn_range_destroy(ff_node_range * restrict n)
{
  ffn_free((ff_node*)n->start);
  ffn_free((ff_node*)n->end);
}

xapi ffn_range_match(pattern_match_context * restrict context, const ff_node_range * restrict range)
{
  enter;

  const char * name = context->node->name->name;
  int namel = context->node->name->namel;

  if(namel > context->name_offset)
  {
    if(name[context->name_offset] >= range->start->code && name[context->name_offset] <= range->end->code)
    {
      context->name_offset += 1;
      goto XAPI_FINALIZE;
    }
  }

  context->node = 0;

  finally : coda;
}
