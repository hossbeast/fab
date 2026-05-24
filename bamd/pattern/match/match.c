/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "moria/foreach.h"
#include "bacon/value.h"

#include "match.internal.h"
#include "fsent.h"
#include "vertex.h"
#include "module.h"



#include "search.internal.h"


//
// static
//

/* match_node
 *
 * ctx     - dynamic context
 * matches - (returns) whether the initial section matches
 */
static void match_node(pattern_search_context * restrict ctx)
{
  const struct pattern_section *sec;
  struct search_section_traversal saved_section_traversal;
  struct search_segments_traversal traversal;
  bool match;

  saved_section_traversal = ctx->section_traversal;

  match = true;
  if(ctx->section_traversal.section->nodeset == PATTERN_NODESET_ROOT)
  {
    if(ctx->node != g_root)
    {
      match = false;
    }
  }
  else if(!llist_empty(&ctx->section_traversal.section->qualifiers))
  {
    traversal = (typeof(traversal)) {
      segments_head : &ctx->section_traversal.section->qualifiers
    };
    traversal.container.section = ctx->section_traversal.section;
    traversal.segments = llist_first(traversal.segments_head, typeof(*traversal.segments), lln);
    traversal.segment_head = &traversal.segments->list;

    ctx->traversal = &traversal;
    ctx->matched = false;
    pattern_segments_search(ctx);
    match = ctx->matched;
    ctx->matched = false;
  }

  // match from next section, reverse order
  sec = ctx->section_traversal.section;
  ctx->section_traversal.section = llist_prev(ctx->section_traversal.head, ctx->section_traversal.section, lln);

  // proceed to the next section / parent
  if(match)
  {
    if(ctx->section_traversal.section && ctx->section_traversal.section->nodeset == PATTERN_NODESET_ROOT && ctx->node == g_root)
    {
      ctx->matched = true;
    }
    else if(ctx->section_traversal.section)
    {
      pattern_section_match(ctx, sec, ctx->node);
    }
    else
    {
      ctx->matched = true;
    }
  }

  ctx->section_traversal = saved_section_traversal;
}

static void match_visit(moria_vertex * restrict v, void * _ctx, moria_traversal_mode mode, int distance, moria_traverse_result * restrict result)
{
  pattern_search_context * ctx = _ctx;
  fsent * n;
  moria_foreach_state st;
  moria_edge *edge;

  /* matching path has been found */
  if(ctx->matched) {
    result->flags = MORIA_TRAVERSE_PRUNE;
    return;
  }

  /* end of the line */
  n = containerof(v, fsent, vertex);

  /* directories only */
  if(ctx->section_traversal.section->graph == PATTERN_GRAPH_DIRS && vertex_filetype_get(&n->vertex) != VERTEX_FILETYPE_DIR) {
    result->flags = MORIA_TRAVERSE_PRUNE;
    return;
  }

  /* check for match here */
  ctx->node = n;
  match_node(ctx);

  /* check for match from any symlinks to this node */
  moria_foreach_edge_up(&st, &n->vertex, edge) {
    if(edge->attrs != EDGE_REF_SYMLINK) {
      continue;
    }

    if(ctx->matched) {
      break;
    }
    ctx->node = containerof(edge->A, fsent, vertex);
    match_node(ctx);
  }
}

//
// internal
//

void pattern_section_match(pattern_search_context * restrict ctx, const struct pattern_section * restrict prev, const fsent * restrict dirnode)
{
  uint16_t min_depth;
  uint16_t max_depth;

  min_depth = 0;
  max_depth = 0;
  if(prev->axis == PATTERN_AXIS_DOWN)
  {
    min_depth = 1;
    max_depth = 1;
  }
  else if(prev->axis == PATTERN_AXIS_SELF_OR_BELOW)
  {
    min_depth = 0;
    max_depth = 0xffff;
  }

  moria_traverse_vertices(
      &g_graph
    , (void*)&dirnode->vertex
    , match_visit
    , 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_KIND_FS
        , edge_visit : EDGE_KIND_FS
        , min_depth : min_depth
        , max_depth : max_depth
      }}
    , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH | MORIA_TRAVERSE_NOFOLLOW
    , ctx
  );
}

//
// public
//

bool pattern_match_sections(
    const struct llist * restrict head
  , const struct pattern_section * restrict section
  , const bacon_value * restrict vars
  , const struct fsent * restrict node
)
{
  pattern_search_context ctx = { };

  // setup the dynamic context
  ctx.vars = vars;
  ctx.section_traversal.head = head;
  ctx.section_traversal.section = section;
  ctx.node = node;

  match_node(&ctx);

  return ctx.matched;
}

bool pattern_match(
    const match_pattern * restrict pattern
  , const bacon_value * restrict vars
  , const struct fsent * restrict node)
{
  const struct llist *head;
  const struct pattern_section *section;

  head = &pattern->pattern.sections;
  section = llist_last(head, typeof(*section), lln);

  return pattern_match_sections(head, section, vars, node);
}

bool pattern_match_section(
    const struct pattern_section * restrict section
  , const bacon_value * restrict vars
  , const struct fsent * restrict node
)
{
  pattern_search_context ctx = { };

  // setup the dynamic context
  ctx.vars = vars;
  ctx.section_traversal.section = section;
  ctx.node = node;

  pattern_section_match(&ctx, section, node);

  return ctx.matched;
}
