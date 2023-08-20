/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>
#include <stdio.h>

#include "xlinux/xstdlib.h"

#include "strutil.internal.h"

//
// api
//


void API strloadc(char ** restrict dst, int c)
{
  strloadw(dst, (char[]) { c }, 1);
}

void API strloads(char ** restrict dst, const char * restrict s)
{
  strloadw(dst, s, strlen(s));
}

void API strloadw(char ** restrict dst, const void * restrict buf, size_t bufl)
{
  free(*dst);
  xmalloc(dst, bufl + 1);
  memcpy(*dst, buf, bufl);
  (*dst)[bufl] = 0;
}

void API strloadf(char ** restrict dst, const char * restrict fmt, ...)
{
  va_list va;
  va_start(va, fmt);

  strloadvf(dst, fmt, va);

  va_end(va);
}

void API strloadvf(char ** restrict dst, const char * restrict fmt, va_list va)
{
  va_list va2;
  va_copy(va2, va);

  int r = vsnprintf(0, 0, fmt, va2);
  va_end(va2);

  free(*dst);
  xmalloc(dst, r + 1);
  vsprintf(*dst, fmt, va);

  va_end(va);
}
