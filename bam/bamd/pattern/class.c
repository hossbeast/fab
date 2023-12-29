/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "narrator.h"
#include "xlinux/xstdlib.h"
#include "common/attrs.h"
#include "bacon/builder.h"

#include "class.h"
#include "pattern.internal.h"
#include "fsent.h"
#include "generate.internal.h"
#include "search.internal.h"
#include "match.internal.h"
#include "render.internal.h"
#include "segment.h"
#include "segments.h"

//
// static
//

static void class_describe(const pattern_segment * restrict fn, bacon_writer * restrict builder_writer)
{
  const pattern_class *class = &fn->class;

  { MAPPING
    STRING("invert")
    BOOL(class->invert)
  }

  if(!llist_empty(&class->segments)) {
    MAPPING
    STRING("segments")
    pattern_segments_list_describe(&class->segments, builder_writer);
  }
}

static void class_say(const pattern_segment * restrict fn, const pattern_segment * restrict next, narrator * restrict N)
{
  const pattern_class * n = &fn->class;
  pattern_segments *segments;

  xsayc('[');

  if(n->invert) {
    xsayc('!');
  }

  llist_foreach(&n->segments, segments, lln) {
    pattern_segment_list_say(&segments->list, N);
  }

  xsayc(']');
}

static void class_render(const pattern_segment * restrict fn, pattern_render_context * restrict ctx)
{
  const pattern_class * class = &fn->class;

  const pattern_segments * alt_segments;
  const pattern_segment * alt_segment;
  off_t start_pos;
  bool first;

  char saved_text[256];
  off_t saved_text_len;
  off_t saved_text_pos;

  uint8_t c;
  uint8_t start;
  uint8_t end;

  // make a copy of the current section
  narrator_seek(ctx->narrator, 0, NARRATOR_SEEK_CUR, &start_pos);
  saved_text_pos = ctx->pos + sizeof(pattern_render_fragment);
  saved_text_len = MIN(sizeof(saved_text), start_pos - ctx->pos - sizeof(pattern_render_fragment));
  narrator_seek(ctx->narrator, saved_text_pos, NARRATOR_SEEK_SET, 0);
  narrator_read(ctx->narrator, saved_text, saved_text_len, 0);
  narrator_seek(ctx->narrator, start_pos, NARRATOR_SEEK_SET, 0);

  // section traversal
  struct render_section_traversal saved_section_traversal;
  struct render_segment_traversal *saved_segment_traversal;

  saved_section_traversal = ctx->section_traversal;
  saved_segment_traversal = ctx->segment_traversal;

  first = true;
  llist_foreach(&class->segments, alt_segments, lln) {
    llist_foreach(&alt_segments->list, alt_segment, lln) {

      start = 0;
      end = 0;

      if(alt_segment->vtab->type == PATTERN_CHARACTER)
      {
        start = end = alt_segment->byte.code;
      }
      else if(alt_segment->vtab->type == PATTERN_RANGE)
      {
        start = alt_segment->range.start;
        end = alt_segment->range.end;
      }

      for(c = start; c <= end; c++)
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
        first = false;

        ctx->segment_traversal = saved_segment_traversal;
        narrator_sayc(ctx->narrator, c);

        // continue
        pattern_segment_render(ctx);

        // restore
        ctx->section_traversal = saved_section_traversal;
      }
    }
  }
}

static void class_destroy(pattern_segment * restrict n)
{
  pattern_segments_list_free(&n->class.segments);
}

static void class_search(const pattern_segment * restrict segment, pattern_search_context * restrict ctx)
{
  const pattern_class * class = &segment->class;
  struct search_segments_traversal traversal;

  memset(&traversal, 0, sizeof(traversal));
  traversal.u.prev = ctx->traversal;
  traversal.start = traversal.u.prev->offset;
  traversal.offset = traversal.u.prev->offset;

  /* inner segments : the ranges */
  if(!(traversal.segments = llist_first(&class->segments, typeof(*traversal.segments), lln))) {
    goto end;
  }

  while(!ctx->matched)
  {
    traversal.segment_head = &traversal.segments->list;
    traversal.segment = 0;
    traversal.unsatisfied = false;
    traversal.sealed = false;

    ctx->traversal = &traversal;
    pattern_segments_search(ctx);

    if(!(traversal.segments = llist_next(&class->segments, traversal.segments, lln))) {
      break;
    }
  }

end:
  // pop
  ctx->traversal = traversal.u.prev;
}

static void class_generate(const pattern_segment * restrict pat, pattern_generate_context * restrict ctx)
{
  const pattern_class * class = &pat->class;
  const pattern_segments * alt_segments;
  const pattern_segment * alt_segment;

  uint8_t c;
  uint8_t start = 0;
  uint8_t end = 0;

  fsent * saved_context_node;
  char saved_section_text[256];
  off_t saved_section_narrator_pos;

  // section traversal
  struct section_traversal saved_section_traversal;
  struct segment_traversal *saved_segment_traversal;

  saved_context_node = ctx->node;
  saved_section_traversal = ctx->section_traversal;
  saved_segment_traversal = ctx->segment_traversal;
  narrator_seek(ctx->section_narrator, 0, NARRATOR_SEEK_CUR, &saved_section_narrator_pos);
  narrator_seek(ctx->section_narrator, 0, NARRATOR_SEEK_SET, 0);
  narrator_read(ctx->section_narrator, saved_section_text, saved_section_narrator_pos, 0);

  llist_foreach(&class->segments, alt_segments, lln) {
    llist_foreach(&alt_segments->list, alt_segment, lln) {
      start = 0;
      end = 0;

      if(alt_segment->vtab->type == PATTERN_CHARACTER)
      {
        start = end = alt_segment->byte.code;
      }
      else if(alt_segment->vtab->type == PATTERN_RANGE)
      {
        start = alt_segment->range.start;
        end = alt_segment->range.end;
      }

      for(c = start; c <= end; c++)
      {
        ctx->segment_traversal = saved_segment_traversal;
        narrator_sayc(ctx->section_narrator, c);
        pattern_segment_generate(ctx);

        ctx->node = saved_context_node;
        ctx->section_traversal = saved_section_traversal;
        narrator_seek(ctx->section_narrator, 0, NARRATOR_SEEK_SET, 0);
        narrator_sayw(ctx->section_narrator, saved_section_text, saved_section_narrator_pos);
      }
    }
  }
}

static int class_cmp(const pattern_segment * A, const pattern_segment *B)
{
  int d;

  if((d = INTCMP(A->class.invert, B->class.invert)))
    return d;

  return pattern_segments_list_cmp(&A->class.segments, &B->class.segments);
}

static pattern_segment_vtable vtable = PATTERN_SEGMENT_VTABLE(class, PATTERN_CLASS, PATTERN_SEGMENT_SEARCH_RECURSIVE);

//
// public
//

void pattern_class_mk(
    pattern_segment ** restrict rv
  , const yyu_location * restrict loc
  , llist * restrict segments // pattern_segments * restrict segments_head
  , bool invert
)
{
  pattern_segment * n = 0;

  xmalloc(&n, sizeof(*n));
  pattern_segment_init(n, &vtable, loc);

  llist_init(&n->class.segments);
  llist_splice_head(&n->class.segments, segments); // n->class.segments_head = segments_head;
  n->class.invert = invert;

  *rv = n;
}
