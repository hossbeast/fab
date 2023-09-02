/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>

#include "path_normalize.h"

#include "zcat.h"

//
// api
//

size_t path_normalize(char * restrict dst, size_t sz, const char * restrict s, uint16_t sl)
{
  size_t dst_len = 0;

  const char *A;
  const char *B;
  const char *backtrack = 0;
  const char *t;

  bool nofile = false;
  bool abs = false;

  if(sl > 0 && s[0] == '/') {
    abs = true;
  }
  if(sl > 2 && s[0] == '/' && s[1] == '.' && s[2] == '.') {
    nofile = true;
  }

  for(A = B = s; (B - s) < sl; A = B + 1)
  {
    /* find the next slash */
    for(B = A; (B - s) < sl && *B != '/'; B++);

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
      t = backtrack;

      // first character in the segment
      t--;
      while(t != s && *t != '/') {
        t--;
      }
      if(*t == '/') {
        t++;
      }

      dst_len -= (backtrack - t) + 1;

      if(dst_len == 4 && nofile) {
        dst_len -= 4;
      } else if(dst_len == 2 && !abs) {
        dst_len -= 2;
      }

      if(t != s) {
        t--;
      }

      // preceeding no-op text
      while(t != s && (*t == '/' || (*t == '.' && (t[-1] == '/' || (t - s) == 1)))) {
        t--;
      }
      if(t != s) {
        t++;
      }

      if((backtrack = t) == s) {
        backtrack = 0;
      }
    }
    else
    {
      if(dst_len == 0)
      {
        if(nofile)
        {
          dst_len += zcats(dst + dst_len, sz - dst_len, "/..");
        }
        else if(!abs)
        {
          dst_len += zcats(dst + dst_len, sz - dst_len, ".");
        }
      }

      if((B - A) == 2 && A[0] == '.' && A[1] == '.')
      {
        if(!abs)
          dst_len += zcats(dst + dst_len, sz - dst_len, "/..");
      }
      else
      {
        dst_len += zcats(dst + dst_len, sz - dst_len, "/");
        dst_len += zcatw(dst + dst_len, sz - dst_len, A, B - A);

        // separator following the segment
        backtrack = B;
      }
    }
  }

  if(abs && dst_len == 0) {
    dst_len += zcatc(dst + dst_len, sz - dst_len, '/');
  }

  dst[dst_len] = 0;

  return dst_len;
}
