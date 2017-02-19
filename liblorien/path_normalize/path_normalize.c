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

#include "internal.h"
#include "path_normalize.h"

#include "zbuffer.h"

#define restrict __restrict

//
// api
//

API size_t path_normalize(char * restrict dst, size_t sz, const char * restrict s)
{
  size_t dst_len = 0;

  const char * A;
  const char * B;
  const char * backtrack = 0;

  int nofile = 0;
  int abs = 0;

  if(s[0] == '/')
    abs = 1;
  if(s[0] == '/' && s[1] == '.' && s[2] == '.')
    nofile = 1;

  for(A = B = s; *B; A = B + 1)
  {
    if((B = strchr(A, '/')) == 0)
      B = A + strlen(A);

    if((B - A) == 0)
    {
      // no-op
    }
    else if((B - A) == 1 && A[0] == '.')
    {
      // no-op
    }
    else if((B - A) == 2 && A[0] == '.' && A[1] == '.' && backtrack)
    {
      // separator following the segment
      const char * t = backtrack;

      // first character in the segment
      t--;
      while(t != s && *t != '/')
        t--;
      if(*t == '/')
        t++;

      dst_len -= (backtrack - t) + 1;

      if(dst_len == 4 && nofile)
      {
        dst_len -= 4;
      }
      else if(dst_len == 2 && !abs)
      {
        dst_len -= 2;
      }

      if(t != s)
        t--;

      // preceeding no-op text
      while(t != s && (*t == '/' || (*t == '.' && (t[-1] == '/' || (t - s) == 1))))
        t--;
      if(t != s)
        t++;

      if((backtrack = t) == s)
        backtrack = 0;
    }
    else
    {
      if(dst_len == 0)
      {
        if(nofile)
        {
          dst_len += znloads(dst + dst_len, sz - dst_len, "/..");
        }
        else if(!abs)
        {
          dst_len += znloads(dst + dst_len, sz - dst_len, ".");
        }
      }

      if((B - A) == 2 && A[0] == '.' && A[1] == '.')
      {
        if(!abs)
          dst_len += znloads(dst + dst_len, sz - dst_len, "/..");
      }
      else
      {
        dst_len += znloads(dst + dst_len, sz - dst_len, "/");
        dst_len += znloadw(dst + dst_len, sz - dst_len, A, B - A);

        // separator following the segment
        backtrack = B;
      }
    }
  }

  if(abs && dst_len == 0)
    dst_len += znloadc(dst + dst_len, sz - dst_len, '/');

  return dst_len;
}
