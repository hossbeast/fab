/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "types.h"
#include "bacon/value.h"
#include "bacon/set.h"
#include "bacon/builder.h"

#include "moria/graph.h"
#include "moria/vertex.h"
#include "moria/lookup.h"
#include "xlinux/xstdlib.h"
#include "narrator.h"
#include "narrator/fixed.h"

#include "lookup.h"
#include "fsent.h"
#include "vertex.h"
#include "pattern.h"
#include "render.h"
#include "selection.h"
#include "channel.h"
#include "events.h"
#include "system_state.h"
#include "pattern/section.h"
#include "shadow.h"
#include "qualifier.h"
#include "index_vertex.h"
#include "entlist.h"

typedef struct lookup_context {
  const char * path;
  uint16_t path_len;

  const char *segment;

  entlist *index_hits;
} lookup_context;

static void __attribute__((nonnull(1))) lookup_callback(
    void * restrict _ctx
  , const char ** restrict label
  , uint16_t * restrict label_len
)
{
  lookup_context *ctx = _ctx;
  uint16_t seg_len;

  if(label && !ctx->segment)
  {
    *label = 0;
  }
  else if(label)
  {
    seg_len = 0;
    if(*ctx->segment == '/')
    {
      ctx->segment--;
      seg_len++;
    }
    while(ctx->segment != ctx->path && *ctx->segment != '/')
    {
      ctx->segment--;
      seg_len++;
    }

    if(ctx->segment == ctx->path)
    {
      *label = ctx->segment;
      *label_len = seg_len;
      ctx->segment = 0;
    }
    else
    {
      *label = ctx->segment + 1;
      *label_len = seg_len - 1;
    }
  }

  // rewind
  else
  {
    ctx->segment = ctx->path + ctx->path_len;
  }
}

static void index_lookup_callback(void *_ctx, const char * restrict label, uint16_t label_len, moria_vertex * restrict v)
{
  entlist *hits;
  index_vertex *index;
  lookup_context *ctx;

  ctx = _ctx;
  if(!(hits = ctx->index_hits)) {
    return;
  }

  if(v) {
    index = containerof(v, typeof(*index), vertex); /* hit */
  } else {
    index = index_vertex_create(label, label_len);  /* miss */
  }
  entlist_push(hits, index);
}

static void fragment_lookup_error(
    const pattern * restrict ref
  , const fsent * restrict base
  , int r
  , const moria_vertex ** restrict vertices
)
{
  narrator_fixed fixed;
  bacon_writer writer, *builder_writer;
  channel * restrict chan;
  fabipc_message *msg;

  msg = system_error_event(&chan);
  narrator_fixed_init(&fixed, msg->text, sizeof(msg->text));
  bacon_writer_init(&writer);
  builder_writer = bacon_writer_open(&writer, &fixed.base);

  { MAPPING
    if(r == 0) {
      STRING("unresolved ref");
    } else {
      STRING("ambiguous ref");
    }
    { SET
      { MAPPING
        STRING("location")
        LOADF("%s [%d,%d - %d,%d]", ref->loc.fname, ref->loc.f_lin + 1, ref->loc.f_col + 1, ref->loc.l_lin + 1, ref->loc.l_col + 1);
      }
      { MAPPING
        STRING("lookup")
        bacon_writer_push_string(builder_writer);
        pattern_say(ref, builder_writer->N);
        bacon_writer_pop_string(builder_writer);
      }
      if(base) {
        MAPPING
        STRING("base")
        fsent_absolute_path_describe(base, builder_writer);
      }
      if(r > 1) {
        { MAPPING
          STRING("one")
          fsent_absolute_path_describe(containerof(vertices[0], fsent, vertex), builder_writer);
        }
        { MAPPING
          STRING("two")
          fsent_absolute_path_describe(containerof(vertices[1], fsent, vertex), builder_writer);
        }
      }
    }
  }
  bacon_writer_close(&writer);

  msg->code = EINVAL;
  msg->size = fixed.l;
  events_publish(chan, msg);
}

void pattern_lookup_relative(
    const pattern * restrict ref
  , const bacon_value * restrict vars
  , const fsent *base
  , entlist * restrict hits
)
{
  const pattern_section *section;
  const fsent *fse;
  const moria_vertex *v;
  char frag[128];
  uint16_t frag_len;

  fse = base;
  v = &fse->vertex;

  /* left-to-right */
  llist_foreach(&ref->sections, section, lln) {
#define CASE3(x, y, z) ((x << 16) | (y << 8) | z)
    switch(CASE3(section->nodeset, section->graph, section->axis)) {
      case CASE3(0, 0, PATTERN_AXIS_SELF):
      case CASE3(PATTERN_NODESET_SELF, 0, 0):
        break;
      case CASE3(PATTERN_NODESET_ROOT, 0, 0):
        v = &g_root->vertex;
        break;
      case CASE3(PATTERN_NODESET_SHADOW, 0, 0):
        v = &g_shadow->vertex;
        break;
      case CASE3(0, PATTERN_GRAPH_FS, PATTERN_AXIS_UP):
        fse = containerof(v, typeof(*fse), vertex);
        fse = fsent_parent(fse);
        v = &fse->vertex;
        break;
      case CASE3(0, PATTERN_GRAPH_FS, PATTERN_AXIS_DOWN):
        frag_len = pattern_section_cat(frag, sizeof(frag), vars, section);
        v = moria_vertex_downw(v, frag, frag_len);
        break;
      default:
        RTA;
    }
  }

  if(v) {
    entlist_push(hits, v);
  }
}

