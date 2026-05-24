/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xunit/type.h"
#include "narrator/fixed.h"

#include "assert.h"
#include "value.h"

//
// types
//

static void bacon_value_unpack(va_list va, xunit_arg * a)
{
  a->p = va_arg(va, void*);
}

static int bacon_value_compare(xunit_arg * A, xunit_arg * B)
{
  return bacon_value_cmp(A->p, B->p);
}

static void bacon_value_info_push(const char * const restrict name, xunit_arg * a)
{
  char buf[512];
  narrator_fixed fixed;

  if(a->p == 0)
  {
    xunit_info_pushs(name, "-none-");
  }
  else
  {
    narrator_fixed_init(&fixed, buf, sizeof(buf));
    bacon_value_say(a->p, &fixed.base);
    xunit_info_pushw(name, buf, fixed.l);
  }
}

//
// API
//

xunit_type * bacon_xunit_value = (xunit_type[]) {{
    xu_unpack : bacon_value_unpack
  , xu_compare : bacon_value_compare
  , xu_info_push : bacon_value_info_push
}};
