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

#include "types.h"
#include "xapi.h"

#include "narrator.h"
#include "valyria/list.h"
#include "xlinux/xstdlib.h"

#include "ff_node_stem.internal.h"
#include "artifact.h"
#include "ff_node.internal.h"
#include "ff_node_pattern.internal.h"
#include "ff_node_word.internal.h"
#include "node.h"
#include "path.h"
#include "pattern/generate.internal.h"
#include "pattern/match.internal.h"

xapi ffn_stem_say_tree(const ff_node_stem * restrict n, narrator * restrict N)
{
  enter;

  if(n->num)
    xsayf("%%(%hhu)", n->num);
  else
    xsayc('%');

  finally : coda;
}

xapi ffn_stem_say_normal(const ff_node_stem * restrict n, narrator * restrict N)
{
  enter;

  if(n->num)
    xsayf("%%(%hhu)", n->num);
  else
    xsayc('%');

  finally : coda;
}

xapi ffn_stem_mknode(ff_node_stem ** restrict n, va_list va)
{
  enter;

  fatal(xmalloc, n, sizeof(**n));

  (*n)->num = va_arg(va, uint32_t);

  finally : coda;
}

void ffn_stem_destroy(ff_node_stem * restrict n)
{

}

xapi ffn_stem_match(pattern_match_context * restrict context, ffn_bydir_walk * restrict walk)
{
  enter;

  const struct node * context_node = context->node;
  uint16_t context_name_offset = context->name_offset;

  uint16_t next_offset;
  if(!walk->ffn->next)
  {
    next_offset = context->node->name->namel;
  }
  else
  {
    next_offset = context->node->name->namel - 1;
    while(next_offset >= context_name_offset)
    {
      ffn_bydir_context alt_walk_context = *walk->context;

      ffn_bydir_walk alt_walk = *walk;
      alt_walk.context = &alt_walk_context;
      alt_walk.ffn = (ff_node_pattern_part*)walk->ffn->next;

      context->name_offset = next_offset;

      fatal(pattern_part_match, context, &alt_walk);
      if(context->node)
        break;

      if(--next_offset == 0)
        break;

      context->node = context_node;
    }
  }

  if(next_offset > context_name_offset)
  {
    context->name_offset = next_offset;
    context->stem = context_node->name->name + context_name_offset;
    context->stem_len = context->name_offset - context_name_offset;
  }
  else
  {
    context->node = NULL;
  }

  finally : coda;
}

xapi ffn_stem_generate(
    pattern_generate_context * restrict context
  , ffn_bydir_walk * restrict walk
  , const artifact * restrict context_af
  , const char * restrict stem
  , uint16_t stem_len
  , list * restrict results
  , bool generating_artifact
)
{
  enter;

  if(stem && stem_len)
    fatal(narrator_xsayw, context->segment_narrator, stem, stem_len);

  walk->ffn = (typeof(walk->ffn))walk->ffn->next;
  fatal(pattern_segment_generate, context, walk, context_af, stem, stem_len, results, generating_artifact);

  finally : coda;
}
