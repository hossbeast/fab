/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "stresc.h"
#include "zbuffer.h"

size_t API strescw(void * restrict dst, size_t sz, const char * const restrict src, size_t len)
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
