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

#if DEVEL

#include <stdarg.h>

#include "xapi.h"
#include "types.h"

#include "xunit/type.h"

#include "internal.h"
#include "assert.h"
#include "set.internal.h"

#include "stresc.h"
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
    xapi_info_pushs(name, "(null)");
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

  xapi_info_pushw(name, buf, bufz);
}

//
// API
//

APIDATA xunit_type * valyria_xunit_set = (xunit_type[]) {{
    xu_unpack : set_unpack
  , xu_compare : set_compare
  , xu_info_push : set_info_push
}};

#endif
