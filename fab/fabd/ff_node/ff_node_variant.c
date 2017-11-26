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

#include <string.h>

#include "xapi.h"
#include "types.h"

#include "narrator.h"
#include "xlinux/xstdlib.h"
#include "valyria/list.h"

#include "ff_node_variant.internal.h"
#include "ff_node.internal.h"
#include "artifact.h"
#include "ff_node_pattern.internal.h"
#include "node.h"
#include "path.h"
#include "pattern/generate.internal.h"
#include "pattern/match.internal.h"

xapi ffn_variant_say_tree(const ff_node_variant * restrict n, narrator * restrict N)
{
  enter;

  xsayc('?');

  finally : coda;
}

xapi ffn_variant_say_normal(const ff_node_variant * restrict n, narrator * restrict N)
{
  enter;

  xsayc('?');

  finally : coda;
}

xapi ffn_variant_mknode(ff_node_variant ** restrict n, va_list va)
{
  enter;

  fatal(xmalloc, n, sizeof(**n));

  finally : coda;
}

void ffn_variant_destroy(ff_node_variant * restrict n)
{

}

xapi ffn_variant_match(pattern_match_context * restrict context, ffn_bydir_walk * restrict walk)
{
  enter;

  const char * name = context->node->name->name;
  int namel = context->node->name->namel;

  if((namel - context->name_offset) >= context->variant_len && strncmp(name + context->name_offset, context->variant, context->variant_len) == 0)
    context->name_offset += context->variant_len;
  else
    context->node = 0;

  finally : coda;
}

xapi ffn_variant_generate(
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

  if(context_af && context_af->variant_len)
    fatal(narrator_xsayw, context->segment_narrator, context_af->variant, context_af->variant_len);

  walk->ffn = (typeof(walk->ffn))walk->ffn->next;
  fatal(pattern_segment_generate, context, walk, context_af, stem, stem_len, results, opts);

  finally : coda;
}
