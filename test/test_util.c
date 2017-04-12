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

#define restrict __restrict

void ufailf_info(const char * const restrict expfmt, const char * const restrict actfmt, ...)
{
  va_list va;
  va_start(va, actfmt);

  xapi_info_pushvf("expected", expfmt, va);
  xapi_info_pushvf("actual", actfmt, va);

  va_end(va);
}

void ufails_info(const char * const restrict exp, const char * const restrict act)
{
  xapi_info_pushs("expected", exp);
  xapi_info_pushs("actual", act);
}
