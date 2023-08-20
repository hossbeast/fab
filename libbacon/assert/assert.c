/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdarg.h>

#include "xunit/type.h"

#include "assert.h"
#include "value.h"

//
// types
//

static void value_unpack(va_list va, xunit_arg * a)
{
  a->p = va_arg(va, void*);
}

static int value_compare(xunit_arg * A, xunit_arg * B)
{
  return value_cmp(A->p, B->p);
}

static void value_info_push(const char * const restrict name, xunit_arg * a)
{
  char buf[512];
  size_t z;

  if(a->p == 0)
  {
    xunit_info_pushs(name, "-none-");
  }
  else
  {
    z = value_znload(buf, sizeof(buf), a->p);
    xunit_info_pushw(name, buf, z);
  }
}

//
// API
//

xunit_type * API value_xunit_value = (xunit_type[]) {{
    xu_unpack : value_unpack
  , xu_compare : value_compare
  , xu_info_push : value_info_push
}};
