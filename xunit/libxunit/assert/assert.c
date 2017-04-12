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

#include "internal.h"
#include "assert.internal.h"
#include "type.internal.h"

#include "zbuffer.h"

#define restrict __restrict

APIDATA uint32_t xunit_assertions_passed;
APIDATA uint32_t xunit_assertions_failed;

//
// API
//

API int xunit_assertion_evaluate(const xunit_type * const restrict type, uint8_t op, const char * const restrict value, ...)
{
  va_list va;
  va_start(va, value);

  xunit_arg expected;
  xunit_arg actual;

  int r;
  if(op == XU_NULL || op == XU_NOTNULL)
  {
    type->xu_unpack(va, &actual);
    r = actual.d;
  }
  else
  {
    type->xu_unpack(va, &expected);
    type->xu_unpack(va, &actual);
    r = type->xu_compare(&expected, &actual);
  }

  if(op == XU_EQ && r) { }
  else if(op == XU_LT && r >= 0) { }
  else if(op == XU_NULL && r) { }
  else if(op == XU_NOTNULL && !r) { }
  else
  {
    xunit_assertions_passed++;
    return 1;
  }

  xunit_assertions_failed++;
  xapi_info_pushs("value", value);
  if(op == XU_NULL || op == XU_NOTNULL)
  {
    xapi_info_pushs("expected", op == XU_NULL ? "(null)" : "(notnull");
    type->xu_info_add("actual", &actual);
  }
  else
  {
    type->xu_info_add("expected", &expected);
    type->xu_info_add("actual", &actual);
  }

  return 0;
}
