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

#include "xlinux/xstdlib.h"
#include "narrator.h"

#include "alternation.h"
#include "fsent.h"
#include "generate.internal.h"
#include "match.internal.h"
#include "pattern.internal.h"
#include "render.internal.h"
#include "segment.h"

//
// static
//

static xapi say(const pattern_segment * restrict fn, narrator * restrict N)
{
  enter;

  const pattern_alternation * n = &fn->alternation;
  pattern_segments * segments;
  const chain *T;
  int x;

  xsayc('{');

  x = 0;
  chain_foreach(T, segments, chn, n->segments_head) {
    if(x++) {
      xsays(",");
    }

    fatal(pattern_segment_chain_say, segments->segment_head, N);
  }

  if(n->epsilon) {
    xsayc(',');
  }

  xsayc('}');

  finally : coda;
}

static void destroy(pattern_segment * restrict n)
{
  pattern_segments * segments;
  const chain *T[2];

  pattern_alternation * alternation = &n->alternation;

  chain_foreach_safe(T, segments, chn, alternation->segments_head) {
    pattern_segments_free(segments);
  }
}

static xapi match(pattern_match_context * restrict ctx, const pattern_segment * restrict segment)
{
  enter;

  const pattern_alternation * alternation = &segment->alternation;

  struct match_segments_traversal traversal;

  traversal = (typeof(traversal)) {
      segments_head : alternation->segments_head
  };
  traversal.container.segment = segment;
  traversal.segments = chain_next(traversal.segments_head, &traversal.segments_cursor, chn);
  traversal.start = traversal.offset = ctx->traversal->offset;

  traversal.u.prev = ctx->traversal;

  ctx->traversal = &traversal;

  fatal(pattern_segments_match, ctx);

  ctx->traversal = traversal.u.prev;

  finally : coda;
}

static xapi generate(const pattern_segment * pat, pattern_generate_context * restrict ctx)
{
  enter;

  const pattern_alternation * alternation = &pat->alternation;

  const chain *T;
  const pattern_segments * alt_segments;
  fsent * saved_context_node;
  char saved_text[256];
  off_t saved_section_narrator_pos;

  // section traversal
  struct section_traversal saved_section_traversal;

  // segment traversal
  struct segment_traversal alt_segment_traversal;

  saved_context_node = ctx->node;
  saved_section_traversal = ctx->section_traversal;
  fatal(narrator_xseek, ctx->section_narrator, 0, NARRATOR_SEEK_CUR, &saved_section_narrator_pos);
  fatal(narrator_xseek, ctx->section_narrator, 0, NARRATOR_SEEK_SET, 0);
  fatal(narrator_xread, ctx->section_narrator, saved_text, saved_section_narrator_pos, 0);

  llist_append(&ctx->segment_traversal_stack, &alt_segment_traversal, lln);

  chain_foreach(T, alt_segments, chn, alternation->segments_head) {
    alt_segment_traversal.head = alt_segments->segment_head;
    alt_segment_traversal.cursor = 0;
    ctx->segment_traversal = &alt_segment_traversal;
    fatal(pattern_segment_generate, ctx);

    ctx->node = saved_context_node;
    ctx->section_traversal = saved_section_traversal;
    fatal(narrator_xseek, ctx->section_narrator, 0, NARRATOR_SEEK_SET, 0);
    fatal(narrator_xsayw, ctx->section_narrator, saved_text, saved_section_narrator_pos);
  }

  llist_delete(&alt_segment_traversal, lln);
  ctx->segment_traversal = llist_last(&ctx->segment_traversal_stack, typeof(*ctx->segment_traversal), lln);

  if(alternation->epsilon)
    fatal(pattern_segment_generate, ctx);

  finally : coda;
}

static xapi render(const pattern_segment * restrict pat, pattern_render_context * restrict ctx)
{
  enter;

  const pattern_alternation * alternation = &pat->alternation;

  const chain *T;
  const pattern_segments * alt_segments;
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
  fatal(narrator_xseek, ctx->narrator, 0, NARRATOR_SEEK_CUR, &start_pos);
  saved_text_pos = ctx->pos + sizeof(pattern_render_fragment);
  saved_text_len = MIN(sizeof(saved_text), start_pos - ctx->pos - sizeof(pattern_render_fragment));
  fatal(narrator_xseek, ctx->narrator, saved_text_pos, NARRATOR_SEEK_SET, 0);
  fatal(narrator_xread, ctx->narrator, saved_text, saved_text_len, 0);
  fatal(narrator_xseek, ctx->narrator, start_pos, NARRATOR_SEEK_SET, 0);

  llist_append(&ctx->segment_traversal_stack, &alt_segment_traversal, lln);

  first = true;
  chain_foreach(T, alt_segments, chn, alternation->segments_head) {
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

    alt_segment_traversal.head = alt_segments->segment_head;
    alt_segment_traversal.cursor = 0;
    ctx->segment_traversal = &alt_segment_traversal;

    // render sub-segments
    fatal(pattern_segment_render, ctx);

    // restore
    ctx->section_traversal = saved_section_traversal;
  }

  llist_delete(&alt_segment_traversal, lln);
  ctx->segment_traversal = llist_last(&ctx->segment_traversal_stack, typeof(*ctx->segment_traversal), lln);

  if(alternation->epsilon)
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

    fatal(pattern_segment_render, ctx);
  }

  finally : coda;
}

static int cmp(const pattern_segment * A, const pattern_segment *B)
{
  int d;

  if((d = INTCMP(A->alternation.epsilon, B->alternation.epsilon)))
    return d;

  return pattern_segments_cmp(A->alternation.segments_head, B->alternation.segments_head);
}

static pattern_segment_vtable vtable = {
    type : PATTERN_ALTERNATION
  , say : say
  , destroy : destroy
  , match : match
  , generate : generate
  , render : render
  , cmp : cmp
};

//
// public
//

xapi pattern_alternation_mk(
    pattern_segment ** restrict rv
  , const yyu_location * restrict loc
  , pattern_segments * restrict segments_head
  , bool epsilon
)
{
  enter;

  pattern_segment * n;

  fatal(xmalloc, &n, sizeof(*n));
  fatal(pattern_segment_init, n, &vtable, loc);

  n->alternation.segments_head = segments_head;
  n->alternation.epsilon = epsilon;

  chain_init(n, chn);

  *rv = n;

  finally : coda;
}
