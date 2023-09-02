/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "narrator.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "common/attrs.h"
#include "bacon/builder.h"

#include "group.h"
#include "pattern.internal.h"
#include "search.internal.h"
#include "segment.h"
#include "fsent.h"
#include "segments.h"

//
// static
//

static void group_describe(const pattern_segment * restrict fn, bacon_writer * restrict builder_writer)
{
  const pattern_group *group = &fn->group;

  { MAPPING
    STRING("group-num")
    UINT(group->group_num)
  }
  if(group->capture_group_num != UINT16_MAX) {
    MAPPING
    STRING("capture-group-num")
    UINT(group->capture_group_num)
  }
  { MAPPING
    STRING("has-type-qualifiers")
    BOOL(group->has_type_qualifiers)
  }

  if(group->name_len) {
    MAPPING
    STRING("name")
    BYTES(group->name, group->name_len);
  }

  if(!llist_empty(&group->segments)) {
    MAPPING
    STRING("segments")
    pattern_segments_list_describe(&group->segments, builder_writer);
  }
}

static void group_say(const pattern_segment * restrict fn, const pattern_segment * restrict next, narrator * restrict N)
{
  const pattern_group * n = &fn->group;

  xsayc('(');
  if(n->name) {
    xsayf("?<%.*s>", n->name_len, n->name);
  }

  pattern_segments_list_say(&n->segments, true, N);
  xsayc(')');
}

static void group_destroy(pattern_segment * restrict n)
{
  pattern_group * group = &n->group;

  pattern_segments_list_free(&group->segments);
  free(group->name);
}

static void group_search(const pattern_segment * restrict segment, pattern_search_context * restrict ctx)
{
  const pattern_group *group = &segment->group;
  struct search_segments_traversal traversal;

  memset(&traversal, 0, sizeof(traversal));
  traversal.u.prev = ctx->traversal;
  traversal.start = traversal.u.prev->offset;
  traversal.offset = traversal.u.prev->offset;

  /* outer segments */
  traversal.segments_head = &group->segments;
  traversal.segments = llist_first(traversal.segments_head, typeof(*traversal.segments), lln);

  ctx->traversal = &traversal;
  pattern_segments_search(ctx);

  if(group->has_type_qualifiers)
  {
    /* type-qualifier groups dont capture */
  }
  else if(group->capture_group_num == UINT16_MAX)
  {
    /* non-capturing group */
  }
  else if(group->capture_group_num < (sizeof(ctx->groups) / sizeof(*ctx->groups)))
  {
    ctx->groups[group->capture_group_num].start = ctx->node->name.name + traversal.start;
    ctx->groups[group->capture_group_num].len = traversal.end - traversal.start;
    ctx->group_max = MAX(ctx->group_max, group->capture_group_num);
  }

  // pop
  ctx->traversal = traversal.u.prev;
}

static int group_cmp(const pattern_segment * A, const pattern_segment *B)
{
  int d;

  if((d = INTCMP(A->group.capture_group_num, B->group.capture_group_num)))
    return d;

  if((d = memncmp(A->group.name, A->group.name_len, B->group.name, B->group.name_len)))
    return d;

  return pattern_segments_list_cmp(&A->group.segments, &B->group.segments);
}

#define group_render 0
#define group_match 0
#define group_generate 0
static pattern_segment_vtable vtable = PATTERN_SEGMENT_VTABLE(group, PATTERN_GROUP, PATTERN_SEGMENT_SEARCH_RECURSIVE);

//
// public
//

void pattern_group_mk(
    pattern_segment ** restrict rv
  , const yyu_location * restrict loc
  , llist * restrict segments
  , const char * restrict name
  , uint8_t name_len
  , uint16_t group_number
  , uint16_t capture_group_number
)
{
  pattern_segment * n = 0;

  xmalloc(&n, sizeof(*n));
  pattern_segment_init(n, &vtable, loc);

  llist_init(&n->group.segments);
  llist_splice_head(&n->group.segments, segments);
  n->group.group_num = group_number;
  n->group.capture_group_num = capture_group_number;

  if(name)
  {
    n->group.name_len = name_len;
    ixstrndup(&n->group.name, name, name_len);
  }

  *rv = n;
}
