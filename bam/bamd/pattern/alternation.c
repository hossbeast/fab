/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"
#include "narrator.h"
#include "common/attrs.h"
#include "bacon/builder.h"

#include "alternation.h"
#include "fsent.h"
#include "generate.internal.h"
#include "search.internal.h"
#include "match.internal.h"
#include "pattern.internal.h"
#include "render.internal.h"
#include "segment.h"
#include "segments.h"

//
// static
//

static void alternation_describe(const pattern_segment * restrict fn, bacon_writer * restrict builder_writer)
{
  const pattern_alternation *alternation = &fn->alternation;

  { MAPPING
    STRING("epsilon")
    BOOL(alternation->epsilon)
  }
  if(!llist_empty(&alternation->segments)) {
    MAPPING
    STRING("segments")
    pattern_segments_list_describe(&alternation->segments, builder_writer);
  }
}

static void alternation_say(const pattern_segment * restrict fn, const pattern_segment * restrict next, narrator * restrict N)
{
  const pattern_alternation * n = &fn->alternation;
  pattern_segments * segments;
  int x;

  xsayc('{');

  x = 0;
  llist_foreach(&n->segments, segments, lln) {
    if(x++) {
      xsays(",");
    }

    pattern_segment_list_say(&segments->list, N);
  }

  if(n->epsilon) {
    xsayc(',');
  }

  xsayc('}');
}

static void alternation_destroy(pattern_segment * restrict n)
{
  pattern_segments * segments;
  llist *T;

  pattern_alternation * alternation = &n->alternation;

  llist_foreach_safe(&alternation->segments, segments, lln, T) {
    pattern_segments_free(segments);
  }
}

static void alternation_search(const pattern_segment * restrict segment, pattern_search_context * restrict ctx)
{
  const pattern_alternation *alternation;
  struct search_segments_traversal traversal;

  alternation = &segment->alternation;

  memset(&traversal, 0, sizeof(traversal));
  traversal.u.prev = ctx->traversal;
  traversal.start = traversal.u.prev->offset;
  traversal.offset = traversal.u.prev->offset;

  if(alternation->epsilon) {
    ctx->traversal = &traversal;
    pattern_segments_search(ctx);
  }

  /* inner segments : the alternations */
  if(!(traversal.segments = llist_first(&alternation->segments, typeof(*traversal.segments), lln))) {
    goto end;
  }

  while(!ctx->matched)
  {
    traversal.segment_head = &traversal.segments->list;
    traversal.segment = 0;
//    traversal.segment = llist_first(&traversal.segment_head, typeof(*traversal.segment), lln);//segments->segment_head;
    traversal.unsatisfied = false;
    traversal.sealed = false;

    ctx->traversal = &traversal;
    pattern_segments_search(ctx);

    if(!(traversal.segments = llist_next(&alternation->segments, traversal.segments, lln))) {
      break;
    }
  }

end:
  // pop
  ctx->traversal = traversal.u.prev;
}

static void alternation_generate(const pattern_segment * pat, pattern_generate_context * restrict ctx)
{
  const pattern_alternation * alternation = &pat->alternation;

  const pattern_segments *alt_segments;
  fsent *saved_context_node;
  char saved_text[256];
  off_t saved_section_narrator_pos;

  // section traversal
  struct section_traversal saved_section_traversal;

  // segment traversal
  struct segment_traversal alt_segment_traversal;

  saved_context_node = ctx->node;
  saved_section_traversal = ctx->section_traversal;
  narrator_seek(ctx->section_narrator, 0, NARRATOR_SEEK_CUR, &saved_section_narrator_pos);
  narrator_seek(ctx->section_narrator, 0, NARRATOR_SEEK_SET, 0);
  narrator_read(ctx->section_narrator, saved_text, saved_section_narrator_pos, 0);

  llist_append(&ctx->segment_traversal_stack, &alt_segment_traversal.lln);

  llist_foreach(&alternation->segments, alt_segments, lln) {
    alt_segment_traversal.head = &alt_segments->list;
    alt_segment_traversal.segment = 0;
    alt_segment_traversal.end = 0;
    ctx->segment_traversal = &alt_segment_traversal;
    pattern_segment_generate(ctx);

    ctx->node = saved_context_node;
    ctx->section_traversal = saved_section_traversal;
    narrator_seek(ctx->section_narrator, 0, NARRATOR_SEEK_SET, 0);
    narrator_sayw(ctx->section_narrator, saved_text, saved_section_narrator_pos);
  }

  llist_delete(&alt_segment_traversal.lln);
  ctx->segment_traversal = llist_last(&ctx->segment_traversal_stack, typeof(*ctx->segment_traversal), lln);

  if(alternation->epsilon) {
    pattern_segment_generate(ctx);
  }
}

