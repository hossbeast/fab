/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>

#include "common/assure.h"
#include "pstring.h"

void psinitx(char ** restrict ps, size_t * restrict len, size_t * restrict a, size_t capacity)
{
  *ps = 0;
  *len = 0;
  *a = 0;
  assure(ps, 1, capacity, a);
}

void psinit(char ** restrict ps, size_t * restrict len, size_t * restrict a)
{
  psinitx(ps, len, a, 128);
}

void psdestroy(char ** restrict ps, size_t * restrict len, size_t * restrict a)
{
  free(*ps);
  *ps = 0;
}

void psclear(char ** restrict ps, size_t * restrict len, size_t * restrict a)
{
  *len = 0;
}

//
// load
//

void psloadvf(char ** restrict ps, size_t * restrict len, size_t * restrict a, const char * const restrict fmt, va_list va)
{
  va_list va2;
  int r;

  va_copy(va2, va);

  // attempt to perform the entire write to the buffer
  r = vsnprintf(*ps, *a, fmt, va2);
  va_end(va2);

  // check whether output was truncated
  if(r >= *a)
  {
    assure(ps, 1, r + 1, a);
    vsprintf(*ps, fmt, va);
  }
  *len = r;
}

void psloadf(char ** restrict ps, size_t * restrict len, size_t * restrict a, const char * const restrict fmt, ...)
{
  va_list va;

  va_start(va, fmt);
  psloadvf(ps, len, a, fmt, va);
  va_end(va);
}

void psloads(char ** restrict ps, size_t * restrict len, size_t * restrict a, const char * const restrict s)
{
  psloadw(ps, len, a, s, strlen(s));
}

void psloadw(char ** restrict ps, size_t * restrict len, size_t * restrict a, const char * const restrict s, size_t l)
{
  assure(ps, 1, l + 1, a);
  memcpy(*ps, s, l);
  (*ps)[l] = 0;
  *len = l;
}

void psloadc(char ** restrict ps, size_t * restrict len, size_t * restrict a, int c)
{
  assure(ps, 1, 2, a);
  (*ps)[0] = c;
  (*ps)[1] = 0;
  *len = 1;
}

//
// cat
//

void pscatvf(char ** restrict ps, size_t * restrict len, size_t * restrict a, const char * const restrict fmt, va_list va)
{
  va_list va2;
  int r;

  va_copy(va2, va);

  // attempt to perform the entire write to the buffer
  r = vsnprintf(*ps + *len, *a - *len, fmt, va2);
  va_end(va2);

  // check whether output was truncated
  if(r >= (*a - *len))
  {
    assure(ps, 1, *len + r + 1, a);
    vsprintf(*ps + *len, fmt, va);
  }
  *len += r;
}

void pscatf(char ** restrict ps, size_t * restrict len, size_t * restrict a, const char * const restrict fmt, ...)
{
  va_list va;

  va_start(va, fmt);
  pscatvf(ps, len, a, fmt, va);
  va_end(va);
}

void pscats(char ** restrict ps, size_t * restrict len, size_t * restrict a, const char * const restrict s)
{
  pscatw(ps, len, a, s, strlen(s));
}

void pscatw(char ** restrict ps, size_t * restrict len, size_t * restrict a, const char * const restrict b, size_t l)
{
  assure(ps, 1, *len + l + 1, a);
  memcpy((*ps) + *len, b, l);
  (*ps)[*len + l] = 0;
  *len += l;
}

void pscatc(char ** restrict ps, size_t * restrict len, size_t * restrict a, int c)
{
  assure(ps, 1, *len + 2, a);
  (*ps)[*len] = c;
  (*ps)[*len + 1] = 0;
  (*len)++;
}
