/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "narrator.h"
#include "xlinux/xstdlib.h"
#include "common/attrs.h"

#include "star.h"
#include "fsent.h"
#include "search.internal.h"
#include "match.internal.h"
#include "pattern.internal.h"

//
// static
//

static void star_describe(const pattern_segment * restrict n, struct bacon_writer * restrict builder_writer)
{
}

static void star_say(const pattern_segment * restrict n, const pattern_segment * restrict next, narrator * restrict N)
{
  xsayc('*');
}

static void star_destroy(pattern_segment * restrict n)
{

}

static void star_search(const pattern_segment * restrict segment, pattern_search_context * restrict ctx)
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
    traversal->sealed = false;

    pattern_segments_search(ctx);
    ctx->traversal = traversal;
    if(ctx->matched) {
      break;
    }
  }
}

static int star_cmp(const pattern_segment * A, const pattern_segment *B)
{
  return 0;
}

#define star_generate 0
#define star_render 0
static pattern_segment_vtable vtable = PATTERN_SEGMENT_VTABLE(star, PATTERN_STAR, PATTERN_SEGMENT_SEARCH_RECURSIVE);

//
// public
//

void pattern_star_mk(pattern_segment ** restrict rv, const yyu_location * restrict loc)
{
  pattern_segment * n = 0;

  xmalloc(&n, sizeof(*n));
  pattern_segment_init(n, &vtable, loc);

  *rv = n;
}
