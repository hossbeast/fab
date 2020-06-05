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

#include <stdarg.h>

#include "xapi.h"
#include "types.h"

#include "xunit/type.h"

#include "value.h"
#include "value.h"
#include "assert.h"

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
  if(a->p == 0)
  {
    xapi_info_pushs(name, "-none-");
  }
  else
  {
    char buf[512];
    size_t __attribute__((unused)) z = value_znload(buf, sizeof(buf), a->p);

    xapi_info_pushw(name, buf, z);
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
