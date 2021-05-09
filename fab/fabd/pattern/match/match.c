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
#include "search.internal.h"
#include "pattern.internal.h"
#include "fsent.h"
#include "module.internal.h"

#include "narrator.h"

#include "section.h"

//
// static
//

static xapi match_visit(moria_vertex * restrict v, void * _ctx, moria_traversal_mode mode, int distance, int * restrict result)
{
  enter;

  pattern_search_context * ctx = _ctx;
  fsent * n;
  struct search_section_traversal saved_section_traversal;
  struct search_segments_traversal traversal;

  if(ctx->match) {
    *result = MORIA_TRAVERSE_PRUNE;
    goto XAPI_FINALLY;
  }

printf("visit %.*s\n", (int)v->label_len, v->label);

  saved_section_traversal = ctx->section_traversal;
  n = containerof(v, fsent, vertex);

  if(ctx->section_traversal.section->graph == PATTERN_GRAPH_DIRS && fsent_filetype_get(n) != VERTEX_FILETYPE_DIR)
  {
printf("%s:%d\n", __FUNCTION__, __LINE__);
    *result = MORIA_TRAVERSE_PRUNE;
    goto restore;
  }

  saved_section_traversal = ctx->section_traversal;

  if(ctx->section_traversal.section->qualifiers_head)
  {
    traversal = (typeof(traversal)) {
        segments_head : ctx->section_traversal.section->qualifiers_head
    };
    traversal.container.section = ctx->section_traversal.section;
    if(traversal.segments_head) {
      traversal.segments = chain_next(traversal.segments_head, &traversal.segments_cursor, chn);
    }

    ctx->traversal = &traversal;

    ctx->label = n->vertex.label;
    ctx->label_len = n->vertex.label_len;
    fatal(pattern_segments_match, ctx);

    //if(!ctx->matched) {
    //  goto restore;
    //}
    ctx->section_traversal.match = traversal.match;

    // match from next section, reverse order
    ctx->section_traversal.section = chain_prev(ctx->section_traversal.head, &ctx->section_traversal.cursor, chn);
  }
  else
  {
    ctx->section_traversal.match = true;
  }

  if(ctx->section_traversal.section)
  {
printf("%s:%d\n", __FUNCTION__, __LINE__);
    if(ctx->section_traversal.match && (n = fsent_parent(n)))
    {
      fatal(pattern_section_match, ctx, n);
    }
  }
  else
  {
    ctx->match = ctx->section_traversal.match;
  }

restore:
  ctx->section_traversal = saved_section_traversal;

  finally : coda;
}

//
// internal
//

xapi pattern_segments_match(pattern_search_context * ctx)
{
  enter;

  struct search_segments_traversal saved_traversal;
  struct search_segments_traversal *traversal;

  bool end;
  bool matches;
  bool next;
  bool abandon;

  uint16_t saved_offset = 0;
  uint16_t offset;

  traversal = ctx->traversal;
  saved_traversal = *traversal;

printf(" %s:%d segments -> ", __FUNCTION__, __LINE__);
fatal(pattern_segments_say, traversal->segments_head, true, g_narrator_stdout);
printf(" vs %.*s\n", (int)ctx->label_len, ctx->label);
  while(1) // !ctx->matched)
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
        fatal(traversal->segment->vtab->search, traversal->segment, ctx);

printf("%s:%d len %d\n", __FUNCTION__, __LINE__, traversal->offset - offset);

        // matched something
        if(offset != traversal->offset) { // || ctx->matched) {
          continue;
        }
      }
    }