static bool type_qualifiers_evaluate(const pattern_segments * restrict segments, const moria_vertex * restrict v)
{
  bool match;
  const pattern_group *group;
  const pattern_qualifier *qualifier;
  const pattern_segments *group_segments;

  group = llist_first(&segments->list, typeof(*group), lln);
  RUNTIME_ASSERT(group->type == PATTERN_GROUP);
  RUNTIME_ASSERT(group->has_type_qualifiers);

  RUNTIME_ASSERT(llist_count(&group->segments) == 1);
  group_segments = llist_first(&group->segments, typeof(*group_segments), lln);

  llist_foreach(&group_segments->list, qualifier, lln) {
    RUNTIME_ASSERT(qualifier->type == PATTERN_QUALIFIER);
    match = pattern_qualifier_evaluate(qualifier->qualifier_type, v);
    if(segments->qualifier_type == PATTERN_QUALIFIER_TYPE_AND && !match) {
      return false;
    }
    if(segments->qualifier_type == PATTERN_QUALIFIER_TYPE_NOT && match) {
      return false;
    }
  }

  return true;
}

void pattern_lookup_indexed(
    const pattern * restrict ref
  , const bacon_value * restrict vars
  , entlist * restrict index_hits
  , entlist * restrict hits
)
{
  pattern_section *section;
  pattern_segments *segments = 0;
  pattern_render_result *last_rendered;
  pattern_render_fragment *last_frag;
  const moria_vertex *last_v;
  pattern_render_result *rendered;
  pattern_render_fragment *frag;
  lookup_context ctx = { 0 };
  const moria_vertex *vertices[2];
  int r;
  int x;
  int y;
  int i;
  uint32_t flags;

  section = llist_first(&ref->sections, typeof(*section), lln);
  if(section->nodeset || section->axis != PATTERN_AXIS_DOWN) {
    RTA;
  }

  flags = 0;
  section = llist_last(&ref->sections, typeof(*section), lln);
  if(section->has_type_qualifiers) {
    segments = llist_last(&section->qualifiers, typeof(*segments), lln);
    flags |= PATTERN_RENDER_LAST_SECTION_SKIP;
  }

  /* render the reference pattern to resolve classes, alternations, variables */
  /* this is limited to one single fragment though */
  pattern_render(ref, vars, &rendered, flags);

  ctx.index_hits = index_hits;

  last_rendered = 0;
  frag = rendered->fragments;
  for(x = 0; x < rendered->size; x++)
  {
    ctx.path = frag->text;
    ctx.path_len = frag->len;

    /* indexed lookup on the final section, then prefix traversal leftwards */
    moria_graph_lookup(&g_graph, &g_index->vertex, lookup_callback, index_lookup_callback, &ctx, vertices, &r);
    if(r != 1) {
      goto next;
    }
    if(!section->has_type_qualifiers) {
      goto next;
    }
    last_v = vertices[0];

    /* evaluate last-section-has-type-qualifiers patterns */
    r = 0;
    free(last_rendered);
    pattern_render(ref, vars, &last_rendered, PATTERN_RENDER_LAST_SECTION_ONLY);
    last_frag = last_rendered->fragments;
    for(y = 0; y < last_rendered->size; y++)
    {
      RUNTIME_ASSERT(last_frag->len > 1);
      RUNTIME_ASSERT(last_frag->text[0] == '/');

      if(!(vertices[r] = moria_vertex_downw(last_v, last_frag->text + 1, last_frag->len - 1))) {
        continue;
      }
      if(type_qualifiers_evaluate(segments, vertices[r])) {
        r++;
      }
    }

  next:
    frag = pattern_render_fragment_next(frag);

    for(i = 0; i < r; i++) {
      entlist_push(hits, vertices[i]);
    }

    if(r == 1) {
      continue;
    }

    fragment_lookup_error(ref, 0, r, vertices);
  }

  free(rendered);
  free(last_rendered);
}

void pattern_lookup(
    const pattern * restrict ref
  , const bacon_value * restrict vars
  , const fsent * restrict base
  , entlist * restrict hits
)
{
  pattern_section *section;

  entlist_destroy(hits);
  section = llist_first(&ref->sections, typeof(*section), lln);
  if(section->nodeset || section->axis != PATTERN_AXIS_DOWN)
  {
    pattern_lookup_relative(ref, vars, base, hits);
  }
  else
  {
    pattern_lookup_indexed(ref, vars, 0, hits);
  }
}

void pattern_resolve_lookup(
    pattern * restrict ref
  , struct bacon_value * restrict vars
  , const fsent * restrict base
  , entlist * restrict scratch
  , fsent ** restrict dirp
)
{
  entlist_destroy(scratch);
  pattern_lookup(ref, vars, base, scratch);

  switch(llist_count(&scratch->lln)) {
    case 0:
      *dirp = 0;
      return;
    case 1:
      break;
    default:
      RTA;
  }

  *dirp = entlist_first(scratch)->p;
}
