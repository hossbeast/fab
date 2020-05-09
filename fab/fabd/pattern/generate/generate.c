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
#include "valyria/set.h"
#include "valyria/map.h"
#include "xlinux/xstdlib.h"
#include "xapi/SYS.errtab.h"

#include "generate.internal.h"
#include "variant.h"
#include "errtab/FABD.errtab.h"
#include "pattern.internal.h"
#include "section.internal.h"
#include "filesystem.h"
#include "module.internal.h"
#include "node.h"
#include "shadow.h"
#include "node_operations.h"
#include "match.internal.h"
#include "path.h"
#include "attrs.h"

//
// static
//

static xapi pattern_section_generate(pattern_generate_context * restrict ctx)
{
  enter;

  struct segment_traversal segment_traversal;
  const variant * var = 0;
  vertex * v;

  fatal(narrator_xreset, ctx->section_narrator);

  ctx->section_traversal.section = chain_next(ctx->section_traversal.head, &ctx->section_traversal.cursor, chn);

  if(!ctx->section_traversal.section)
  {
    if(ctx->node && (node_kind_get(ctx->node) == VERTEX_SHADOW_MODULE || node_kind_get(ctx->node) == VERTEX_SHADOW_MODULES))
    {
      RUNTIME_ABORT();
    }

    // apply the variant from the match, if it was used in the generate pattern
    var = ctx->section_traversal.variant_replacement;

    // variant from the generate pattern wins (the ? token)
    if(ctx->section_traversal.variant_index != -1)
      var = set_table_get(ctx->variants, ctx->section_traversal.variant_index);

    if(var)
    {
      RUNTIME_ASSERT(ctx->node->var == 0 || ctx->node->var == var);
      ctx->node->var = var;
    }

    fatal(set_put, ctx->nodes, ctx->node, 0);
  }
  else if(ctx->section_traversal.section->nodeset == PATTERN_NODESET_MATCHDIR)
  {
    v = vertex_containerof(ctx->match->node);
    v = vertex_up(v);
    ctx->node = vertex_value(v);

    fatal(pattern_section_generate, ctx);
  }
  else if(ctx->section_traversal.section->nodeset == PATTERN_NODESET_SELF)
  {
    fatal(pattern_section_generate, ctx);
  }
  else if(ctx->section_traversal.section->nodeset == PATTERN_NODESET_SHADOW)
  {
    v = vertex_containerof(g_shadow);
    ctx->node = vertex_value(v);
    RUNTIME_ASSERT(ctx->node);

    fatal(pattern_section_generate, ctx);
  }
  else
  {
    if(ctx->node && node_kind_get(ctx->node) == VERTEX_SHADOW_MODULE)
    {
      ctx->node = ctx->mod->shadow;
      RUNTIME_ASSERT(ctx->node);
    }

    segment_traversal = (typeof(segment_traversal)) {
        head : ctx->section_traversal.section->qualifiers_head->segment_head
    };
    llist_append(&ctx->segment_traversal_stack, &segment_traversal, lln);
    ctx->segment_traversal = llist_last(&ctx->segment_traversal_stack, typeof(*ctx->segment_traversal), lln);
    fatal(pattern_segment_generate, ctx);

    llist_delete(&segment_traversal, lln);
  }

  finally : coda;
}

//
// internal
//

xapi pattern_segment_generate(pattern_generate_context * restrict ctx)
{
  enter;

  node * next_context_node;
  vertex * next_context_vertex;
  const char * section;
  size_t section_len;
  struct segment_traversal *segment_traversal;
  const chain * segchain;

  segment_traversal = ctx->segment_traversal;
  segchain = segment_traversal->cursor;

  segment_traversal->segment = chain_next(segment_traversal->head, &segment_traversal->cursor, chn);

  // continue with the current segment
  if(segment_traversal->segment)
  {
    fatal(segment_traversal->segment->vtab->generate, segment_traversal->segment, ctx);
    goto XAPI_FINALIZE;
  }

  // ascend to the outer segment traversal
  ctx->segment_traversal = llist_prev(&ctx->segment_traversal_stack, ctx->segment_traversal, lln);
  if(ctx->segment_traversal)
  {
    fatal(pattern_segment_generate, ctx);
    goto XAPI_FINALIZE;
  }

  /* the end of a section */
  section_len = narrator_fixed_size(ctx->section_narrator);
  section = narrator_fixed_buffer(ctx->section_narrator);

  if(section_len == 0)
  {
    goto XAPI_FINALIZE;
  }

  next_context_vertex = 0;
  if(!ctx->node)
  {
    if((next_context_vertex = vertex_downw(ctx->scope, section, section_len)))
    {
      ctx->node = vertex_value(next_context_vertex);
    }
    else
    {
      ctx->node = ctx->base;
    }
  }

  if(!next_context_vertex)
  {
    next_context_vertex = vertex_downw(
        vertex_containerof(ctx->node)
      , section
      , section_len
    );
  }

  // only the final section is a file
  if(next_context_vertex)
  {
    next_context_node = vertex_value(next_context_vertex);
    if(node_kind_get(next_context_node) == VERTEX_SHADOW_LINK)
    {
      next_context_node = vertex_value(next_context_vertex->ref);
      RUNTIME_ASSERT(next_context_node);
    }
  }
  else
  {
    /* create as not yet existing */
    fatal(node_createw, &next_context_node, VERTEX_UNCREATED, 0, 0, section, section_len);
    fatal(node_connect, ctx->node, next_context_node, EDGE_TYPE_FS, ctx->invalidation, 0, 0);
  }

  // continue to the next section
  ctx->node = next_context_node;
  fatal(pattern_section_generate, ctx);

finally:
  segment_traversal->cursor = segchain;
coda;
}

//
// public
//

xapi pattern_generate(
    const pattern * restrict pattern
  , module * restrict mod
  , const set * restrict variants
  , graph_invalidation_context * invalidation
  , const pattern_match_node * restrict match
  , set * restrict results
)
{
  enter;

  char space[512] = { 0 };
  char fixed[NARRATOR_STATIC_SIZE];

  fatal(set_recycle, results);

  // setup the dynamic context
  pattern_generate_context ctx =  {
    /* inputs */
      base : mod->dir_node
    /* generate patterns begin at the module scope */
    , scope : vertex_containerof(mod->shadow_scope)
    , variants : variants
    , invalidation : invalidation
    , match : match
    , mod : mod

    /* outputs */
    , nodes : results
  };

  ctx.section_traversal.head = pattern->section_head;
  ctx.section_traversal.variant_index = -1;
  ctx.section_narrator = narrator_fixed_init(fixed, space, sizeof(space));

  ctx.segment_traversal_stack = LLIST_INITIALIZER(ctx.segment_traversal_stack);

  fatal(pattern_section_generate, &ctx);

  finally : coda;
}
