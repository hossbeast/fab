/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "narrator.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"

#include "group.h"
#include "pattern.internal.h"
#include "search.internal.h"
#include "segment.h"

//
// static
//

static void say(const pattern_segment * restrict fn, narrator * restrict N)
{
  const pattern_group * n = &fn->group;

  xsayc('(');
  if(n->name) {
    xsayf("?<%.*s>", n->name_len, n->name);
  }

  pattern_segments_say(n->segments_head, true, N);
  xsayc(')');
}

static void destroy(pattern_segment * restrict n)
{
  pattern_group * group = &n->group;

  pattern_segments_list_free(group->segments_head);
  wfree(group->name);
}

static void search(const pattern_segment * restrict segment, pattern_search_context * restrict ctx)
{
  const pattern_group * group = &segment->group;
  struct search_segments_traversal traversal;

  traversal = (typeof(traversal)) {
    segments_head : group->segments_head
  };
  traversal.container.segment = segment;
  traversal.segments = chain_next(traversal.segments_head, &traversal.segments_cursor, chn);
  traversal.start = traversal.offset = ctx->traversal->offset;

  traversal.u.prev = ctx->traversal;

  ctx->traversal = &traversal;
  pattern_segments_search(ctx);

  // pop
  ctx->traversal = traversal.u.prev;
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
  , search : search
  , cmp : cmp
};

//
// public
//

void pattern_group_mk(
    pattern_segment ** restrict rv
  , const yyu_location * restrict loc
  , pattern_segments * restrict segments_head
  , const char * restrict name
  , uint8_t name_len
  , uint16_t group_number
)
{
  pattern_segment * n = 0;

  xmalloc(&n, sizeof(*n));
  pattern_segment_init(n, &vtable, loc);

  n->group.segments_head = segments_head;
  n->group.num = group_number;

  if(name)
  {
    n->group.name_len = name_len;
    ixstrndup(&n->group.name, name, name_len);
  }

  chain_init(n, chn);

  *rv = n;
}
