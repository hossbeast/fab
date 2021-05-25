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

#include "common/hash.h"
#include "valyria/set.h"
#include "xlinux/xstdlib.h"
#include "moria/traverse.h"

#include "match.internal.h"
#include "pattern.internal.h"
#include "fsent.h"
#include "module.internal.h"

#include "narrator.h"

#include "section.h"

//
// static
//

/* match_node
 *
 * ctx     - dynamic context
 * quals   - whether to evalate section qualifiers, if present
 * matches - (returns) whether the initial section matches
 */
static xapi match_node(pattern_match_context * ctx, bool quals, bool * restrict matches)
{
  enter;

  struct match_section_traversal saved_section_traversal;
  struct match_segments_traversal traversal;

  saved_section_traversal = ctx->section_traversal;

  *matches = true;
  if(ctx->section_traversal.section->qualifiers_head && quals)
  {
    traversal = (typeof(traversal)) {
        segments_head : ctx->section_traversal.section->qualifiers_head
    };
    traversal.container.section = ctx->section_traversal.section;
    if(traversal.segments_head) {
      traversal.segments = chain_next(traversal.segments_head, &traversal.segments_cursor, chn);
    }

    ctx->traversal = &traversal;

    fatal(pattern_segments_match, ctx);

    *matches = traversal.u.match;
  }

  // match from next section, reverse order
  ctx->section_traversal.section = chain_prev(ctx->section_traversal.head, &ctx->section_traversal.cursor, chn);

  // proceed to the next section / parent
  if(*matches)
  {
    if(ctx->section_traversal.section)
    {
      if(ctx->dirnode)
      {
        fatal(pattern_section_match, ctx, ctx->dirnode);
      }
    }
    else
    {
      ctx->matched = true;
    }
  }

  ctx->section_traversal = saved_section_traversal;

  finally : coda;
}

static xapi match_visit(moria_vertex * restrict v, void * _ctx, moria_traversal_mode mode, int distance, int * restrict result)
{
  enter;

  pattern_match_context * ctx = _ctx;
  fsent * n;
  bool initial_match;

  /* matching path has been found */
  if(ctx->matched) {
    goto prune;
  }

  /* end of the line */
  n = containerof(v, fsent, vertex);
  if(n == g_root) {
    goto prune;
  }

  /* directories only */
  if(ctx->section_traversal.section->graph == PATTERN_GRAPH_DIRS && fsent_filetype_get(n) != VERTEX_FILETYPE_DIR) {
    goto prune;
  }

  ctx->label = n->vertex.label;
  ctx->label_len = n->vertex.label_len;
  ctx->dirnode = fsent_parent(n);
  fatal(match_node, ctx, distance == 0, &initial_match);

  /* for match patterns, discard non-matching paths */
  if(!initial_match) {
    goto prune;
  }

  goto XAPI_FINALLY;
prune:
  *result = MORIA_TRAVERSE_PRUNE;

  finally : coda;
}

//
// internal
//

