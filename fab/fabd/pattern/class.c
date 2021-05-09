/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

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

#include "narrator.h"
#include "xlinux/xstdlib.h"

#include "class.h"
#include "pattern.internal.h"
#include "fsent.h"
#include "generate.internal.h"
#include "search.internal.h"
#include "match.internal.h"
#include "render.internal.h"
#include "segment.h"

//
// static
//

static xapi say(const pattern_segment * restrict fn, narrator * restrict N)
{
  enter;

  const pattern_class * n = &fn->class;
  const chain *T;
  pattern_segments *segments;

  xsayc('[');

  if(n->invert)
    xsayc('!');

  chain_foreach(T, segments, chn, n->segments_head) {
    fatal(pattern_segment_chain_say, segments->segment_head, N);
  }

  xsayc(']');

  finally : coda;
}

static xapi render(const pattern_segment * restrict fn, pattern_render_context * restrict ctx)
{
  enter;

  const pattern_class * class = &fn->class;

  const chain *T;
  const pattern_segments * alt_segments;
  const chain *alt_cursor;
  const pattern_segment * alt_segment;
  off_t start_pos;
  bool first;

  char saved_text[256];
  off_t saved_text_len;
  off_t saved_text_pos;

  // make a copy of the current section
  fatal(narrator_xseek, ctx->narrator, 0, NARRATOR_SEEK_CUR, &start_pos);
  saved_text_pos = ctx->pos + sizeof(pattern_render_fragment);
  saved_text_len = MIN(sizeof(saved_text), start_pos - ctx->pos - sizeof(pattern_render_fragment));
  fatal(narrator_xseek, ctx->narrator, saved_text_pos, NARRATOR_SEEK_SET, 0);
  fatal(narrator_xread, ctx->narrator, saved_text, saved_text_len, 0);
  fatal(narrator_xseek, ctx->narrator, start_pos, NARRATOR_SEEK_SET, 0);

  // section traversal
  struct render_section_traversal saved_section_traversal;
  struct render_segment_traversal *saved_segment_traversal;

  saved_section_traversal = ctx->section_traversal;
  saved_segment_traversal = ctx->segment_traversal;

  first = true;
  chain_foreach(T, alt_segments, chn, class->segments_head) {
    alt_cursor = 0;
    alt_segment = chain_next(alt_segments->segment_head, &alt_cursor, chn);

    uint8_t c;
    uint8_t start = 0;
    uint8_t end = 0;

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
        fatal(narrator_xseek, ctx->narrator, 0, NARRATOR_SEEK_CUR, &ctx->pos);

        // save a place for this fragment length
        fatal(narrator_xsayw, ctx->narrator, (uint16_t[]) { 0 }, sizeof(uint16_t));

        // replay preceeding text
        fatal(narrator_xsayw, ctx->narrator, saved_text, saved_text_len);
      }
      first = false;

      ctx->segment_traversal = saved_segment_traversal;
      fatal(narrator_xsayc, ctx->narrator, c);

      // continue
      fatal(pattern_segment_render, ctx);

      // restore
      ctx->section_traversal = saved_section_traversal;
    }
  }

  finally : coda;
}

static void destroy(pattern_segment * restrict n)
{
  pattern_segments_list_free(n->class.segments_head);
}

static xapi search(const pattern_segment * restrict segment, pattern_search_context * restrict ctx)
{
  enter;

  const pattern_class * class = &segment->class;
  struct search_segments_traversal traversal;

  traversal = (typeof(traversal)) {
      segments_head : class->segments_head
  };
  traversal.container.segment = segment;
  traversal.segments = chain_next(traversal.segments_head, &traversal.segments_cursor, chn);
  traversal.start = traversal.offset = ctx->traversal->offset;

  traversal.u.prev = ctx->traversal;
  ctx->traversal = &traversal;

  fatal(ctx->segments_process, ctx);

  ctx->traversal = traversal.u.prev;

  finally : coda;
}

static xapi generate(const pattern_segment * restrict pat, pattern_generate_context * restrict ctx)
{
  enter;

  const pattern_class * class = &pat->class;
  const chain *T;
  const pattern_segments * alt_segments;
  const chain *alt_cursor;
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
  fatal(narrator_xseek, ctx->section_narrator, 0, NARRATOR_SEEK_CUR, &saved_section_narrator_pos);
  fatal(narrator_xseek, ctx->section_narrator, 0, NARRATOR_SEEK_SET, 0);
  fatal(narrator_xread, ctx->section_narrator, saved_section_text, saved_section_narrator_pos, 0);

  chain_foreach(T, alt_segments, chn, class->segments_head) {
    alt_cursor = 0;
    alt_segment = chain_next(alt_segments->segment_head, &alt_cursor, chn);

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
      fatal(narrator_xsayc, ctx->section_narrator, c);
      fatal(pattern_segment_generate, ctx);

      ctx->node = saved_context_node;
      ctx->section_traversal = saved_section_traversal;
      fatal(narrator_xseek, ctx->section_narrator, 0, NARRATOR_SEEK_SET, 0);
      fatal(narrator_xsayw, ctx->section_narrator, saved_section_text, saved_section_narrator_pos);
    }
  }

  finally : coda;
}

static int cmp(const pattern_segment * A, const pattern_segment *B)
{
  int d;

  if((d = INTCMP(A->class.invert, B->class.invert)))
    return d;

  return pattern_segments_cmp(A->class.segments_head, B->class.segments_head);
}

static pattern_segment_vtable vtable = {
    type : PATTERN_CLASS
  , say : say
  , render : render
  , destroy : destroy
  , search : search
  , generate : generate
  , cmp : cmp
};

//
// public
//

xapi pattern_class_mk(pattern_segment ** restrict rv, const yyu_location * restrict loc, pattern_segments * restrict segments_head, bool invert)
{
  enter;

  pattern_segment * n = 0;

  fatal(xmalloc, &n, sizeof(*n));
  fatal(pattern_segment_init, n, &vtable, loc);

  n->class.segments_head = segments_head;
  n->class.invert = invert;

  chain_init(n, chn);

  *rv = n;

  finally : coda;
}
