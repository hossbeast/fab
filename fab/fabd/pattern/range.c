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

#include <string.h>

#include "xapi.h"
#include "types.h"

#include "xlinux/xstdlib.h"
#include "value/writer.h"
#include "narrator.h"

#include "pattern.internal.h"
#include "range.internal.h"
#include "generate.internal.h"
#include "byte.internal.h"
#include "match.internal.h"
#include "node.h"
#include "path.h"

#include "attrs.h"

//
// static
//

static xapi say(const pattern_segment * restrict fn, narrator * restrict N)
{
  enter;

  const pattern_range * n = &fn->range;

  fatal(byte_say, n->start, N);
  xsayc('-');
  fatal(byte_say, n->end, N);

  finally : coda;
}

static void destroy(pattern_segment * restrict fn)
{
}

static xapi match(pattern_match_context * restrict ctx, const pattern_segment * restrict segment)
{
  enter;

  const pattern_range * range = &segment->range;

  const char * restrict name = ctx->node->name->name;
  uint16_t namel = ctx->node->name->namel;

  if((namel > ctx->traversal->offset) && (name[ctx->traversal->offset] >= range->start && name[ctx->traversal->offset] <= range->end))
  {
    ctx->traversal->offset += 1;
  }

  finally : coda;
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
  , match : match
  , cmp : cmp
};

//
// internal
//

xapi pattern_range_mk(pattern_segment ** restrict rv, const yyu_location * restrict loc, uint8_t start, uint8_t end)
{
  enter;

  pattern_segment * n = 0;

  fatal(xmalloc, &n, sizeof(*n));
  fatal(pattern_segment_init, n, &vtable, loc);

  n->range.start = start;
  n->range.end = end;

  chain_init(n, chn);

  *rv = n;

  finally : coda;
}