xapi pattern_segments_match(pattern_match_context * ctx)
{
  enter;

  struct match_segments_traversal saved_traversal;
  struct match_segments_traversal *traversal;

  bool end;
  bool matches;
  bool next;
  bool abandon;

  uint16_t saved_offset = 0;
  uint16_t offset;

  traversal = ctx->traversal;
  saved_traversal = *traversal;

  while(1)
  {
    // next segment
    if(traversal->segments_head)
    {
      if(!traversal->segment_cursor) {
        traversal->segment_head = traversal->segments->segment_head;
      }

      if((traversal->segment = chain_next(traversal->segment_head, &traversal->segment_cursor, chn)))
      {
        offset = traversal->offset;
        fatal(traversal->segment->vtab->match, traversal->segment, ctx);

        // matched something
        if(offset != traversal->offset) {
          continue;
        }
      }
    }

    end = traversal->segment == NULL;
    matches = false;   // whether the node is matched by these segments
    next = true;       // whether to continue to the next segments
    abandon = false;   // whether to break out early

    if(traversal->segments_head)
    {
      matches = traversal->offset == ctx->label_len;
      next = end;

      if(traversal->container.segment && traversal->container.segment->type == PATTERN_ALTERNATION)
      {
        abandon = false;
        if(!traversal->container.segment->alternation.epsilon)
          abandon = !end && !chain_has_next(traversal->segments_head, traversal->segments_cursor, chn);
        next = !end;
      }
      else if(traversal->container.segment && traversal->container.segment->type == PATTERN_CLASS)
      {
        abandon = !end && !chain_has_next(traversal->segments_head, traversal->segments_cursor, chn);
        next = !end;
      }
      else  // section
      {
        abandon = !end;
        if(traversal->segments->qualifier_type == PATTERN_QUALIFIER_TYPE_NOT)
        {
          matches = !end;
          next = !end;
          abandon = end;
          traversal->offset = saved_offset;
        }
      }
    }
    else  // section with no qualifiers
    {
      matches = true;
    }

    if(abandon) {
      traversal->u.match = matches;
      break;
    }

    // next alternation, class, or qualifiers
    if(next && traversal->segments_head)
    {
      traversal->segment = 0;
      traversal->segment_head = 0;
      traversal->segment_cursor = 0;
      if((traversal->segments = chain_next(traversal->segments_head, &traversal->segments_cursor, chn)))
      {
        saved_offset = traversal->offset;
        traversal->offset = traversal->start;
        continue;
      }
    }

    // ascend
    if(ctx->traversal->u.prev && (ctx->traversal = ctx->traversal->u.prev))
    {
      ctx->traversal->offset = traversal->offset;
      memcpy(traversal, &saved_traversal, sizeof(*traversal) - sizeof(typeof(traversal->u)));
      traversal = ctx->traversal;
      saved_traversal = *traversal;

      continue;
    }

    traversal->u.match = matches;
    break;
  }

  memcpy(traversal, &saved_traversal, sizeof(*traversal) - sizeof(typeof(traversal->u)));

  finally : coda;
}

xapi pattern_section_match(pattern_match_context * restrict ctx, const fsent * restrict dirnode)
{
  enter;

  const pattern_section * section;
  uint16_t min_depth;
  uint16_t max_depth;
  moria_vertex_traversal_state *state = 0;

  section = ctx->section_traversal.section;

  min_depth = 0;
  max_depth = 0xffff;
  if(section->axis == PATTERN_AXIS_DOWN)
  {
    max_depth = 0;
  }
  else
  {
    RUNTIME_ASSERT(section->axis == PATTERN_AXIS_SELF_OR_BELOW);
  }

  fatal(moria_traverse_vertices
    , &g_graph
    , (void*)&dirnode->vertex
    , match_visit
    , state
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_TYPE_FSTREE
        , edge_visit : EDGE_TYPE_FSTREE
        , min_depth : min_depth
        , max_depth : max_depth
      }}
    , MORIA_TRAVERSE_UP | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
    , ctx
  );

finally:
  moria_vertex_traversal_end(&g_graph, state);
coda;
}

//
// public
//

xapi pattern_match(
    const pattern * restrict pattern
  , const struct fsent * restrict dirnode
  , const char * restrict label
  , uint16_t label_len
  , bool * restrict matched
)
{
  enter;

  pattern_match_context ctx = { };
  bool initial_match;

  // setup the dynamic context
  ctx.section_traversal.head = pattern->section_head;
  ctx.section_traversal.section = chain_prev(ctx.section_traversal.head, &ctx.section_traversal.cursor, chn);
  ctx.dirnode = dirnode;
  ctx.label = label;
  ctx.label_len = label_len;

  fatal(match_node, &ctx, true, &initial_match);

  *matched = ctx.matched;

  finally : coda;
}
