/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "narrator.h"
#include "narrator/fixed.h"

#include "generate.internal.h"
#include "fsedge.h"
#include "fsent.h"
#include "vertex.h"
#include "vertex/probes.h"
#include "search.internal.h"
#include "entlist.h"
#include "module.h"
#include "shadow.h"

//
// static
//

static void pattern_section_generate(pattern_generate_context * restrict ctx)
{
  struct segment_traversal segment_traversal;
  pattern_segments *segments;
  moria_vertex *v;
  fsent *fse;

  narrator_reset(ctx->section_narrator);

  if(!ctx->section_traversal.section) {
    ctx->section_traversal.section = llist_first(ctx->section_traversal.head, typeof(*ctx->section_traversal.section), lln);
  } else {
    ctx->section_traversal.section = llist_next(ctx->section_traversal.head, ctx->section_traversal.section, lln);
  }

  if(!ctx->section_traversal.section)
  {
    entlist_push(ctx->hits, ctx->node);
  }
  else if(ctx->section_traversal.section->nodeset == PATTERN_NODESET_ANCHOR)
  {
    v = &ctx->match->node->vertex;
    v = moria_vertex_up(v);
    fse = containerof(v, fsent, vertex);

    ctx->node = fsent_translate(fse, ctx->mod->dir_node, ctx->mod->anchor_node, ctx->invalidation);
    pattern_section_generate(ctx);
  }
  else if(ctx->section_traversal.section->nodeset == PATTERN_NODESET_MODULE)
  {
    v = &ctx->match->node->vertex;
    v = moria_vertex_up(v);
    fse = containerof(v, fsent, vertex);

    ctx->node = fsent_translate(fse, ctx->mod->anchor_node, ctx->mod->dir_node, ctx->invalidation);
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
    if(ctx->node && vertex_type_get(&ctx->node->vertex) == VERTEX_SHADOW_MODULE)
    {
      ctx->node = ctx->mod->shadow;
      RUNTIME_ASSERT(ctx->node);
    }

    memset(&segment_traversal, 0, sizeof(segment_traversal));

    segments = llist_first(&ctx->section_traversal.section->qualifiers, typeof(*segments), lln);
    segment_traversal.head = &segments->list;

    llist_append(&ctx->segment_traversal_stack, &segment_traversal.lln);
    ctx->segment_traversal = llist_last(&ctx->segment_traversal_stack, typeof(*ctx->segment_traversal), lln);
    pattern_segment_generate(ctx);

    llist_delete(&segment_traversal.lln);
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
  const pattern_segment *segment;
  bool end;

  segment_traversal = ctx->segment_traversal;
  segment = segment_traversal->segment;
  end = segment_traversal->end;

  if(segment_traversal->end) {
    segment_traversal->segment = 0;
  } else if(!segment_traversal->segment) {
    segment_traversal->segment = llist_first(segment_traversal->head, typeof(*segment_traversal->segment), lln);
  } else {
    segment_traversal->segment = llist_next(segment_traversal->head, segment_traversal->segment, lln);
  }

  // continue with the current segment
  if(segment_traversal->segment)
  {
    segment_traversal->segment->vtab->generate(segment_traversal->segment, ctx);
    goto end;
  }
  segment_traversal->end = true;

  // ascend to the outer segment traversal
  ctx->segment_traversal = llist_prev(&ctx->segment_traversal_stack, ctx->segment_traversal, lln);
  if(ctx->segment_traversal) {
    pattern_segment_generate(ctx);
    goto end;
  }

  /* the end of a section */
  section_len = ctx->section_narrator_fixed->l;
  section = ctx->section_narrator_fixed->s;

  if(section_len == 0) {
    goto end;
  }

  next_context_vertex = 0;
  if(!ctx->node)
  {
    ctx->node = ctx->base;
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
    if(!(ctx->flags & PATTERN_GENERATE_NOFOLLOW)) {
      while(next_context_vertex->attrs & MORIA_VERTEX_LINK) {
        next_context_vertex = next_context_vertex->ref;
      }
    }
    if(vertex_shadowtype_get(next_context_vertex) == VERTEX_SHADOWTYPE_CONTEXT_MODULE) {
      next_context_vertex = &ctx->mod->shadow->vertex;
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
    fsedge_connect(next_context_node, ctx->node, ctx->invalidation);
    vertex_alloc_probe(&next_context_node->vertex);
  }

  // continue to the next section
  ctx->node = next_context_node;
  pattern_section_generate(ctx);

end:
  segment_traversal->segment = segment;
  segment_traversal->end = end;
}

//
// public
//

void pattern_generate(
    const pattern * restrict pattern
  , module * restrict mod
  , fsent * restrict base
  , graph_invalidation_context * invalidation
  , const pattern_search_node * restrict match
  , entlist * restrict hits
  , uint32_t flags
)
{
  char space[512] = { };
  narrator_fixed fixed;

  // setup the dynamic context
  pattern_generate_context ctx =  {
    /* inputs */
      base : base
    , invalidation : invalidation
    , match : match
    , mod : mod
    , flags : flags

    /* outputs */
    , hits : hits
  };

  ctx.section_traversal.head = &pattern->sections;
  ctx.section_narrator = narrator_fixed_init(&fixed, space, sizeof(space));
  llist_init(&ctx.segment_traversal_stack);

  pattern_section_generate(&ctx);
}
