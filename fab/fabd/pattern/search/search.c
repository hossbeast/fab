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

#include "search.internal.h"
#include "pattern.internal.h"
#include "fsent.h"
#include "module.internal.h"
#include "shadow.h"

//
// static
//

static uint32_t search_node_hash(uint32_t h, const void * el, size_t sz)
{
  const pattern_search_node * m = el;

  return hash32(h, &m->node, sizeof(m->node));
}

static int search_node_cmp(const void * A, size_t Asz, const void *B, size_t Bsz)
{
  const pattern_search_node * mA = A;
  const pattern_search_node * mB = B;

  return INTCMP(mA->node, mB->node);
}

static void search_node_free(void * m)
{
  wfree(m);
}

static void search_visit(moria_vertex * restrict v, void * _ctx, moria_traversal_mode mode, int distance, int * restrict result)
{
  pattern_search_context * ctx = _ctx;
  fsent * n = containerof(v, fsent, vertex);
  pattern_search_node * m = 0;
  int x;

  struct search_section_traversal saved_section_traversal;
  uint16_t saved_variant_index;
  struct search_segments_traversal traversal;

  saved_section_traversal = ctx->section_traversal;
  saved_variant_index = ctx->variant_index;

  if(fsent_state_get(n) == VERTEX_UNLINKED)
  {
    *result = MORIA_TRAVERSE_PRUNE;
    goto restore;
  }

  if(ctx->section_traversal.section->graph == PATTERN_GRAPH_DIRS && fsent_filetype_get(n) != VERTEX_FILETYPE_DIR)
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

  ctx->node = n;
  //ctx->label = n->vertex.label;
  //ctx->label_len = n->vertex.label_len;
  ctx->matched = false;
  pattern_segments_search(ctx);

  if(!ctx->matched)
    goto restore;

  // match from next section
  ctx->section_traversal.section = chain_next(ctx->section_traversal.head, &ctx->section_traversal.cursor, chn);

  if(fsent_filetype_get(n) == VERTEX_FILETYPE_REG && !ctx->section_traversal.section)
  {
    xmalloc(&m, sizeof(*m));
    m->node = n;
    if(ctx->variant_index != -1) {
      m->variant = set_table_get(ctx->variants, ctx->variant_index);
    }
    memcpy(m->groups, ctx->groups, sizeof(m->groups));
    m->group_max = ctx->group_max;

    m->groups[0].start = ctx->node->name.name;
    m->groups[0].len = ctx->node->name.namel;
    for(x = 1; x <= ctx->group_max; x++) {
      m->groups[x] = ctx->groups[x];
    }

    set_put(ctx->matches, m, 0);
    m = 0;
  }
  else if(fsent_filetype_get(n) != VERTEX_FILETYPE_REG && ctx->section_traversal.section)
  {
    pattern_section_search(ctx, n);
  }

restore:
  ctx->section_traversal = saved_section_traversal;
  ctx->variant_index = saved_variant_index;

  wfree(m);
}

//
// internal
//

void pattern_segments_search(pattern_search_context * ctx)
{
  struct search_segments_traversal saved_traversal;
  struct search_segments_traversal *traversal;

  bool end;
  bool matches;
  bool next;
  bool abandon;

  uint16_t saved_offset = 0;
  uint16_t num;
  uint16_t offset;

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
        offset = traversal->offset;
        traversal->segment->vtab->search(traversal->segment, ctx);

        // matched something
        if(offset != traversal->offset || ctx->matched) {
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
      matches = traversal->offset == ctx->node->name.namel;
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
      num = traversal->container.segment->group.num;
      if(num < (sizeof(ctx->groups) / sizeof(*ctx->groups)))
      {
        ctx->groups[num].start = ctx->node->name.name + traversal->start;
        ctx->groups[num].len = traversal->offset - traversal->start;

        ctx->group_max = MAX(ctx->group_max, num);
      }
    }

    if(abandon) {
      break;
    }

    // next alternation, class, or qualifier
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
      memcpy(traversal, &saved_traversal, sizeof(*traversal) - sizeof(typeof(traversal->u)));
      traversal = ctx->traversal;
      saved_traversal = *traversal;

      continue;
    }

    if(matches) {
      ctx->matched = true;
    }

    break;
  }

  memcpy(traversal, &saved_traversal, sizeof(*traversal) - sizeof(typeof(traversal->u)));
}

void pattern_section_search(pattern_search_context * restrict ctx, fsent * restrict dirnode)
{
  const pattern_section * section;
  uint16_t min_depth = 0;
  uint16_t max_depth = UINT16_MAX;

  moria_vertex_traversal_state *state = 0;

  if(ctx->dirnode_visit)
  {
    ctx->dirnode_visit(ctx->dirnode_visit_ctx, dirnode);
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

  if(fsent_kind_get(dirnode) == VERTEX_SHADOW_MODULE)
  {
    dirnode = ctx->module->shadow;
    RUNTIME_ASSERT(dirnode);
  }

  moria_traverse_vertices(
      &g_graph
    , &dirnode->vertex
    , search_visit
    , state
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_TYPE_FSTREE
        , edge_visit : EDGE_TYPE_FSTREE
        , min_depth : min_depth
        , max_depth : max_depth
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH
    , ctx
  );

  moria_vertex_traversal_end(&g_graph, state);
}

//
// public
//

void pattern_search_matches_create(set ** matches)
{
  set_createx(matches, 0, search_node_hash, search_node_cmp, search_node_free);
}

void pattern_search(
    const pattern * restrict pattern
  , const module * restrict module
  , const llist * restrict modules
  , const set * restrict variants
  , set * restrict matches
  , void (*dirnode_visit)(void * ctx, struct fsent * dirnode)
  , void *dirnode_visit_ctx
)
{
  pattern_search_context ctx;

  if(matches) {
    set_recycle(matches);
  }

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
  };

  ctx.section_traversal.head = pattern->section_head;
  ctx.section_traversal.section = chain_next(ctx.section_traversal.head, &ctx.section_traversal.cursor, chn);
  ctx.segments_process = pattern_segments_search;

  /* search pattern begins at the module dirnode */
  pattern_section_search(&ctx, module->dir_node);
}
