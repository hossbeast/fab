/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdarg.h>

#include "types.h"

#include "xunit/type.h"

#include "assert.h"
#include "set.internal.h"

#include "common/stresc.h"
#include "zbuffer.h"

//
// types
//

static void set_unpack(va_list va, xunit_arg * a)
{
  a->p = va_arg(va, void*);
}

static int set_compare(xunit_arg * A, xunit_arg * B)
{
  return !set_equal(A->p, B->p);
}

static void set_info_push(const char * const restrict name, xunit_arg * a)
{
  if(a->p == 0)
  {
    xunit_info_pushs(name, "(null)");
    return;
  }

  set * ht = a->p;
  char buf[256];
  size_t bufsz = sizeof(buf);
  size_t bufz = 0;

  bufz += znloads(buf + bufz, bufsz - bufz, "{");

  int x;
  for(x = 0; x < ht->table_size; x++)
  {
    const void * e;
    size_t el;
    if(!set_table_element(ht, x, &e, &el))
      continue;

    if(bufz > 1)
      bufz += znloads(buf + bufz, bufsz - bufz, " ");

    bufz += strescw(buf + bufz, bufsz - bufz, e, el);
  }

  bufz += znloads(buf + bufz, bufsz - bufz, "}");

  xunit_info_pushw(name, buf, bufz);
}

//
// API
//

xunit_type * APIDATA valyria_xunit_set = (xunit_type[]) {{
    xu_unpack : set_unpack
  , xu_compare : set_compare
  , xu_info_push : set_info_push
}};
