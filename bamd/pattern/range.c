/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "narrator.h"
#include "xlinux/xstdlib.h"
#include "bacon/builder.h"

#include "range.h"
#include "byte.h"
#include "fsent.h"
#include "search.internal.h"
#include "pattern.internal.h"

//
// static
//

static void range_describe(const pattern_segment * restrict fn, bacon_writer * restrict builder_writer)
{
  const pattern_range *range = &fn->range;

  { MAPPING
    STRING("start")
    LOADF("0x%02x", range->start)
  }
  { MAPPING
    STRING("end")
    LOADF("0x%02x", range->end)
  }
}

static void range_say(const pattern_segment * restrict fn, const pattern_segment * restrict next, narrator * restrict N)
{
  const pattern_range * n = &fn->range;

  byte_say_code(n->start, N);
  xsayc('-');
  byte_say_code(n->end, N);
}

static void range_destroy(pattern_segment * restrict fn)
{
}

static void range_search(const pattern_segment * restrict segment, pattern_search_context * restrict ctx)
{
  const pattern_range * range;
  const char * restrict name;
  uint16_t namel;

  range = &segment->range;
  name = ctx->node->name.name;
  namel = ctx->node->name.namel;

  if((namel > ctx->traversal->offset) && (name[ctx->traversal->offset] >= range->start && name[ctx->traversal->offset] <= range->end)) {
    ctx->traversal->offset += 1;
  } else {
    ctx->traversal->unsatisfied = true;
  }
}

static int range_cmp(const pattern_segment * A, const pattern_segment *B)
{
  int d;

  if((d = INTCMP(A->range.start, B->range.start)))
    return d;

  if((d = INTCMP(A->range.end, B->range.end)))
    return d;

  return 0;
}

#define range_cat 0
#define range_generate 0
#define range_render 0
static pattern_segment_vtable vtable = PATTERN_SEGMENT_VTABLE(range, PATTERN_RANGE, 0);

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

  *rv = n;
}
