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

#include "ff_node_strings.internal.h"
#include "ff_node.internal.h"
#include "ff_node_string.internal.h"

#include "macros.h"

xapi ffn_strings_say_tree(const ff_node_strings * restrict n, int level, narrator * restrict N)
{
  enter;

  xsayf("%*schain\n", level * 2, "");
  fatal(ffn_say_tree_level, (ff_node*)n->chain, level + 1, N);

  finally : coda;
}

xapi ffn_strings_say_normal(const ff_node_strings * restrict n, narrator * restrict N)
{
  enter;

  fatal(ffn_say_normal_list, (ff_node*)n->chain, N, " ");

  finally : coda;
}

xapi ffn_strings_mknode(ff_node_strings ** restrict n, va_list va)
{
  enter;

  fatal(xmalloc, n, sizeof(**n));

  (*n)->chain = va_arg(va, void*);

  finally : coda;
}

void ffn_strings_destroy(ff_node_strings * restrict n)
{
  ffn_free(n->chain);
}
