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

#include "narrator.h"
#include "narrator/fixed.h"
#include "valyria/set.h"

#include "generate.internal.h"
#include "variant.h"
#include "fsedge.h"
#include "fsent.h"
#include "search.internal.h"
#include "module.internal.h"
#include "shadow.h"

//
// static
//

static void pattern_section_generate(pattern_generate_context * restrict ctx)
{
  struct segment_traversal segment_traversal;
  const variant * var = 0;
  moria_vertex * v;

  narrator_xreset(ctx->section_narrator);

  ctx->section_traversal.section = chain_next(ctx->section_traversal.head, &ctx->section_traversal.cursor, chn);

  if(!ctx->section_traversal.section)
  {
    if(ctx->node && (fsent_kind_get(ctx->node) == VERTEX_SHADOW_MODULE || fsent_kind_get(ctx->node) == VERTEX_SHADOW_MODULES))
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

    set_put(ctx->nodes, ctx->node, 0);
  }
  else if(ctx->section_traversal.section->nodeset == PATTERN_NODESET_MATCHDIR)
  {
    v = &ctx->match->node->vertex;
    v = moria_vertex_up(v);
    ctx->node = containerof(v, fsent, vertex);

    pattern_section_generate(ctx);
  }
  else if(ctx->section_traversal.section->nodeset == PATTERN_NODESET_SELF)
  {
    pattern_section_generate(ctx);
  }
  else if(ctx->section_traversal.section->nodeset == PATTERN_NODESET_SHADOW)
  {
    v = &g_shadow->vertex;
    ctx->node = containerof(v, fsent, vertex);
    RUNTIME_ASSERT(ctx->node);

    pattern_section_generate(ctx);
  }
  else if(ctx->section_traversal.section->nodeset == PATTERN_NODESET_ROOT)
  {
    v = &g_root->vertex;
    ctx->node = containerof(v, fsent, vertex);
    RUNTIME_ASSERT(ctx->node);

    pattern_section_generate(ctx);
  }
  else
  {
    if(ctx->node && fsent_kind_get(ctx->node) == VERTEX_SHADOW_MODULE)
    {
      ctx->node = ctx->mod->shadow;
      RUNTIME_ASSERT(ctx->node);
    }

    segment_traversal = (typeof(segment_traversal)) {
        head : ctx->section_traversal.section->qualifiers_head->segment_head
    };
    llist_append(&ctx->segment_traversal_stack, &segment_traversal, lln);
    ctx->segment_traversal = llist_last(&ctx->segment_traversal_stack, typeof(*ctx->segment_traversal), lln);
    pattern_segment_generate(ctx);

    llist_delete(&segment_traversal, lln);
  }
}

//
// internal
//

void pattern_segment_generate(pattern_generate_context * restrict ctx)
{
  fsent * next_context_node;
  moria_vertex * next_context_vertex;
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
    segment_traversal->segment->vtab->generate(segment_traversal->segment, ctx);
    goto XAPI_FINALIZE;
  }

  // ascend to the outer segment traversal
  ctx->segment_traversal = llist_prev(&ctx->segment_traversal_stack, ctx->segment_traversal, lln);
  if(ctx->segment_traversal)
  {
    pattern_segment_generate(ctx);
    goto XAPI_FINALIZE;
  }

  /* the end of a section */
  section_len = ctx->section_narrator_fixed->l;
  section = ctx->section_narrator_fixed->s;

  if(section_len == 0)
  {
    goto XAPI_FINALIZE;
  }

  next_context_vertex = 0;
  if(!ctx->node)
  {
    if((next_context_vertex = moria_vertex_downw(ctx->scope, section, section_len)))
    {
      ctx->node = containerof(next_context_vertex, fsent, vertex);
    }
    else
    {
      ctx->node = ctx->base;
    }
  }

  if(!next_context_vertex)
  {
    next_context_vertex = moria_vertex_downw(
        &ctx->node->vertex
      , section
      , section_len
    );
  }
  if(next_context_vertex)
  {
    while(next_context_vertex->attrs & MORIA_VERTEX_LINK) {
      next_context_vertex = next_context_vertex->ref;
    }
  }

  // only the final section is a file
  if(next_context_vertex)
  {
    next_context_node = containerof(next_context_vertex, fsent, vertex);
  }
  else
  {
    /* create as not yet existing */
    fsent_create(&next_context_node, VERTEX_FILE, VERTEX_UNCREATED, section, section_len);
    fsedge_connect(ctx->node, next_context_node, ctx->invalidation);
  }

  // continue to the next section
  ctx->node = next_context_node;
  pattern_section_generate(ctx);

finally:
  segment_traversal->cursor = segchain;
coda;
}

//
// public
//

void pattern_generate(
    const pattern * restrict pattern
  , module * restrict mod
  , fsent * restrict base
  , fsent * restrict scope
  , const set * restrict variants
  , graph_invalidation_context * invalidation
  , const pattern_search_node * restrict match
  , set * restrict results
)
{
  char space[512] = { };
  narrator_fixed fixed;

  RUNTIME_ASSERT(!mod || base);
  RUNTIME_ASSERT(!mod || scope);

  // setup the dynamic context
  pattern_generate_context ctx =  {
    /* inputs */
      base : base
    , variants : variants
    , invalidation : invalidation
    , match : match
    , mod : mod

    /* outputs */
    , nodes : results
  };

  if(scope)
  {
    /* generate patterns begin at scope fsent */
    ctx.scope = &scope->vertex;
    while(ctx.scope->attrs & MORIA_VERTEX_LINK) {
      ctx.scope = ctx.scope->ref;
    }
  }

  ctx.section_traversal.head = pattern->section_head;
  ctx.section_traversal.variant_index = -1;
  ctx.section_narrator = narrator_fixed_init(&fixed, space, sizeof(space));
  ctx.segment_traversal_stack = LLIST_INITIALIZER(ctx.segment_traversal_stack);

  pattern_section_generate(&ctx);
}
