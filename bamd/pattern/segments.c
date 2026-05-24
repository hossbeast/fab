/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>

#include "narrator/fixed.h"
#include "bacon/builder.h"
#include "bacon/value.h"
#include "valyria/llist.h"
#include "xlinux/xstdlib.h"

#include "pattern.internal.h"
#include "segments.h"
#include "segment.h"
#include "qualifier.h"
#include "module.h"

#include "common/attrs.h"

void pattern_segments_say(const pattern_segments * restrict segments, bool only, narrator * restrict N)
{
  if(segments->qualifier_type == PATTERN_QUALIFIER_TYPE_NOT) {
    xsays("~");
  } else if(segments->qualifier_type == PATTERN_QUALIFIER_TYPE_AND && !only) {
    xsays("+");
  }

  pattern_segment_list_say(&segments->list, N);
}

void pattern_segments_list_say(const llist * segments_list, bool only, narrator * restrict N)
{
  const pattern_segments * segments;

  llist_foreach(segments_list, segments, lln) {
    pattern_segments_say(segments, only, N);
    only = false;
  }
}

size_t pattern_segments_list_cat(
    void * restrict dst
  , size_t dst_size
  , const bacon_value * restrict vars
  , const llist * restrict segments
  , bool only
)
{
  narrator_fixed fixed;

  narrator_fixed_init(&fixed, dst, dst_size);
  pattern_segments_list_say(segments, true, &fixed.base);

  return fixed.l;
}

void pattern_segments_list_describe(const llist * restrict segments_list, struct bacon_writer * restrict builder_writer)
{
  const pattern_segments *segments;

  { LIST
    llist_foreach(segments_list, segments, lln) {
      { SET
        { MAPPING STRING("type")
          STRING(attrs16_name_byvalue(pattern_qualifier_type_attrs, segments->qualifier_type))
        }
        { MAPPING STRING("has-type-qualifiers")
          BOOL(segments->has_type_qualifiers)
        }
        { MAPPING STRING("list")
          pattern_segment_list_describe(&segments->list, builder_writer);
        }
      }
    }
  }
}

int pattern_segments_list_cmp(const llist * restrict A, const llist * restrict B)
{
  const pattern_segments * a;
  const pattern_segments * b;
  int d;

  a = llist_first(A, typeof(*a), lln);
  b = llist_first(B, typeof(*b), lln);
  while(a || b)
  {
    if((d = pattern_segments_cmp(a, b)))
      return d;

    a = llist_next(A, a, lln);
    b = llist_next(B, b, lln);
  }

  return 0;
}

int pattern_segments_cmp(const pattern_segments *A, const pattern_segments *B)
{
  const pattern_segment * a;
  const pattern_segment * b;
  int d;

  if(!A)
    return -1;

  if(!B)
    return 1;

  if((d = INTCMP(A->qualifier_type, B->qualifier_type)))
    return d;

  a = llist_first(&A->list, typeof(*a), lln);
  b = llist_first(&B->list, typeof(*b), lln);

  while(a || b)
  {
    if(!a)
      return -1;

    if(!b)
      return 1;

    if((d = pattern_segment_cmp(a, b)))
      return d;

    a = llist_next(&A->list, a, lln);
    b = llist_next(&B->list, b, lln);
  }

  return 0;
}


void pattern_segments_mk(
    pattern_segments ** restrict rv
  , const struct yyu_location * restrict loc
  , pattern_qualifier_type qualifier_type
  , bool has_type_qualifiers
  , llist * restrict list
)
{
  pattern_segments *segments;

  xmalloc(&segments, sizeof(*segments));
  memcpy(&segments->loc, loc, sizeof(segments->loc));

  segments->qualifier_type = qualifier_type;
  segments->has_type_qualifiers = has_type_qualifiers;
  llist_init(&segments->list);
  llist_splice_head(&segments->list, list);
  llist_init(&segments->lln);

  *rv = segments;
}

void pattern_segments_list_free(llist * restrict list)
{
  pattern_segments * segments;
  llist *T;

  if(!list) {
    return;
  }

  llist_foreach_safe(list, segments, lln, T) {
    pattern_segments_free(segments);
  }
}

void pattern_segments_free(pattern_segments * restrict segments)
{
  if(!segments) {
    return;
  }

  pattern_segment_list_free(&segments->list);
  free(segments);
}

static const pattern_segments *qualifiers_get(const pattern_segments * restrict segments)
{
  const pattern_group *group;
  const pattern_segments *group_segments;

  group = llist_first(&segments->list, typeof(*group), lln);
  RUNTIME_ASSERT(group->type == PATTERN_GROUP);
  RUNTIME_ASSERT(group->has_type_qualifiers);

  RUNTIME_ASSERT(llist_count(&group->segments) == 1);
  group_segments = llist_first(&group->segments, typeof(*group_segments), lln);
  RUNTIME_ASSERT(group_segments->qualifier_type == PATTERN_QUALIFIER_TYPE_OR);

  return group_segments;
}

bool pattern_segments_type_qualifiers_match(const pattern_segments * restrict segments, const moria_vertex * restrict v)
{
  bool match;
  const pattern_qualifier *qualifier;
  const pattern_segments *qualifiers;

  qualifiers = qualifiers_get(segments);

  /* evaluate each qualifier in the group */
  match = false;
  llist_foreach(&qualifiers->list, qualifier, lln) {
    RUNTIME_ASSERT(qualifier->type == PATTERN_QUALIFIER);
    match |= pattern_qualifier_match(qualifier->qualifier_type, v);
  }

  /* overall match */
  if(segments->qualifier_type == PATTERN_QUALIFIER_TYPE_AND && !match) {
    return false;
  }
  if(segments->qualifier_type == PATTERN_QUALIFIER_TYPE_NOT && match) {
    return false;
  }

  return true;
}

bool pattern_segments_type_qualifiers_has(const pattern_segments * restrict segments, pattern_qualifier_type type)
{
  const pattern_qualifier *qualifier;
  const pattern_segments *qualifiers;

  qualifiers = qualifiers_get(segments);

  llist_foreach(&qualifiers->list, qualifier, lln) {
    RUNTIME_ASSERT(qualifier->type == PATTERN_QUALIFIER);
    if(qualifier->qualifier_type == type) {
      return true;
    }
  }

  return false;
}

size_t pattern_segments_cat(
    void * restrict dst
  , size_t sz
  , const bacon_value * restrict vars
  , const struct pattern_segments * restrict segments
)
{
  size_t z;
  pattern_segment *seg;

  z = 0;
  llist_foreach(&segments->list, seg, lln) {
    z += seg->vtab->cat(dst + z, sz - z, vars, seg);
  }

  return z;
}
