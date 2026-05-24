/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>

#include "narrator/fixed.h"
#include "bacon/builder.h"
#include "valyria/llist.h"

#include "pattern.internal.h"
#include "segment.h"

#include "common/attrs.h"

//
// internal
//

int pattern_segment_cmp(const pattern_segment * A, const pattern_segment * B)
{
  int d;

  if(!A)
    return -1;

  if(!B)
    return -1;

  if((d = INTCMP(A->type, B->type)))
    return d;

  return A->vtab->cmp(A, B);
}

void pattern_segment_free(pattern_segment * restrict segment)
{
  segment->vtab->destroy(segment);
  free(segment);
}

void pattern_segment_list_say(const llist * restrict list, narrator * restrict N)
{
  const pattern_segment *segment, *next;

  segment = llist_first(list, typeof(*segment), lln);
  while(segment) {
    next = llist_next(list, segment, lln);
    segment->vtab->say(segment, next, N);
    segment = next;
  }
}

void pattern_segment_list_describe(const llist * restrict list, struct bacon_writer * restrict builder_writer)
{
  const pattern_segment *segment;

  { LIST
    llist_foreach(list, segment, lln) {
      SET
      { MAPPING
        STRING("type")
        STRING(attrs16_name_byvalue(pattern_segment_type_attrs, segment->type))
      }
      segment->vtab->describe(segment, builder_writer);
    }
  }
}

void pattern_segment_list_free(llist * restrict list)
{
  pattern_segment *segment;
  llist *T;

  llist_foreach_safe(list, segment, lln, T) {
    pattern_segment_free(segment);
	}
}

void pattern_segment_init(pattern_segment * restrict n, const pattern_segment_vtable * restrict vtab, const yyu_location * restrict loc)
{
  n->vtab = vtab;
  n->type = vtab->type;
	memcpy(&n->loc, loc, sizeof(n->loc));
  llist_init(&n->lln);
}
