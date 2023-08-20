/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "narrator.h"
#include "xlinux/xstdlib.h"

#include "range.h"
#include "byte.h"
#include "fsent.h"
#include "search.internal.h"
#include "match.internal.h"
#include "pattern.internal.h"

//
// static
//

static void say(const pattern_segment * restrict fn, narrator * restrict N)
{
  const pattern_range * n = &fn->range;

  byte_say(n->start, N);
  xsayc('-');
  byte_say(n->end, N);
}

static void destroy(pattern_segment * restrict fn)
{
}

static void match(const pattern_segment * restrict segment, pattern_match_context * restrict ctx)
{
  const pattern_range * range = &segment->range;
  const char * restrict name; // = ctx->node->name.name;
  uint16_t namel; // = ctx->node->name.namel;

  name = ctx->label;
  namel = ctx->label_len;

  if((namel > ctx->traversal->offset) && (name[ctx->traversal->offset] >= range->start && name[ctx->traversal->offset] <= range->end))
  {
    ctx->traversal->offset += 1;
  }
}

static void search(const pattern_segment * restrict segment, pattern_search_context * restrict ctx)
{
  const pattern_range * range;
  const char * restrict name;
  uint16_t namel;

  range = &segment->range;
  name = ctx->node->name.name;
  namel = ctx->node->name.namel;

  if((namel > ctx->traversal->offset) && (name[ctx->traversal->offset] >= range->start && name[ctx->traversal->offset] <= range->end))
  {
    ctx->traversal->offset += 1;
  }
}

static int cmp(const pattern_segment * A, const pattern_segment *B)
{
  int d;

  if((d = INTCMP(A->range.start, B->range.start)))
    return d;

  if((d = INTCMP(A->range.end, B->range.end)))
    return d;

  return 0;
}

static pattern_segment_vtable vtable = {
    type : PATTERN_RANGE
  , say : say
  , destroy : destroy
  , search : search
  , match : match
  , cmp : cmp
};

//
// internal
//

void pattern_range_mk(pattern_segment ** restrict rv, const yyu_location * restrict loc, uint8_t start, uint8_t end)
{
  pattern_segment * n = 0;

  xmalloc(&n, sizeof(*n));
  pattern_segment_init(n, &vtable, loc);

  n->range.start = start;
  n->range.end = end;

  chain_init(n, chn);

  *rv = n;
}
