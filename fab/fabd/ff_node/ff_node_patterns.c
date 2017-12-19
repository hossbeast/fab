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
#include "narrator/growing.h"

#include "ff_node_patterns.internal.h"
#include "ff_node.internal.h"
#include "ff_node_pattern.internal.h"

//
// public
//

xapi ffn_patterns_say_tree(const ff_node_patterns * restrict n, int level, narrator * restrict N)
{
  enter;

  xsayf("%*schain\n", level * 2, "");
  fatal(ffn_say_tree_level, (ff_node*)n->chain, level + 1, N);

  finally : coda;
}

xapi ffn_patterns_say_normal(const ff_node_patterns * restrict n, narrator * restrict N)
{
  enter;

  fatal(ffn_say_normal_list, (ff_node*)n->chain, N, ", ");

  finally : coda;
}

xapi ffn_patterns_mknode(ff_node_patterns ** restrict n, va_list va)
{
  enter;

  fatal(xmalloc, n, sizeof(**n));

  (*n)->chain = va_arg(va, typeof((*n)->chain));
  

  finally : coda;
}

void ffn_patterns_destroy(ff_node_patterns * restrict n)
{
  ffn_free(n->chain);
}

xapi ffn_patterns_render(const ff_node_patterns * restrict pats, rendered_patterns ** block)
{
  enter;

  narrator * N = 0;
  ffn_render_context ctx = {};

  fatal(narrator_growing_create, &N);

  // save a place in the buffer for the number of paths
  xsayw((uint16_t[]) { 0 }, sizeof(uint16_t));

  const ff_node_pattern * pat = pats->chain;
  while(pat)
  {
    // save the current offset
    fatal(narrator_xseek, N, 0, NARRATOR_SEEK_CUR, &ctx.start);

    // save a place in the buffer for the length
    xsayw((uint16_t[]) { 0 }, sizeof(uint16_t));

    fatal(ffn_pattern_render, pat, &ctx, N);
    pat = (typeof(pat))pat->next;
  }

  // record the number of patterns
  fatal(narrator_xseek, N, 0, NARRATOR_SEEK_SET, 0);
  xsayw(&ctx.num, sizeof(ctx.num));

  // ownership transfer
  *block = (void*)narrator_growing_buffer(N);
  N = 0;

finally:
  fatal(narrator_xfree, N);
coda;
}
