/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "segment.h"
#include "pattern.internal.h"

//
// internal
//

int pattern_segment_cmp(const pattern_segment * A, const pattern_segment * B)
{
  int d;

  if(!A && !B)
    return 0;

  if(!A)
    return -1;

  if(!B)
    return -1;

  if((d = INTCMP(A->type, B->type)))
    return d;

  return A->vtab->cmp(A, B);
}

int pattern_segments_cmp(const pattern_segments * A, const pattern_segments * B)
{
  const pattern_segment * a;
  const pattern_segment * b;
  int d;

  if(!A && !B)
    return 0;

  if(!A)
    return -1;

  if(!B)
    return 1;

  if((d = INTCMP(A->qualifier_type, B->qualifier_type)))
    return d;

  a = A->segment_head;
  b = B->segment_head;

  while(1)
  {
    if(!a && !b)
      return 0;

    if(!a)
      return -1;

    if(!b)
      return 1;

    if((d = pattern_segment_cmp(a, b)))
      return d;

    a = containerof(a->chn.next, typeof(*a), chn);
    if(a == A->segment_head)
      a = NULL;

    b = containerof(b->chn.next, typeof(*b), chn);
    if(b == B->segment_head)
      b = NULL;
  }
}
