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
#include "narrator/record.h"
#include "moria/vertex.h"
#include "valyria/llist.h"
#include "valyria/set.h"
#include "valyria/dictionary.h"
#include "xlinux/xstdlib.h"

#include "pattern.internal.h"
#include "section.internal.h"
#include "match.internal.h"
#include "module.internal.h"
#include "node.h"
#include "shadow.h"
#include "path.h"

#include "attrs.h"
#include "hash.h"

//
// static
//

static uint32_t match_hash(uint32_t h, const void * el, size_t sz)
{
  const pattern_match_node * m = el;

  return hash32(h, &m->node, sizeof(m->node));
}

static int match_cmp(const void * A, size_t Asz, const void *B, size_t Bsz)
{
  const pattern_match_node * mA = A;
  const pattern_match_node * mB = B;

  return INTCMP(mA->node, mB->node);
}

static void match_free(void * m)
{
  wfree(m);
}

static xapi match_visit(vertex * restrict v, void * _ctx, traversal_mode mode, int distance, int * restrict result)
{
  enter;

  pattern_match_context * ctx = _ctx;
  node * node = vertex_value(v);
  pattern_match_node * m = 0;
  int x;

  struct match_section_traversal saved_section_traversal;
  uint16_t saved_variant_index;
  struct match_segments_traversal traversal;

  saved_section_traversal = ctx->section_traversal;
  saved_variant_index = ctx->variant_index;

  if(node_state_get(node) == VERTEX_UNLINKED)
  {
    *result = MORIA_TRAVERSE_PRUNE;
    goto restore;
  }

  if(ctx->section_traversal.section->graph == PATTERN_GRAPH_DIRS && node_filetype_get(node) != VERTEX_FILETYPE_DIR)
  {
    *result = MORIA_TRAVERSE_PRUNE;
    goto restore;
  }

  saved_section_traversal = ctx->section_traversal;

  traversal = (typeof(traversal)) {
      segments_head : ctx->section_traversal.section->qualifiers_head
  };
  traversal.container.section = ctx->section_traversal.section;
  if(traversal.segments_head)
    traversal.segments = chain_next(traversal.segments_head, &traversal.segments_cursor, chn);

  ctx->traversal = &traversal;

  ctx->node = node;
  ctx->matched = false;
  fatal(pattern_segments_match, ctx);

  if(!ctx->matched)
    goto restore;

  // match from next section
  ctx->section_traversal.section = chain_next(ctx->section_traversal.head, &ctx->section_traversal.cursor, chn);

  if(node_filetype_get(node) == VERTEX_FILETYPE_REG && !ctx->section_traversal.section)
  {
    if(ctx->matches)
    {

      fatal(xmalloc, &m, sizeof(*m));
      m->node = node;
      if(ctx->variant_index != -1) {
        m->variant = set_table_get(ctx->variants, ctx->variant_index);
      }
      memcpy(m->groups, ctx->groups, sizeof(m->groups));
      m->group_max = ctx->group_max;

      m->groups[0].start = ctx->node->name->name;
      m->groups[0].len = ctx->node->name->namel;
      for(x = 1; x <= ctx->group_max; x++)
        m->groups[x] = ctx->groups[x];

      fatal(set_put, ctx->matches, m, 0);
      m = 0;
    }
    else if(ctx->matched_nodes)
    {
      fatal(set_put, ctx->matched_nodes, node, 0);
    }
  }
  else if(node_filetype_get(node) != VERTEX_FILETYPE_REG && ctx->section_traversal.section)
  {
    fatal(pattern_section_match, ctx, node);
  }

restore:
  ctx->section_traversal = saved_section_traversal;
  ctx->variant_index = saved_variant_index;

finally:
  wfree(m);
coda;
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

  traversal = ctx->traversal;
  saved_traversal = *traversal;

  while(!ctx->matched)
  {
    // next segment
    if(traversal->segments_head)
    {
      if(!traversal->segment_cursor)
        traversal->segment_head = traversal->segments->segment_head;

      if((traversal->segment = chain_next(traversal->segment_head, &traversal->segment_cursor, chn)))
      {
        uint16_t offset = traversal->offset;

        fatal(traversal->segment->vtab->match, ctx, traversal->segment);

        // matched something
        if(offset != traversal->offset || ctx->matched)
          continue;
      }
    }

    end = traversal->segment == NULL;
    matches = false;   // whether the node is matched by these segments
    next = true;       // whether to continue to the next segments
    abandon = false;   // whether to break out early

    if(traversal->segments_head)
    {
      matches = traversal->offset == ctx->node->name->namel;
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

      else  // group, section
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

    // end of group
    if(traversal->container.segment && traversal->container.segment->type == PATTERN_GROUP)
    {
      uint16_t num = traversal->container.segment->group.num;

      if(num < (sizeof(ctx->groups) / sizeof(*ctx->groups)))
      {
        ctx->groups[num].start = ctx->node->name->name + traversal->start;
        ctx->groups[num].len = traversal->offset - traversal->start;

        ctx->group_max = MAX(ctx->group_max, num);
      }
    }

    if(abandon)
      break;

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
    if((ctx->traversal = ctx->traversal->u.prev))
    {
      ctx->traversal->offset = traversal->offset;
      memcpy(traversal, &saved_traversal, sizeof(*traversal) - sizeof(struct unrestored));
      traversal = ctx->traversal;
      saved_traversal = *traversal;

      continue;
    }

    if(matches)
      ctx->matched = true;

    break;
  }

  memcpy(traversal, &saved_traversal, sizeof(*traversal) - sizeof(struct unrestored));

  finally : coda;
}

xapi pattern_section_match(pattern_match_context * restrict ctx, node * restrict dirnode)
{
  enter;

  uint32_t relation = 0;
  const pattern_section * section;
  uint16_t min_depth = 0;
  uint16_t max_depth = UINT16_MAX;

  vertex_traversal_state *state = 0;

  if(ctx->dirnode_visit)
  {
    fatal(ctx->dirnode_visit, ctx->dirnode_visit_ctx, dirnode);
  }

  section = ctx->section_traversal.section;

  if(section->nodeset == PATTERN_NODESET_SHADOW)
  {
    dirnode = g_shadow;
  }

  if(section->axis == PATTERN_AXIS_DOWN)
  {
    min_depth = 1;
    max_depth = 1;
  }
  else if(section->axis == PATTERN_AXIS_SELF_OR_BELOW)
  {
    min_depth = 0;
    max_depth = 0xffff;
  }

  relation = EDGE_TYPE_FS;

  if(node_kind_get(dirnode) == VERTEX_SHADOW_MODULE)
  {
    dirnode = ctx->module->shadow;
    RUNTIME_ASSERT(dirnode);
  }

  fatal(graph_traverse_vertices
    , g_graph
    , vertex_containerof(dirnode)
    , match_visit
    , state // 0
    , (traversal_criteria[]) {{
          edge_travel : relation
        , edge_visit : relation
        , min_depth : min_depth
        , max_depth : max_depth
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
    , ctx
  );

finally:
  graph_vertex_traversal_end(g_graph, state);
coda;
}

//
// public
//

xapi pattern_match_matches_create(set ** matches)
{
  enter;

  fatal(set_createx, matches, 0, match_hash, match_cmp, match_free, 0);

  finally : coda;
}

xapi pattern_match(
    const pattern * restrict pattern
  , const module * restrict module
  , const llist * restrict modules
  , const set * restrict variants
  , set * restrict matches
  , set * restrict matched_nodes
  , xapi (*dirnode_visit)(void * ctx, struct node * dirnode)
  , void *dirnode_visit_ctx
)
{
  enter;

  pattern_match_context ctx = { 0 };

  if(matches)
    fatal(set_recycle, matches);

  // setup the dynamic context
  ctx = (typeof(ctx)) {
    /* inputs */
      variants : variants
    , module : module
    , modules : modules
    , dirnode_visit : dirnode_visit
    , dirnode_visit_ctx : dirnode_visit_ctx

    /* state */
    , variant_index : -1

    /* outputs */
    , matches : matches
    , matched_nodes : matched_nodes
  };

  ctx.section_traversal.head = pattern->section_head;
  ctx.section_traversal.section = chain_next(ctx.section_traversal.head, &ctx.section_traversal.cursor, chn);

  /* match patterns begin at the module fs */
  fatal(pattern_section_match, &ctx, module->dir_node);

  finally : coda;
}
