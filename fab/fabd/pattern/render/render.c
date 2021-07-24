/* Copyright (c) 2012-2019 Todd Freed <todd.freed@gmail.com>

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

#include "narrator/growing.h"

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

  ctx->section_traversal.section = chain_next(ctx->section_traversal.head, &ctx->section_traversal.cursor, chn);

  // end of an item
  if(!ctx->section_traversal.section)
  {
    // write a null terminator
    narrator_xsayc(ctx->narrator, 0);

    // start pos - save
    narrator_xseek(ctx->narrator, 0, NARRATOR_SEEK_CUR, &start);
    len = start - ctx->pos - sizeof(pattern_render_fragment) - 1;

    // two-byte alignment
    if((len & 1) == 0) {
      narrator_xsayc(ctx->narrator, 0);
      start++;
    }

    // record the length
    narrator_xseek(ctx->narrator, ctx->pos, NARRATOR_SEEK_SET, 0);
    narrator_xsayw(ctx->narrator, &len, sizeof(len));
    narrator_xseek(ctx->narrator, start, NARRATOR_SEEK_SET, 0);

    ctx->size++;

    // start position for next item
    ctx->pos = start;
  }
  else
  {
    if(ctx->section_traversal.section != ctx->section_traversal.head) {
      narrator_xsayc(ctx->narrator, '/');
    }

    /* NODESET_SELF or '.' */
    if(ctx->section_traversal.section->nodeset == PATTERN_NODESET_SHADOW)
    {
      narrator_xsays(ctx->narrator, "/");
      pattern_section_render(ctx);
    }
    else if(ctx->section_traversal.section->nodeset == PATTERN_NODESET_SELF)
    {
      narrator_xsays(ctx->narrator, ".");
      pattern_section_render(ctx);
    }
    else
    {
      memset(&segment_traversal, 0, sizeof(segment_traversal));
      segment_traversal.head = ctx->section_traversal.section->qualifiers_head->segment_head;
      llist_append(&ctx->segment_traversal_stack, &segment_traversal, lln);
      ctx->segment_traversal = llist_last(&ctx->segment_traversal_stack, typeof(*ctx->segment_traversal), lln);
      pattern_segment_render(ctx);

      llist_delete(&segment_traversal, lln);
    }
  }
}

//
// internal
//

void pattern_segment_render(pattern_render_context * restrict ctx)
{
  struct render_segment_traversal *segment_traversal;
  const chain * segchain;

  segment_traversal = ctx->segment_traversal;
  segchain = segment_traversal->cursor;

  segment_traversal->segment = chain_next(segment_traversal->head, &segment_traversal->cursor, chn);

  // continue with the current segment
  if(segment_traversal->segment)
  {
    segment_traversal->segment->vtab->render(segment_traversal->segment, ctx);
    return;
  }

  // ascend to the outer segment traversal
  ctx->segment_traversal = llist_prev(&ctx->segment_traversal_stack, ctx->segment_traversal, lln);
  if(ctx->segment_traversal)
  {
    pattern_segment_render(ctx);
    return;
  }

  pattern_section_render(ctx);

  segment_traversal->cursor = segchain;
}

//
// public
//

void pattern_render(const pattern * restrict pattern, pattern_render_result ** result)
{
  pattern_render_context ctx = { 0 };

  ctx.section_traversal.head = pattern->section_head;
  ctx.segment_traversal_stack = LLIST_INITIALIZER(ctx.segment_traversal_stack);
  narrator_growing_create(&ctx.narrator_growing);

  // save a place in the buffer for the number of path fragments
  narrator_xsayw(ctx.narrator, (uint16_t[]) { 0 }, sizeof(uint16_t));

  // save the current offset
  narrator_xseek(ctx.narrator, 0, NARRATOR_SEEK_CUR, &ctx.pos);

  // save a place in the buffer for the length of the first fragment
  narrator_xsayw(ctx.narrator, (uint16_t[]) { 0 }, sizeof(uint16_t));

  // render the entire pattern
  pattern_section_render(&ctx);

  // update the number of path fragments
  narrator_xseek(ctx.narrator, 0, NARRATOR_SEEK_SET, 0);
  narrator_xsayw(ctx.narrator, &ctx.size, sizeof(ctx.size));

  // ownership transfer
  narrator_growing_claim_buffer(ctx.narrator_growing, (void*)result, 0);

  narrator_growing_free(ctx.narrator_growing);
}
