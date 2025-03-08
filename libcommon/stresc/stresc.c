/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "stresc.h"
#include "zcat.h"

size_t strescw(void * restrict dst, size_t sz, const char * const restrict src, size_t len)
{
  size_t z  = 0;

  int x;
  for(x = 0; x < len; x++)
  {
    if(src[x] == 0x09)
      z += zcats(dst + z, sz - z, "\\t");
    else if(src[x] == 0x0a)
      z += zcats(dst + z, sz - z, "\\n");
    else if(src[x] == 0x0d)
      z += zcats(dst + z, sz - z, "\\r");
    else if(src[x] >= 0x20 && src[x] <= 0x7e)
      z += zcatc(dst + z, sz - z, src[x]);
    else
      z += zcatf(dst + z, sz - z, "\\x%02hhx", src[x]);
  }

  return z;
}
