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

#include "xapi.h"
#include "types.h"

#include "moria/vertex.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "valyria/list.h"
#include "xlinux/xstdlib.h"

#include "generate.internal.h"
#include "artifact.h"
#include "errtab/FABD.errtab.h"
#include "ff_node.h"
#include "ff_node_alternation.h"
#include "ff_node_class.h"
#include "ff_node_dirsep.h"
#include "ff_node_pattern.h"
#include "ff_node_patterns.h"
#include "ff_node_stem.h"
#include "ff_node_string.h"
#include "ff_node_variant.h"
#include "ff_node_word.h"
#include "node.h"
#include "node_operations.h"
#include "path.h"

//
// internal
//

xapi pattern_segment_generate(
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

  artifact * af = 0;

  // the end of a slash-delimited section of pattern
  if(walk->ffn == walk->context->stop)
  {
    ffn_pattern_bydir_ltr(walk->context);

    size_t section_len = narrator_fixed_size(context->segment_narrator) - context->segment_base_pos;
    const char * section = narrator_fixed_buffer(context->segment_narrator) + context->segment_base_pos;

    if(walk->outer && walk->outer->ffn->attrs & FFN_VARIANT_GROUP)
    {
      context->variant_len = narrator_fixed_size(context->segment_narrator) - context->variant_pos;
    }

    // next is a slash, or the end of the outermost pattern
    if(section_len > 0 && (walk->ffn || walk->outer == NULL))
    {
      vertex * next_context_vertex = vertex_travel_vertexw(
          vertex_containerof(context->node)
        , section
        , section_len
        , 0
        , NODE_RELATION_FS
        , MORIA_TRAVERSE_DOWN
      );

      // only the final section is a file
      node_fstype fstype = NODE_FSTYPE_FILE;
      if(walk->ffn)
        fstype = NODE_FSTYPE_DIR;

      node * next_context_node;
      if(next_context_vertex)
      {
        next_context_node = vertex_value(next_context_vertex);
        if(next_context_node->fstype != fstype)
        {
          char space[512];
          size_t z = node_get_relative_path(next_context_node, space, sizeof(space));

          xapi_info_pushw("path", space, z);
          fail(FABD_AMBIGPATH);
        }
      }
      else
      {
        fatal(node_createw, &next_context_node, fstype, context->node->fs, section, section_len);
        fatal(node_connect_fs, context->node, next_context_node);
      }

      if(next_context_node->fstype == NODE_FSTYPE_FILE)
      {
        if(generating_artifact)
        {
          fatal(xmalloc, &af, sizeof(*af));
          af->node = next_context_node;
          if(context->variant_pos)
          {
            af->variant = next_context_node->name->name + context->variant_pos;
            af->variant_len = context->variant_len;
          }
          fatal(list_push, results, af);
          af = 0;
        }
        else
        {
          fatal(list_push, results, next_context_node);
        }
      }
      else
      {
        /* TODO : call notify_thread_watch for new directories */
        context->node = next_context_node;
      }
    }

    // start of the next slash-delimited section of the pattern
    if(walk->ffn)
    {
      context->segment_base_pos += section_len + 1;
      fatal(pattern_segment_generate, context, walk, context_af, stem, stem_len, results, generating_artifact);
    }

    // continue the outer walk
    else if(walk->outer)
    {
      ffn_bydir_context outer_walk_context = *walk->outer->context;
      ffn_bydir_walk outer_walk = *walk->outer;
      outer_walk.context = &outer_walk_context;

      outer_walk.ffn = (ff_node_pattern_part*)outer_walk.ffn->next;

      fatal(pattern_segment_generate, context, &outer_walk, context_af, stem, stem_len, results, generating_artifact);
    }
  }
  else if(walk->ffn->type == FFN_ALTERNATION)
  {
    fatal(ffn_alternation_generate, context, walk, context_af, stem, stem_len, results, generating_artifact);
  }
  else if(walk->ffn->type == FFN_CHAR)
  {
    fatal(ffn_char_generate, context, walk, context_af, stem, stem_len, results, generating_artifact);
  }
  else if(walk->ffn->type == FFN_CLASS)
  {
    fatal(ffn_class_generate, context, walk, context_af, stem, stem_len, results, generating_artifact);
  }
  else if(walk->ffn->type == FFN_STEM)
  {
    fatal(ffn_stem_generate, context, walk, context_af, stem, stem_len, results, generating_artifact);
  }
  else if(walk->ffn->type == FFN_WORD)
  {
    fatal(ffn_word_generate, context, walk, context_af, stem, stem_len, results, generating_artifact);
  }
  else if(walk->ffn->type == FFN_VARIANT)
  {
    fatal(ffn_variant_generate, context, walk, context_af, stem, stem_len, results, generating_artifact);
  }
  else if(walk->ffn->type == FFN_DIRSEP)
  {
    fatal(ffn_dirsep_generate, context, walk, context_af, stem, stem_len, results, generating_artifact);
  }

finally:
  wfree(af);
coda;
}

//
// public
//

xapi pattern_generate(
    const ff_node_pattern * restrict pat
  , node * restrict context_node
  , const artifact * restrict context_af
  , const char * restrict stem
  , uint16_t stem_len
  , list * restrict results
  , bool generating_artifact
)
{
  enter;

  char space[512] = { };
  narrator_fixed_storage fixed = { s : space, a : sizeof(space) };

  // setup the dynamic context
  pattern_generate_context context =  { node : context_node };
  context.segment_narrator = narrator_fixed_init(&fixed);

  // setup the outermost walk
  ffn_bydir_context walk_context;
  ffn_pattern_bydir_ltr_init(pat->chain, &walk_context);

  ffn_bydir_walk walk = {
      context : &walk_context
    , ffn : walk_context.first
  };

  fatal(pattern_segment_generate, &context, &walk, context_af, stem, stem_len, results, generating_artifact);

  finally : coda;
}
