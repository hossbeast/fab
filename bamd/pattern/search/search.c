/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "common/hash.h"
#include "valyria/hashtable.h"
#include "xlinux/xstdlib.h"
#include "moria/traverse.h"
#include "narrator.h"

#include "search.internal.h"
#include "pattern.internal.h"
#include "fsent.h"
#include "vertex.h"
#include "module.h"
#include "shadow.h"
#include "rule.h"
#include "section.h"
#include "zcat.h"

//
// static
//

static uint32_t search_node_hash(uint32_t h, const void * el)
{
  const pattern_search_node * m = el;

  return hash32(h, &m->node, sizeof(m->node));
}

static int search_node_cmp(const void * A, const void *B)
{
  const pattern_search_node * mA = A;
  const pattern_search_node * mB = B;

  return INTCMP(mA->node, mB->node);
}

static void matched(pattern_search_context * restrict ctx, fsent * restrict fse)
{
  pattern_search_node *m;
  int x;

  xmalloc(&m, sizeof(*m));
  m->node = fse;

  memcpy(m->groups, ctx->groups, sizeof(m->groups));
  m->group_max = ctx->group_max;

  m->groups[0].start = fse->name.name;
  m->groups[0].len = fse->name.namel;
  for(x = 1; x <= ctx->group_max; x++) {
    m->groups[x] = ctx->groups[x];
  }

  hashtable_put(ctx->matches, m);
}

static void search_visit(
    moria_vertex * restrict v
  , void * restrict _ctx
  , moria_traversal_mode mode
  , int distance
  , moria_traverse_result * restrict result
)
{
  pattern_search_context * restrict ctx = _ctx;
  fsent * n;
  bool section_has_type_qualifiers;

  struct search_section_traversal saved_section_traversal;
  struct search_segments_traversal traversal;

  saved_section_traversal = ctx->section_traversal;

  n = containerof(v, fsent, vertex);
  if(vertex_state_get(&n->vertex) == VERTEX_UNLINKED) {
    result->flags = MORIA_TRAVERSE_PRUNE;
    goto end;
  }

  /* starstar */
  if(ctx->section_traversal.section->graph == PATTERN_GRAPH_DIRS && vertex_filetype_get(&n->vertex) != VERTEX_FILETYPE_DIR) {
    result->flags = MORIA_TRAVERSE_PRUNE;
    goto end;
  }
  section_has_type_qualifiers = ctx->section_traversal.section->has_type_qualifiers;

  /* outer segments */
  traversal = (typeof(traversal)) {
    segments_head : &ctx->section_traversal.section->qualifiers
  };
  if(llist_empty(traversal.segments_head)) {
    traversal.segments = 0;
  } else {
    traversal.segments = llist_first(traversal.segments_head, typeof(*traversal.segments), lln);
    traversal.segment_head = &traversal.segments->list;
  }

  ctx->traversal = &traversal;
  ctx->node = n;
  ctx->matched = false;
  pattern_segments_search(ctx);
  if(!ctx->matched) {
    goto end;
  }

  /* follow links */
  while(v->attrs & MORIA_VERTEX_LINK) {
    v = v->ref;
  }
  /* follow context-dependent nodes */
  n = containerof(v, fsent, vertex);
  if(vertex_type_get(&n->vertex) == VERTEX_CONTEXT_MODULE) {
    v = &ctx->mod->shadow->vertex;
  }
  n = containerof(v, fsent, vertex);

  if((ctx->section_traversal.section = llist_next(ctx->section_traversal.head, ctx->section_traversal.section, lln)))
  {
    /* continue to the next section of the pattern */
    pattern_section_search(ctx, n);
  }
  else if(vertex_filetype_get(&n->vertex) == VERTEX_FILETYPE_DIR && !section_has_type_qualifiers)
  {
    /* to match a directory, it is necessary to specify a directory qualifier */
  }
  else
  {
    matched(ctx, n);
  }

end:
  ctx->section_traversal = saved_section_traversal;
}

//
// internal
//

/* check if these segments match the context node */
void pattern_segments_search(pattern_search_context * ctx)
{
  struct search_segments_traversal saved_traversal;
  struct search_segments_traversal *traversal;

  bool matched;
  bool invert;
  bool has_segments;

  traversal = ctx->traversal;
  saved_traversal = *traversal;

  has_segments = !!traversal->segments;
  invert = traversal->segments && traversal->segments->qualifier_type == PATTERN_QUALIFIER_TYPE_NOT;

  while(1)
  {
    /* bind inner segment list to outer segment list */
    if(!traversal->segment && traversal->segments) {
      traversal->segment_head = &traversal->segments->list;
    }

    if(traversal->segment_head && !traversal->segment) {
      traversal->segment = llist_first(traversal->segment_head, typeof(*traversal->segment), lln);
    } else if(traversal->segment_head) {
      traversal->segment = llist_next(traversal->segment_head, traversal->segment, lln);
    }

    if(traversal->segment) {
      traversal->segment->vtab->search(traversal->segment, ctx);

      if(traversal->segment->vtab->flags & PATTERN_SEGMENT_SEARCH_RECURSIVE) {
        /* this segments' match calls this function recursively */
        break;
      }

      if(!traversal->unsatisfied) {
        /* this segment did match */
        continue;
      }
    }

    /* whether the node is matched by this member of the inner segment list */
    matched = !traversal->unsatisfied;

    /* entire inner segment list was matched */
    matched &= traversal->segment == NULL;

    if(invert) {
      matched = !matched;
    }

    if(!matched) {
      break;
    }

    // end of group
    if(!traversal->sealed) {
      traversal->end = traversal->offset;
      traversal->sealed = true;
    }

    // outer segments list : qualifiers list
    if(traversal->segments_head && (traversal->segments = llist_next(traversal->segments_head, traversal->segments, lln))) {
      traversal->segment = 0;
      traversal->segment_head = 0;
      traversal->unsatisfied = 0;
      traversal->offset = traversal->start;

      invert = traversal->segments->qualifier_type == PATTERN_QUALIFIER_TYPE_NOT;
      has_segments = !!traversal->segments;
      continue;
    }

    // ascend : class, group, alternation matched
    if((ctx->traversal = ctx->traversal->u.prev))
    {
      ctx->traversal->offset = traversal->offset;
      ctx->traversal->offset = traversal->end;

      ctx->traversal->unsatisfied = 0;
      memcpy(traversal, &saved_traversal, sizeof(*traversal) - sizeof(typeof(traversal->u)));
      traversal = ctx->traversal;
      saved_traversal = *traversal;

      invert = traversal->segments && traversal->segments->qualifier_type == PATTERN_QUALIFIER_TYPE_NOT;
      has_segments = !!traversal->segments;
      continue;
    }

    /* entire label was matched */
    if(has_segments && !ctx->section_traversal.section->has_type_qualifiers) {
      matched &= (traversal->offset == ctx->node->name.namel);
    }
    if(invert) {
      matched = !matched;
    }

    ctx->matched = matched;
    break;
  }

  /* restore */
  memcpy(traversal, &saved_traversal, sizeof(*traversal) - sizeof(typeof(traversal->u)));
}

