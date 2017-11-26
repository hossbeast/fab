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
#include "narrator/fixed.h"

#include "ff_node_string.internal.h"
#include "ff_node.internal.h"
#include "ff_node_char.internal.h"
#include "ff_node_pattern.internal.h"
#include "ff_parser.h"

#include "zbuffer.h"

//
// static
//

static bool string_contains_nonprintable(const ff_node_string_part * restrict n)
{
  while(n)
  {
    if(n->type == FFN_CHAR && ffn_char_nonprintable(&n->character))
      return true;

    n = (ff_node_string_part*)n->next;
  }

  return false;
}

//
// protected
//

xapi ffn_string_say_tree(const ff_node_string * restrict n, int level, narrator * restrict N)
{
  enter;

  xsayf("%*slist\n", level * 2, "");
  fatal(ffn_say_tree_level, n->chain, level + 1, N);

  finally : coda;
}

xapi ffn_string_say_normal(const ff_node_string * restrict n, narrator * restrict N)
{
  enter;

  bool refs = string_contains_nonprintable(n->chain);
  if(refs)
    xsayc('"');
  fatal(ffn_say_normal_list, n->chain, N, 0);
  if(refs)
    xsayc('"');

  finally : coda;
}

xapi ffn_string_mknode(ff_node_string ** restrict n, va_list va)
{
  enter;

  fatal(xmalloc, n, sizeof(**n));

  (*n)->chain = va_arg(va, typeof((*n)->chain));

  finally : coda;
}

void ffn_string_destroy(ff_node_string * restrict n)
{
  ffn_free(n->chain);
}
