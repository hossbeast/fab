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

#include <stdio.h>

#include "fmt.h"

//
// public
//

void API fmt_apply(char * const restrict dst, size_t dst_size, const char * const restrict fmt, va_list va)
{
  int rv;

  rv = vsnprintf(dst, dst_size, fmt, va);
  if(rv < 0 || rv >= dst_size)
  {
    RUNTIME_ABORT();
  }
}
