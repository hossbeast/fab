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

#include "types.h"
#include "xapi.h"

#include "narrator.h"
#include "value/writer.h"
#include "narrator/fixed.h"
#include "valyria/list.h"
#include "valyria/dictionary.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"

#include "group.internal.h"
#include "pattern.internal.h"
#include "segment.internal.h"
#include "generate.internal.h"
#include "match.internal.h"
#include "node.h"
#include "path.h"
#include "common/attrs.h"

//
// static
//

static xapi say(const pattern_segment * restrict fn, narrator * restrict N)
{
  enter;

  const pattern_group * n = &fn->group;

  xsayc('(');
  if(n->name) {
    xsayf("?<%.*s>", n->name_len, n->name);
  }

  fatal(pattern_segments_say, n->segments_head, true, N);
  xsayc(')');

  finally : coda;
}

static void destroy(pattern_segment * restrict n)
{
  pattern_group * group = &n->group;

  pattern_segments_list_free(group->segments_head);
  wfree(group->name);
}

static xapi match(pattern_match_context * restrict ctx, const pattern_segment * restrict segment)
{
  enter;

  const pattern_group * group = &segment->group;
  struct match_segments_traversal traversal;

  traversal = (typeof(traversal)) {
    segments_head : group->segments_head
  };
  traversal.container.segment = segment;
  traversal.segments = chain_next(traversal.segments_head, &traversal.segments_cursor, chn);
  traversal.start = traversal.offset = ctx->traversal->offset;

  traversal.u.prev = ctx->traversal;

  ctx->traversal = &traversal;

  fatal(pattern_segments_match, ctx);

  // pop
  ctx->traversal = traversal.u.prev;

  finally : coda;
}

static int cmp(const pattern_segment * A, const pattern_segment *B)
{
  int d;

  if((d = INTCMP(A->group.num, B->group.num)))
    return d;

  if((d = memncmp(A->group.name, A->group.name_len, B->group.name, B->group.name_len)))
    return d;

  return pattern_segments_cmp(A->group.segments_head, B->group.segments_head);
}

static pattern_segment_vtable vtable = {
    type : PATTERN_GROUP
  , say : say
  , destroy : destroy
  , match : match
  , cmp : cmp
};

//
// public
//

xapi pattern_group_mk(
    pattern_segment ** restrict rv
  , const yyu_location * restrict loc
  , pattern_segments * restrict segments_head
  , const char * restrict name
  , uint8_t name_len
  , uint16_t group_number
)
{
  enter;

  pattern_segment * n = 0;

  fatal(xmalloc, &n, sizeof(*n));
  fatal(pattern_segment_init, n, &vtable, loc);

  n->group.segments_head = segments_head;
  n->group.num = group_number;

  if(name)
  {
    n->group.name_len = name_len;
    fatal(ixstrndup, &n->group.name, name, name_len);
  }

  chain_init(n, chn);

  *rv = n;

  finally : coda;
}
