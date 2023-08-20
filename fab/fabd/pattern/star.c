/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "narrator.h"
#include "xlinux/xstdlib.h"

#include "star.h"
#include "fsent.h"
#include "search.internal.h"
#include "match.internal.h"
#include "pattern.internal.h"

//
// static
//

static void say(const pattern_segment * restrict n, narrator * restrict N)
{
  xsayc('*');
}

static void destroy(pattern_segment * restrict n)
{

}

static void match(const pattern_segment * restrict segment, pattern_match_context * restrict ctx)
{
  uint16_t namel;
  uint16_t delta;
  uint16_t start;
  struct match_segments_traversal *traversal;

  //namel = ctx->node->name.namel;
  namel = ctx->label_len;
  traversal = ctx->traversal;
  start = ctx->traversal->offset;

  for(delta = namel - start; delta >= 0 && delta != 0xffff; delta--)
  {
    ctx->traversal->offset = start + delta;

    pattern_segments_match(ctx);
    if(ctx->traversal->u.match) {
      break;
    }

    ctx->traversal = traversal;
  }
}

static void search(const pattern_segment * restrict segment, pattern_search_context * restrict ctx)
{
  uint16_t namel;
  uint16_t delta;
  uint16_t start;
  struct search_segments_traversal *traversal;

  namel = ctx->node->name.namel;
  traversal = ctx->traversal;
  start = ctx->traversal->offset;

  for(delta = namel - start; delta >= 0 && delta != 0xffff; delta--)
  {
    ctx->traversal->offset = start + delta;

    pattern_segments_search(ctx);
    if(ctx->matched)
      break;

    ctx->traversal = traversal;
  }
}

static int cmp(const pattern_segment * A, const pattern_segment *B)
{
  return 0;
}

static pattern_segment_vtable vtable = {
    type : PATTERN_STAR
  , say : say
  , destroy : destroy
  , search : search
  , match : match
  , cmp : cmp
};

//
// public
//

void pattern_star_mk(pattern_segment ** restrict rv, const yyu_location * restrict loc)
{
  pattern_segment * n = 0;

  xmalloc(&n, sizeof(*n));
  pattern_segment_init(n, &vtable, loc);

  chain_init(n, chn);

  *rv = n;
}
