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

#include "narrator.h"
#include "value/writer.h"
#include "xlinux/xstdlib.h"
#include "valyria/list.h"
#include "valyria/set.h"

#include "segment.internal.h"
#include "pattern.internal.h"
#include "render.internal.h"
#include "match.internal.h"
#include "node.h"
#include "module.h"
#include "path.h"

#include "attrs.h"


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
