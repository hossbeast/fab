/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "narrator/growing.h"
#include "bacon/value.h"

#include "render.internal.h"
#include "pattern.internal.h"

//
// static
//

static void pattern_section_render(pattern_render_context * restrict ctx)
{
  off_t start;
  uint16_t len;
  struct render_segment_traversal segment_traversal;
  pattern_segments *segments;

  if(!ctx->section_traversal.section) {
    ctx->section_traversal.section = llist_first(ctx->section_traversal.head, typeof(*ctx->section_traversal.section), lln);
  } else {
    ctx->section_traversal.section = llist_next(ctx->section_traversal.head, ctx->section_traversal.section, lln);
  }
  if(ctx->flags & PATTERN_RENDER_LAST_SECTION_SKIP) {
    if(ctx->section_traversal.section == llist_last(ctx->section_traversal.head, typeof(*ctx->section_traversal.section), lln)) {
      ctx->section_traversal.section = 0;
    }
  }

  // end of an item
  if(!ctx->section_traversal.section)
  {
    // write a null terminator
    narrator_sayc(ctx->narrator, 0);

    // start pos - save
    narrator_seek(ctx->narrator, 0, NARRATOR_SEEK_CUR, &start);
    len = start - ctx->pos - sizeof(pattern_render_fragment) - 1;

    // two-byte alignment
    if((len & 1) == 0) {
      narrator_sayc(ctx->narrator, 0);
      start++;
    }

    // record the length
    narrator_seek(ctx->narrator, ctx->pos, NARRATOR_SEEK_SET, 0);
    narrator_sayw(ctx->narrator, &len, sizeof(len));
    narrator_seek(ctx->narrator, start, NARRATOR_SEEK_SET, 0);

    ctx->size++;

    // start position for next item
    ctx->pos = start;
  }
  else
  {
    if(ctx->section_traversal.section != llist_first(ctx->section_traversal.head, pattern_section, lln)) {
      narrator_sayc(ctx->narrator, '/');
    }

    /* AXIS SELF or '.' */
    if(ctx->section_traversal.section->nodeset == PATTERN_NODESET_SHADOW)
    {
      narrator_says(ctx->narrator, "/");
      pattern_section_render(ctx);
    }
    else if(ctx->section_traversal.section->axis == PATTERN_AXIS_SELF)
    {
      narrator_says(ctx->narrator, ".");
      pattern_section_render(ctx);
    }
    else if(ctx->section_traversal.section->axis == PATTERN_AXIS_UP)
    {
      narrator_says(ctx->narrator, "..");
      pattern_section_render(ctx);
    }
    else
    {
      memset(&segment_traversal, 0, sizeof(segment_traversal));

      segments = llist_first(&ctx->section_traversal.section->qualifiers, typeof(*segments), lln);
      segment_traversal.head = &segments->list;

      llist_append(&ctx->segment_traversal_stack, &segment_traversal.lln);
      ctx->segment_traversal = llist_last(&ctx->segment_traversal_stack, typeof(*ctx->segment_traversal), lln);

      pattern_segment_render(ctx);

      llist_delete(&segment_traversal.lln);
    }
  }
}

//
// internal
//

void pattern_segment_render(pattern_render_context * restrict ctx)
{
  struct render_segment_traversal *segment_traversal;
  const pattern_segment *segment;
  bool end;

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
    segment_traversal->segment->vtab->render(segment_traversal->segment, ctx);
    goto end;
  }
  segment_traversal->end = true;

  // ascend to the outer segment traversal
  ctx->segment_traversal = llist_prev(&ctx->segment_traversal_stack, ctx->segment_traversal, lln);
  if(ctx->segment_traversal) {
    pattern_segment_render(ctx);
    goto end;
  }

  pattern_section_render(ctx);

end:
  segment_traversal->segment = segment;
  segment_traversal->end = end;
}

//
// public
//

void pattern_render(
    const pattern * restrict ref
  , const bacon_value * restrict vars
  , pattern_render_result ** result
  , uint32_t flags
)
{
  pattern_render_context ctx = (typeof(ctx)){
      vars : vars
    , flags : flags
  };

  ctx.section_traversal.head = &ref->sections;
  llist_init(&ctx.segment_traversal_stack);
  narrator_growing_create(&ctx.narrator_growing);

  // save a place in the buffer for the number of path fragments
  narrator_sayw(ctx.narrator, (uint16_t[]) { 0 }, sizeof(uint16_t));

  // save the current offset
  narrator_seek(ctx.narrator, 0, NARRATOR_SEEK_CUR, &ctx.pos);

  // save a place in the buffer for the length of the first fragment
  narrator_sayw(ctx.narrator, (uint16_t[]) { 0 }, sizeof(uint16_t));

  if(flags & PATTERN_RENDER_LAST_SECTION_ONLY) {
    ctx.section_traversal.section = llist_last(ctx.section_traversal.head, typeof(*ctx.section_traversal.section), lln);
    ctx.section_traversal.section = llist_prev(ctx.section_traversal.head, ctx.section_traversal.section, lln);
  }

  // render the entire pattern
  pattern_section_render(&ctx);

  // update the number of path fragments
  narrator_seek(ctx.narrator, 0, NARRATOR_SEEK_SET, 0);
  narrator_sayw(ctx.narrator, &ctx.size, sizeof(ctx.size));

  // ownership transfer
  narrator_growing_claim_buffer(ctx.narrator_growing, (void*)result, 0);

  narrator_growing_free(ctx.narrator_growing);
}
