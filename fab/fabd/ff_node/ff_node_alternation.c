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
#include "valyria/list.h"

#include "ff_node_alternation.internal.h"
#include "ff_node.internal.h"
#include "ff_node_pattern.internal.h"
#include "artifact.h"
#include "node.h"
#include "match.internal.h"
#include "generate.internal.h"
#include "path.h"

#include "macros.h"

//
// public
//

xapi ffn_alternation_say_tree(const ff_node_alternation * restrict n, int level, narrator * restrict N)
{
  enter;

  xsayf("%*sattrs ", level * 2, "");
  fatal(ffn_attrs_say, n->attrs, N);
  xsays("\n");
  xsayf("%*slist\n", level * 2, "");
  fatal(ffn_say_tree_level, n->chain, level + 1, N);

  finally : coda;
}

xapi ffn_alternation_say_normal(const ff_node_alternation * restrict n, narrator * restrict N)
{
  enter;

  xsayc('{');
  fatal(ffn_say_normal_list, n->chain, N, ",");
  xsayc('}');
  if(n->attrs & FFN_VARIANT_GROUP)
    xsayc('?');

  finally : coda;
}

xapi ffn_alternation_mknode(ff_node_alternation ** restrict n, va_list va)
{
  enter;

  fatal(xmalloc, n, sizeof(**n));

  (*n)->chain = va_arg(va, void*);
  (*n)->attrs = (typeof((*n)->attrs))va_arg(va, int);

  finally : coda;
}

void ffn_alternation_destroy(ff_node_alternation * restrict n)
{
  ffn_free(&n->chain->base);
}

xapi ffn_alternation_match(pattern_match_context * restrict context, ffn_bydir_walk * restrict walk)
{
  enter;

  const node * context_node = context->node;
  uint16_t context_name_offset = context->name_offset;

  const ff_node_pattern * part = walk->ffn->alternation.chain;
  while(part)
  {
    ffn_bydir_context alt_walk_context;
    ffn_pattern_bydir_rtl_init(part->chain, &alt_walk_context);

    ffn_bydir_walk alt_walk;
    alt_walk.context = &alt_walk_context;
    alt_walk.ffn = alt_walk_context.first;

    fatal(pattern_part_match, context, &alt_walk);
    if(context->node)
      break;

    context->node = context_node;
    context->name_offset = context_name_offset;
    part = (typeof(part))part->next;
  }

  if(!part)
    context->node = 0;

  finally : coda;
}

xapi ffn_alternation_generate(
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

  // start of the alternation
  off_t alt_base_pos;
  fatal(narrator_xseek, context->segment_narrator, 0, NARRATOR_SEEK_CUR, &alt_base_pos);

  // start of the segment
  off_t segment_base_pos = context->segment_base_pos;

  node *alt_context_node = context->node;

  if(walk->ffn->attrs & FFN_VARIANT_GROUP)
  {
    context->variant_pos = alt_base_pos;
    context->variant_len = 0;
  }

  const ff_node_pattern * alt_ffn = walk->ffn->alternation.chain;
  while(alt_ffn)
  {
    fatal(narrator_xseek, context->segment_narrator, alt_base_pos, NARRATOR_SEEK_SET, 0);
    context->segment_base_pos = segment_base_pos;

    ffn_bydir_context alt_walk_context;
    ffn_pattern_bydir_ltr_init(alt_ffn->chain, &alt_walk_context);

    ffn_bydir_walk alt_walk;
    alt_walk.context = &alt_walk_context;
    alt_walk.ffn = alt_walk_context.first;

    alt_walk.outer = walk;

    context->node = alt_context_node;
    fatal(pattern_segment_generate, context, &alt_walk, context_af, stem, stem_len, results, opts);

    alt_ffn = (typeof(alt_ffn))alt_ffn->next;
  }

  finally : coda;
}
