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
#include "narrator/fixed.h"
#include "valyria/list.h"
#include "xlinux/xstdlib.h"

#include "ff_node_class.internal.h"
#include "artifact.h"
#include "ff_node.internal.h"
#include "ff_node_pattern.internal.h"
#include "ff_node_range.internal.h"
#include "node.h"
#include "path.h"
#include "generate.internal.h"
#include "match.internal.h"

//
// public
//

xapi ffn_class_say_tree(const ff_node_class * restrict n, int level, narrator * restrict N)
{
  enter;

  xsayf("%*sattrs ", level * 2, "");
  fatal(ffn_attrs_say, n->attrs, N);
  xsays("\n");
  xsayf("%*schain\n", level * 2, "");
  fatal(ffn_say_tree_level, (ff_node*)n->chain, level + 1, N);

  finally : coda;
}

xapi ffn_class_say_normal(const ff_node_class * restrict n, narrator * restrict N)
{
  enter;

  xsayc('[');
  if(n->attrs & FFN_INVERT)
    xsayc('^');
  fatal(ffn_say_normal_list, (ff_node*)n->chain, N, 0);
  xsayc(']');
  if(n->attrs & FFN_VARIANT_GROUP)
    xsayc('?');

  finally : coda;
}

xapi ffn_class_render(const ff_node_class * restrict ffn, ffn_render_context * restrict ctx, narrator * restrict N)
{
  enter;

  char prev[512];
  size_t prevl;

  // store the preceeding content
  fatal(narrator_xseek, N, ctx->start + 2, NARRATOR_SEEK_SET, 0);
  fatal(narrator_xread, N, prev, sizeof(prev), &prevl);

  ff_node_class_part * part = ffn->chain;
  while(part)
  {
    uint8_t start = 0;
    uint8_t end = 0;

    if(part->type == FFN_CHAR)
    {
      start = end = part->character.code;
    }
    else if(part->type == FFN_RANGE)
    {
      start = part->range.start->code;
      end = part->range.end->code;
    }

    uint8_t c;
    for(c = start; c <= end; c++)
    {
      if(c != start || part != ffn->chain)
      {
        // mark this items start, replay preceeding content
        fatal(narrator_xseek, N, 0, NARRATOR_SEEK_CUR, &ctx->start);
        xsayw((uint16_t[]) { 0 }, sizeof(uint16_t));
        xsayw(prev, prevl);
      }

      xsayc(c);
      fatal(render_tail, ffn, ctx, N);
    }

    part = (typeof(part))part->next;
  }

  finally : coda;
}

xapi ffn_class_mknode(ff_node_class ** restrict n, va_list va)
{
  enter;

  fatal(xmalloc, n, sizeof(**n));

  (*n)->chain = va_arg(va, typeof((*n)->chain));
  (*n)->attrs = (typeof((*n)->attrs))va_arg(va, int);

  ffn_chain_attach(*n, (*n)->chain);

  finally : coda;
}

void ffn_class_destroy(ff_node_class * restrict n)
{
  ffn_free(n->chain);
}

xapi ffn_class_match(pattern_match_context * restrict context, ffn_bydir_walk * restrict walk)
{
  enter;

  const char * name = context->node->name->name;
  int namel = context->node->name->namel;

  const node * context_node = context->node;
  uint16_t context_name_offset = context->name_offset;

  ff_node_class_part * part = walk->ffn->class.chain;
  while(part)
  {
    if(part->type == FFN_CHAR)
    {
      if(namel > context->name_offset)
      {
        if(part->character.code == *(name + context->name_offset))
        {
          (context->name_offset) += 1;
          goto XAPI_FINALIZE;
        }
      }
    }
    else if(part->type == FFN_RANGE)
    {
      fatal(ffn_range_match, context, &part->range);
      if(context->node)
        goto XAPI_FINALIZE;

      context->node = context_node;
      context->name_offset = context_name_offset;
    }

    part = (typeof(part))part->next;
  }

  context->node = 0;

  finally : coda;
}

xapi ffn_class_generate(
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

  node * alt_context_node = context->node;

  const ff_node_class_part * alt_ffn = walk->ffn->class.chain;
  while(alt_ffn)
  {
    uint8_t start = 0;
    uint8_t end = 0;

    if(alt_ffn->type == FFN_CHAR)
    {
      start = end = alt_ffn->character.code;
    }
    else if(alt_ffn->type == FFN_RANGE)
    {
      start = alt_ffn->range.start->code;
      end = alt_ffn->range.end->code;
    }

    while(true)
    {
      // reset
      fatal(narrator_xseek, context->segment_narrator, alt_base_pos, NARRATOR_SEEK_SET, 0);
      context->segment_base_pos = segment_base_pos;

      // render
      fatal(narrator_xsayc, context->segment_narrator, start);

      // capture
      if(walk->ffn->attrs & FFN_VARIANT_GROUP)
      {
        context->variant_pos = alt_base_pos;
        context->variant_len = 1;
      }

      // continue
      context->node = alt_context_node;

      ffn_bydir_context alt_walk_context = *walk->context;
      ffn_bydir_walk alt_walk = *walk;
      alt_walk.context = &alt_walk_context;
      alt_walk.ffn = (ff_node_pattern_part*)walk->ffn->next;

      fatal(pattern_segment_generate, context, &alt_walk, context_af, stem, stem_len, results, opts);

      if(start == end)
        break;
      start++;
    }

    alt_ffn = (ff_node_class_part*)alt_ffn->next;
  }

  finally : coda;
}
