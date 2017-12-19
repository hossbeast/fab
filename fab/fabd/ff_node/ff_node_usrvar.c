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

#include "ff_node_usrvar.internal.h"
#include "ff_node.internal.h"

#include "ff_node_string.internal.h"

xapi ffn_usrvar_say_tree(const ff_node_usrvar * restrict n, narrator * restrict N)
{
  enter;

  xsayf("$%s", n->text);

  finally : coda;
}

xapi ffn_usrvar_say_normal(const ff_node_usrvar * restrict n, narrator * restrict N)
{
  enter;

  xsayf("$%s", n->text);

  finally : coda;
}

xapi ffn_usrvar_render(const ff_node_usrvar * restrict n, ffn_render_context * restrict ctx, narrator * restrict N)
{
  enter;

  xsays(n->text);

  finally : coda;
}

xapi ffn_usrvar_mknode(ff_node_usrvar ** restrict n, va_list va)
{
  enter;

  fatal(xmalloc, n, sizeof(**n));

  (*n)->len = va_arg(va, size_t);
  fatal(ixstrndup, &(*n)->text, va_arg(va, char*), (*n)->len);

  finally : coda;
}

void ffn_usrvar_destroy(ff_node_usrvar * restrict n)
{
  wfree(n->text);
}
