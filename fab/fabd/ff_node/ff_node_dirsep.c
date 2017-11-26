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
#include "valyria/list.h"

#include "ff_node_dirsep.internal.h"
#include "ff_node.internal.h"
#include "artifact.h"
#include "ff_node_pattern.internal.h"
#include "node.h"
#include "path.h"
#include "pattern/generate.internal.h"
#include "pattern/match.internal.h"

xapi ffn_dirsep_say_tree(const ff_node_dirsep * restrict n, narrator * restrict N)
{
  enter;

  xsayc('/');

  finally : coda;
}

xapi ffn_dirsep_say_normal(const ff_node_dirsep * restrict n, narrator * restrict N)
{
  enter;

  xsayc('/');

  finally : coda;
}

xapi ffn_dirsep_mknode(ff_node_dirsep ** restrict n, va_list va)
{
  enter;

  fatal(xmalloc, n, sizeof(**n));

  finally : coda;
}

void ffn_dirsep_destroy(ff_node_dirsep * restrict n)
{

}

xapi ffn_dirsep_generate(
    pattern_generate_context * restrict context
  , ffn_bydir_walk * restrict walk
  , const artifact * restrict context_af
  , const char * restrict stem
  , uint16_t stem_len
  , list * restrict results
  , uint8_t opts
)
{
  enter;

  fatal(narrator_xsayc, context->segment_narrator, '/');
  walk->ffn = (typeof(walk->ffn))walk->ffn->next;
  fatal(pattern_segment_generate, context, walk, context_af, stem, stem_len, results, opts);

  finally : coda;
}
