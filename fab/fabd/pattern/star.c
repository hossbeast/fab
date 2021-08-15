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

#include "star.h"
#include "fsent.h"
#include "search.internal.h"
#include "match.internal.h"
#include "pattern.internal.h"

//
// static
//

static xapi say(const pattern_segment * restrict n, narrator * restrict N)
{
  enter;

  xsayc('*');

  finally : coda;
}

static void destroy(pattern_segment * restrict n)
{

}

static xapi match(const pattern_segment * restrict segment, pattern_match_context * restrict ctx)
{
  enter;

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

    fatal(pattern_segments_match, ctx);
    if(ctx->traversal->u.match) {
      break;
    }

    ctx->traversal = traversal;
  }

  finally : coda;
}

static xapi search(const pattern_segment * restrict segment, pattern_search_context * restrict ctx)
{
  enter;

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

    fatal(pattern_segments_search, ctx);
    if(ctx->matched)
      break;

    ctx->traversal = traversal;
  }

  finally : coda;
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

xapi pattern_star_mk(pattern_segment ** restrict rv, const yyu_location * restrict loc)
{
  enter;

  pattern_segment * n = 0;

  fatal(xmalloc, &n, sizeof(*n));
  fatal(pattern_segment_init, n, &vtable, loc);

  chain_init(n, chn);

  *rv = n;

  finally : coda;
}
