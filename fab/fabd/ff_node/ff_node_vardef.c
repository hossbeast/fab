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

#include "narrator.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"

#include "ff_node_vardef.internal.h"
#include "ff_node.internal.h"
#include "ff_node_varname.internal.h"
#include "ff_node_strings.internal.h"

xapi ffn_vardef_say_tree(const ff_node_vardef * restrict n, int level, narrator * restrict N)
{
  enter;

  xsayf("%*sname\n", level * 2, "");
  fatal(ffn_say_tree_level, (ff_node*)n->name, level + 1, N);
  xsayf("%*sdef\n", level * 2, "");
  fatal(ffn_say_tree_level, (ff_node*)n->def, level + 1, N);

  finally : coda;
}

xapi ffn_vardef_say_normal(const ff_node_vardef * restrict n, narrator * restrict N)
{
  enter;

  xsays("var ");
  fatal(ffn_say_normal_list, (ff_node*)n->name, N, 0);
  xsays(" ");
  fatal(ffn_say_normal_list, (ff_node*)n->def, N, 0);

  finally : coda;
}

xapi ffn_vardef_mknode(ff_node_vardef ** restrict n, va_list va)
{
  enter;

  fatal(xmalloc, n, sizeof(**n));

  (*n)->name = va_arg(va, ff_node_varname*);
  (*n)->def = va_arg(va, ff_node_strings*);

  finally : coda;
}

void ffn_vardef_destroy(ff_node_vardef * restrict n)
{
  ffn_free((ff_node*)n->name);
  ffn_free((ff_node*)n->def);
}
