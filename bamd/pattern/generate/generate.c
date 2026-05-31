/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "types.h"

#include "narrator.h"
#include "narrator/fixed.h"
#include "bacon/value.h"
#include "bacon/parser.h"

#include "generate.internal.h"
#include "fsedge.h"
#include "fsent.h"
#include "pattern/section.h"
#include "vertex.h"
#include "vertex/probes.h"
#include "search.internal.h"
#include "edges.h"
#include "vertices.h"
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
    //vertices_push(ctx->hits, &ctx->node->vertex);
    ctx->hit_visit(ctx->hit_ctx, &ctx->node->vertex);
  }
  else if(ctx->section_traversal.section->nodeset == PATTERN_NODESET_ANCHOR_REL)
  {
    v = &ctx->match->node->vertex;
    v = moria_vertex_up(v);
    fse = containerof(v, fsent, vertex);

    ctx->node = fsent_translate(fse, ctx->mod->build_dir, ctx->mod->anchor_dir, ctx->invalidation);
    pattern_section_generate(ctx);
  }
  else if(ctx->section_traversal.section->nodeset == PATTERN_NODESET_BUILD_REL)
  {
    v = &ctx->match->node->vertex;
    v = moria_vertex_up(v);
    fse = containerof(v, fsent, vertex);

    ctx->node = fsent_translate(fse, ctx->mod->anchor_dir, ctx->mod->build_dir, ctx->invalidation);
    pattern_section_generate(ctx);
  }
  else if(ctx->section_traversal.section->nodeset == PATTERN_NODESET_ANCHOR)
  {
    ctx->node = ctx->mod->anchor_dir;
    pattern_section_generate(ctx);
  }
  else if(ctx->section_traversal.section->nodeset == PATTERN_NODESET_BUILD)
  {
    ctx->node = ctx->mod->build_dir;
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
  else if(ctx->section_traversal.section->nodeset == PATTERN_NODESET_BASEDIR)
  {
    v = &g_base->vertex;
    ctx->node = containerof(v, fsent, vertex);
    RUNTIME_ASSERT(ctx->node);

    pattern_section_generate(ctx);
  }
  else
  {
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
  fsent *next_context_node;
  moria_vertex *next_context_vertex;
  const char *section;
  size_t section_len;
  struct segment_traversal *segment_traversal;
  const pattern_segment *segment;
  bool end;
  bool nofollow;
  vertex_kind kind;

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

  if(!ctx->node) {
    ctx->node = ctx->base;
  }

  /* generating onto //module is always redirected */
  if(ctx->node->vertex.attrs & MORIA_VERTEX_LINK) {
    if(ctx->node->vertex.ref == g_module) {
      ctx->node = ctx->mod->shadow;
    }
  }

  /* check type qualifiers for which kind of fsent to create */
  kind = VERTEX_FILE;
  nofollow = false;
  if(pattern_section_type_qualifiers_has(ctx->section_traversal.section, PATTERN_QUALIFIER_TYPE_DIR)) {
    kind = VERTEX_DIR;
  } else if(pattern_section_type_qualifiers_has(ctx->section_traversal.section, PATTERN_QUALIFIER_TYPE_SYMLINK)) {
    kind = VERTEX_SYMLINK;
    nofollow = true;
  }

  next_context_vertex = moria_vertex_downw(&ctx->node->vertex, section, section_len);
  if(next_context_vertex)
  {
    nofollow |= (ctx->flags & PATTERN_GENERATE_NOFOLLOW);

    if(!nofollow) {
      next_context_vertex = moria_vertex_follow(next_context_vertex);
    }
    if(vertex_type_get(next_context_vertex) == VERTEX_CONTEXT_MODULE) {
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
    fsent_create(&next_context_node, kind, VERTEX_UNCREATED, section, section_len);
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
    const generate_pattern * restrict pattern
  , module * restrict mod
  , bacon_value * restrict vars
  , bacon_parser * restrict parser
  , fsent * restrict base
  , graph_invalidation_context * invalidation
  , const pattern_search_hit * restrict match
  , uint32_t flags
  , void (*hit_visit)(void *ctx, const struct moria_vertex *hit)
  , void *hit_ctx
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
    , vars : vars
    , parser : parser
    , flags : flags
    , hit_visit : hit_visit
    , hit_ctx : hit_ctx

    /* outputs */
    //, hits : hits
  };

  ctx.section_traversal.head = &pattern->pattern.sections;
  ctx.section_narrator = narrator_fixed_init(&fixed, space, sizeof(space));
  llist_init(&ctx.segment_traversal_stack);

  pattern_section_generate(&ctx);
}

void pattern_attachment_generate(
    const attachment_pattern * restrict pattern
  , module * restrict mod
  , bacon_value * restrict vars
  , bacon_parser * restrict parser
  , fsent * restrict base
  , graph_invalidation_context * invalidation
  , const pattern_search_hit * restrict match
  , uint32_t flags
  , void (*hit_visit)(void *ctx, const struct moria_vertex *hit)
  , void *hit_ctx
)
  __attribute__((alias("pattern_generate")));

void pattern_include_generate(
    const include_pattern * restrict pattern
  , module * restrict mod
  , bacon_value * restrict vars
  , bacon_parser * restrict parser
  , fsent * restrict base
  , graph_invalidation_context * invalidation
  , const pattern_search_hit * restrict match
  , uint32_t flags
  , void (*hit_visit)(void *ctx, const struct moria_vertex *hit)
  , void *hit_ctx
)
  __attribute__((alias("pattern_generate")));
