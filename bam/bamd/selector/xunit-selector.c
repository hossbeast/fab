/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <errno.h>

#include "common/attrs.h"
#include "moria/traverse.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "bacon/writer.h"
#include "xlinux/xstdlib.h"

#include "xunit/type.h"
#include "xunit/info.h"

#include "selector.internal.h"
#include "dependency.h"
#include "fsent.h"
#include "lookup.h"
#include "module.h"
#include "pattern.h"
#include "request.h"
#include "channel.h"
#include "zcat.h"
#include "system_state.h"
#include "events.h"


static int selector_cmp(selector * A, selector * B)
{
  int d;

  if(!A && !B)
    return 0;

  if(!A)
    return -1;

  if(!B)
    return 1;

  if((d = INTCMP(A->type, B->type)))
    return d;

  if(A->type == SELECTOR_PATTERN)
  {
    return pattern_cmp(A->pattern, B->pattern);
  }
  else if(A->type == SELECTOR_LOOKUP)
  {
    return 0
      ?: memncmp(A->path, A->path_len, B->path, B->path_len)
      ?: INTCMP(A->nofollow, B->nofollow)
      ;
  }
  else if(A->type == SELECTOR_TRAVERSE)
  {
    return 0
      ?: memcmp(&A->direction, &B->direction, sizeof(A->direction))
      ?: memcmp(&A->criteria, &B->criteria, sizeof(A->criteria))
      ;
  }

  else if(A->type == SELECTOR_NODESET)
  {
    return INTCMP(A->nodeset, B->nodeset);
  }
  else if(A->type == SELECTOR_SEQUENCE || A->type == SELECTOR_UNION)
  {
    selector * a = llist_first(&A->head, selector, lln);
    selector * b = llist_first(&B->head, selector, lln);

    while(a || b)
    {
      if((d = selector_cmp(a, b)))
        return d;

      a = llist_next(&A->head, a, lln);
      b = llist_next(&B->head, b, lln);
    }
  }
  else
  {
    RTA;
  }

  return 0;
}

static void selector_unpack(va_list va, xunit_arg * a)
{
  a->p = va_arg(va, void*);
}

static int selector_compare(xunit_arg * A, xunit_arg * B)
{
  return selector_cmp(A->p, B->p);
}

static void selector_info_push(const char * const restrict name, xunit_arg * a)
{
  selector * sel = a->p;

  if(sel == 0)
  {
    xunit_info_pushs(name, "-none-");
  }
  else
  {
    xunit_info_pushs(name, "-not-none-");
  }
}

xunit_type * xunit_selector = (xunit_type[]) {{
    xu_unpack : selector_unpack
  , xu_compare : selector_compare
  , xu_info_push : selector_info_push
}};