printf("%s:%d %p\n", __FUNCTION__, __LINE__, traversal->segment);

    end = traversal->segment == NULL;
    matches = false;   // whether the node is matched by these segments
    next = true;       // whether to continue to the next segments
    abandon = false;   // whether to break out early

    if(traversal->segments_head)
    {
      matches = traversal->offset == ctx->label_len; // ctx->node->name.namel;
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
printf("%s:%d break\n", __FUNCTION__, __LINE__);
      //ctx->matched = matches;
      traversal->match = matches;
      break;
    }

    // next alternation, class, or qualifiers
    if(next && traversal->segments_head)
    {
printf("%s:%d\n", __FUNCTION__, __LINE__);
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
printf("%s:%d ASCEND\n", __FUNCTION__, __LINE__);
      ctx->traversal->offset = traversal->offset;
      memcpy(traversal, &saved_traversal, sizeof(*traversal) - sizeof(struct unrestored));
      traversal = ctx->traversal;
      saved_traversal = *traversal;

      continue;
    }

//    if(!matches) {
//printf("%s:%d NOMATCH\n", __FUNCTION__, __LINE__);
//      ctx->matched = false;
//    }
    traversal->match = matches;
    break;
  }

printf("%s:%d restore\n", __FUNCTION__, __LINE__);
  memcpy(traversal, &saved_traversal, sizeof(*traversal) - sizeof(struct unrestored));

  finally : coda;
}

xapi pattern_section_match(pattern_search_context * restrict ctx, const fsent * restrict dirnode)
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
    min_depth = 0;
    max_depth = 0;
  }
  else
  {
    RUNTIME_ASSERT(section->axis == PATTERN_AXIS_SELF_OR_BELOW);
  }

printf("min %hu max %hu %.*s\n", min_depth, max_depth, (int)dirnode->vertex.label_len, dirnode->vertex.label);

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
  , const struct fsent * restrict n
  , const char * restrict label
  , uint16_t label_len
  , bool * restrict matched
)
{
  enter;

  pattern_search_context ctx = { };
  struct search_segments_traversal traversal;

  // setup the dynamic context
  ctx.section_traversal.head = pattern->section_head;
  ctx.section_traversal.section = chain_prev(ctx.section_traversal.head, &ctx.section_traversal.cursor, chn);
  ctx.segments_process = pattern_segments_match;
  //ctx.matched = true;

fatal(pattern_say, pattern, g_narrator_stdout);
printf("\n");

printf("section list\n");
const chain *T;
pattern_section *section;
chain_foreach(T, section, chn, pattern->section_head) {
  printf(" %p ", section);
  fatal(pattern_section_say, section, true, g_narrator_stdout);
  printf("\n");
}
printf("\n");

char space[512];
fsent_absolute_path_znload(space, sizeof(space), n);
printf("initial node %s // %.*s\n", space, (int)label_len, label);

  /* the initial section matches against the initial label */
  if(ctx.section_traversal.section->qualifiers_head)
  {
    traversal = (typeof(traversal)) {
        segments_head : ctx.section_traversal.section->qualifiers_head
    };
    traversal.container.section = ctx.section_traversal.section;
    traversal.segments = chain_next(traversal.segments_head, &traversal.segments_cursor, chn);

    ctx.traversal = &traversal;
    ctx.dirnode = n;
    ctx.label = label;
    ctx.label_len = label_len;

    fatal(pattern_segments_match, &ctx);

printf("initial segment match %d\n", traversal.match);

    //if(!ctx.matched) {
    //  goto XAPI_FINALLY;
    //}
    ctx.section_traversal.match = traversal.match;

    // match from next section, reverse order
    ctx.section_traversal.section = chain_prev(ctx.section_traversal.head, &ctx.section_traversal.cursor, chn);
  }
  else
  {
    ctx.section_traversal.match = true;
  }

  if(ctx.section_traversal.section)
  {
    if(ctx.section_traversal.match)
    {
      /* proceed from the parent */
      fatal(pattern_section_match, &ctx, n);
printf("%s:%d match %d\n", __FUNCTION__, __LINE__, ctx.match);
    }
  }
  else
  {
    ctx.match = ctx.section_traversal.match;
printf("no more sections?\n");
  }

finally:
  *matched = ctx.section_traversal.match;
coda;
}
