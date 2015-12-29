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
#include <string.h>

#include "search.h"

#define restrict __restrict

int search_r(const void * const restrict key, const void * const restrict base, size_t nmemb, size_t size, int (* compar)(const void *, const void *, void *), void * arg, void * restrict el)
{
  void * lel;

  if(el == 0)
    el = &lel;

  size_t lo = 0;
  size_t hi = nmemb;
  while(hi - lo)
  {
    size_t mid = lo + ((hi - lo) / 2);

    *(void**)el = ((char*)base) + (size * mid);
    int r = compar(key, *(void**)el, arg);
    if(r == 0)
    {
      return 1;
    }
    else if(r < 0)
    {
      hi = mid;
    }
    else // r > 0
    {
      lo = mid + 1;
    }
  }

  return 0;
}
