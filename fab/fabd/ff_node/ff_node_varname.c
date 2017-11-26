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
#include "xlinux/xstring.h"
#include "xlinux/xstdlib.h"

#include "ff_node_varname.internal.h"
#include "ff_node.internal.h"
#include "ff_node_string.internal.h"

xapi ffn_varname_say_tree(const ff_node_varname * restrict n, int level, narrator * restrict N)
{
  enter;

  xsayf("%*sname\n", level * 2, "");
  xsayw(n->text, n->len);
  xsayf("%*svariant\n", level * 2, "");
  fatal(ffn_say_tree_level, (ff_node*)n->variant,  level + 1, N);

  finally : coda;
}

xapi ffn_varname_say_normal(const ff_node_varname * restrict n, narrator * restrict N)
{
  enter;

  xsays(n->text);
  if(n->variant)
  {
    xsays(".");
    fatal(ffn_say_normal_list, (ff_node*)n->variant, N, ".");
  }

  finally : coda;
}

xapi ffn_varname_mknode(ff_node_varname ** restrict n, va_list va)
{
  enter;

  fatal(xmalloc, n, sizeof(**n));

  (*n)->len = va_arg(va, size_t);
  fatal(ixstrndup, &(*n)->text, va_arg(va, char*), (*n)->len);
  (*n)->variant = va_arg(va, ff_node_string*);

  finally : coda;
}

void ffn_varname_destroy(ff_node_varname * restrict n)
{
  wfree(n->text);
  ffn_free(n->variant);
}
