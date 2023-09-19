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

#include "test_util.h"

uint32_t assertions_passed;
uint32_t assertions_failed;

void ufailf_info(const char * restrict value, const char * restrict expfmt, const char * restrict actfmt, ...)
{
  va_list va;
  va_start(va, actfmt);
//
//  xapi_info_inserts (0, "value", value);
//  xapi_info_insertvf(1, "expected", expfmt, va);
//  xapi_info_insertvf(2, "actual", actfmt, va);
//

  fprintf(stderr, "%s : expected ", value);
  vfprintf(stderr, expfmt, va);
  fprintf(stderr, " actual ");
  vfprintf(stderr, actfmt, va);
  fprintf(stderr, "\n");

  va_end(va);
}

void ufails_info(const char * restrict value, const char * restrict exp, const char * restrict act)
{
  //xapi_info_inserts(0, "actual", act);
  //xapi_info_inserts(0, "expected", exp);
  //xapi_info_inserts(0, "value", value);

  fprintf(stderr, "%s : expected %s actual %s\n", value, exp, act);
}
