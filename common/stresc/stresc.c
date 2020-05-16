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

#include "stresc.h"
#include "zbuffer.h"

size_t strescw(void * restrict dst, size_t sz, const char * const restrict src, size_t len)
{
  size_t z  = 0;

  int x;
  for(x = 0; x < len; x++)
  {
    if(src[x] == 0x09)
      z += znloads(dst + z, sz - z, "\\t");
    else if(src[x] == 0x0a)
      z += znloads(dst + z, sz - z, "\\n");
    else if(src[x] == 0x0d)
      z += znloads(dst + z, sz - z, "\\r");
    else if(src[x] >= 0x20 && src[x] <= 0x7e)
      z += znloadc(dst + z, sz - z, src[x]);
    else
      z += znloadf(dst + z, sz - z, "\\x%02hhx", src[x]);
  }

  return z;
}