static void alternation_render(const pattern_segment * restrict pat, pattern_render_context * restrict ctx)
{
  const pattern_alternation * alternation = &pat->alternation;

  const pattern_segments *alt_segments;
  off_t start_pos;
  bool first;

  char saved_text[256];
  off_t saved_text_len;
  off_t saved_text_pos;

  // section traversal
  struct render_section_traversal saved_section_traversal;

  // segment traversal
  struct render_segment_traversal alt_segment_traversal;

  saved_section_traversal = ctx->section_traversal;

  // make a copy of the current section
  narrator_seek(ctx->narrator, 0, NARRATOR_SEEK_CUR, &start_pos);
  saved_text_pos = ctx->pos + sizeof(pattern_render_fragment);
  saved_text_len = MIN(sizeof(saved_text), start_pos - ctx->pos - sizeof(pattern_render_fragment));
  narrator_seek(ctx->narrator, saved_text_pos, NARRATOR_SEEK_SET, 0);
  narrator_read(ctx->narrator, saved_text, saved_text_len, 0);
  narrator_seek(ctx->narrator, start_pos, NARRATOR_SEEK_SET, 0);

  memset(&alt_segment_traversal, 0, sizeof(alt_segment_traversal));
  llist_init(&alt_segment_traversal.lln);
  llist_append(&ctx->segment_traversal_stack, &alt_segment_traversal.lln);

  first = true;
  llist_foreach(&alternation->segments, alt_segments, lln) {
    if(!first)
    {
      // start a new item - save the current offset
      narrator_seek(ctx->narrator, 0, NARRATOR_SEEK_CUR, &ctx->pos);

      // save a place for this fragment length
      narrator_sayw(ctx->narrator, (uint16_t[]) { 0 }, sizeof(uint16_t));

      // replay preceeding text
      narrator_sayw(ctx->narrator, saved_text, saved_text_len);
    }
    first = false;

    alt_segment_traversal.head = &alt_segments->list;
    alt_segment_traversal.segment = 0;
    alt_segment_traversal.end = false;
    ctx->segment_traversal = &alt_segment_traversal;

    // render sub-segments
    pattern_segment_render(ctx);

    // restore
    ctx->section_traversal = saved_section_traversal;
  }

  llist_delete(&alt_segment_traversal.lln);
  ctx->segment_traversal = llist_last(&ctx->segment_traversal_stack, typeof(*ctx->segment_traversal), lln);

  if(alternation->epsilon)
  {
    if(!first)
    {
      // start a new item - save the current offset
      narrator_seek(ctx->narrator, 0, NARRATOR_SEEK_CUR, &ctx->pos);

      // save a place for this fragment length
      narrator_sayw(ctx->narrator, (uint16_t[]) { 0 }, sizeof(uint16_t));

      // replay preceeding text
      narrator_sayw(ctx->narrator, saved_text, saved_text_len);
    }

    pattern_segment_render(ctx);
  }
}

static int alternation_cmp(const pattern_segment * A, const pattern_segment *B)
{
  int d;

  if((d = INTCMP(A->alternation.epsilon, B->alternation.epsilon)))
    return d;

  return pattern_segments_list_cmp(&A->alternation.segments, &B->alternation.segments);
}

static pattern_segment_vtable vtable = PATTERN_SEGMENT_VTABLE(alternation, PATTERN_ALTERNATION, PATTERN_SEGMENT_SEARCH_RECURSIVE);

//
// public
//

void pattern_alternation_mk(
    pattern_segment ** restrict rv
  , const yyu_location * restrict loc
  , llist * restrict segments // pattern_segments * restrict segments_head
  , bool epsilon
)
{
  pattern_segment * n;

  xmalloc(&n, sizeof(*n));
  pattern_segment_init(n, &vtable, loc);

  llist_init(&n->alternation.segments);
  llist_splice_head(&n->alternation.segments, segments);//  n->alternation.segments_head = segments_head;
  n->alternation.epsilon = epsilon;

  *rv = n;
}
