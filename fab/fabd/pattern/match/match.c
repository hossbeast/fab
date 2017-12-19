/* Copyright (c) 2012-2017 Todd Freed <todd.freed@gmail.com>

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
#include "moria/vertex.h"

#include "pattern/match.internal.h"
#include "artifact.h"
#include "ff_node.h"
#include "ff_node_alternation.h"
#include "ff_node_class.h"
#include "ff_node_pattern.h"
#include "ff_node_stem.h"
#include "ff_node_word.h"
#include "node.h"
#include "path.h"

//
// internal
//

xapi pattern_part_match(pattern_match_context * restrict context, ffn_bydir_walk * restrict walk)
{
  enter;

  while(context->node && walk->ffn)
  {
    while(context->node && walk->ffn != walk->context->stop)
    {
      if(walk->ffn->type == FFN_ALTERNATION)
      {
        fatal(ffn_alternation_match, context, walk);
      }
      else if(walk->ffn->type == FFN_CHAR)
      {
        fatal(ffn_char_match, context, walk);
      }
      else if(walk->ffn->type == FFN_CLASS)
      {
        fatal(ffn_class_match, context, walk);
      }
      else if(walk->ffn->type == FFN_STEM)
      {
        fatal(ffn_stem_match, context, walk);
      }
      else if(walk->ffn->type == FFN_WORD)
      {
        fatal(ffn_word_match, context, walk);
      }
      else if(walk->ffn->type == FFN_VARIANT)
      {
        fatal(ffn_variant_match, context, walk);
      }

      walk->ffn = (ff_node_pattern_part*)walk->ffn->next;
    }

    if(context->node)
    {
      ffn_pattern_bydir_rtl(walk->context);
      if((walk->ffn = walk->context->first))
      {
        context->node = node_fsparent(context->node);
        context->name_offset = 0;
      }
    }
  }

  finally : coda;
}

//
// public
//

xapi pattern_match(
    const ff_node_pattern * restrict pattern
  , const node * restrict n
  , const artifact * restrict af
  , bool * restrict r
  , const char ** restrict stem
  , uint16_t * restrict stem_len
)
{
  enter;

  // setup the dynamic context
  pattern_match_context context = {
      node : n
    , variant : af->variant
    , variant_len : af->variant_len
  };

  // setup the walk
  ffn_bydir_context walk_context;
  ffn_pattern_bydir_rtl_init(pattern->chain, &walk_context);

  ffn_bydir_walk walk = {
      context : &walk_context
    , ffn : walk_context.first
  };

  fatal(pattern_part_match, &context, &walk);

  // the match was successful if there is any context node, and its name was fully matched
  if(context.node && context.name_offset == context.node->name->namel)
  {
    *r = true;
    *stem = context.stem;
    *stem_len = context.stem_len;
  }
  else
  {
    *r = false;
  }

  finally : coda;
}