void pattern_section_search(pattern_search_context * restrict ctx, fsent * restrict fse)
{
  const pattern_section *section;
  uint16_t min_depth = 0;
  uint16_t max_depth = UINT16_MAX;
  const module *mod;

  mod = vertex_module_get(&fse->vertex);

  if(mod && mod != ctx->mod) {
    /* this directory is owned by some other module */
    return;
  }

  if(vertex_filetype_get(&fse->vertex) == VERTEX_FILETYPE_DIR) {
    if(ctx->dirnode_visit) {
      ctx->dirnode_visit(ctx->dirnode_visit_ctx, fse);
    }
  }

  section = ctx->section_traversal.section;

  if(section->axis == PATTERN_AXIS_SELF)
  {
    min_depth = 0;
    max_depth = 0;
  }
  else if(section->axis == PATTERN_AXIS_DOWN)
  {
    min_depth = 1;
    max_depth = 1;
  }
  else if(section->axis == PATTERN_AXIS_SELF_OR_BELOW)
  {
    min_depth = 0;
    max_depth = 0xffff;
  }

  moria_traverse_vertices(
      &g_graph
    , &fse->vertex
    , search_visit
    , 0
    , (moria_traversal_criteria[]) {{
          edge_travel : EDGE_KIND_FSTREE
        , edge_visit : EDGE_KIND_FSTREE
        , min_depth : min_depth
        , max_depth : max_depth
      }}
    , MORIA_TRAVERSE_DOWN | MORIA_TRAVERSE_PRE | MORIA_TRAVERSE_DEPTH | MORIA_TRAVERSE_NOFOLLOW
    , ctx
  );
}

//
// public
//

void pattern_search_matches_init(hashtable * matches)
{
  hashtable_initx(matches, 0, search_node_hash, search_node_cmp, free);
}

void pattern_search(
    const pattern * restrict pattern
  , const module * restrict mod
  , hashtable * restrict matches
  , void (*dirnode_visit)(void * ctx, struct fsent * dirnode)
  , void *dirnode_visit_ctx
)
{
  pattern_search_context ctx;
  fsent *base = 0;
  moria_vertex *v;

  // setup the dynamic context
  ctx = (typeof(ctx)) {
    /* inputs */
      mod : mod
    , dirnode_visit : dirnode_visit
    , dirnode_visit_ctx : dirnode_visit_ctx

    /* outputs */
    , matches : matches
  };

  ctx.section_traversal.head = &pattern->sections;
  ctx.section_traversal.section = llist_first(ctx.section_traversal.head, typeof(*ctx.section_traversal.section), lln);

  /* these can only appear as the initial section */
  if(ctx.section_traversal.section->nodeset) {
    switch(ctx.section_traversal.section->nodeset) {
      case PATTERN_NODESET_ROOT:
        base = g_root;
        break;
      case PATTERN_NODESET_SHADOW:
        base = g_shadow;
        break;
      case PATTERN_NODESET_ANCHOR:
        base = mod->anchor_node;
        break;
      case PATTERN_NODESET_MODULE:
        base = mod->dir_node;
        break;
      case PATTERN_NODESET_SELF:
        RTA;
    }
    if((ctx.section_traversal.section = llist_next(ctx.section_traversal.head, ctx.section_traversal.section, lln)) == 0) {
      matched(&ctx, base);
      return;
    }

    /* follow links */
    v = &base->vertex;
    while(v->attrs & MORIA_VERTEX_LINK) {
      v = v->ref;
    }
    /* follow context-dependent nodes */
    base = containerof(v, fsent, vertex);
    if(vertex_type_get(&base->vertex) == VERTEX_CONTEXT_MODULE) {
      v = &mod->shadow->vertex;
    }
    base = containerof(v, fsent, vertex);

    pattern_section_search(&ctx, base);
    return;
  }

  /* otherwise the default is to search from both the base and anchor nodes */
  pattern_section_search(&ctx, mod->dir_node);
  pattern_section_search(&ctx, mod->anchor_node);
}
