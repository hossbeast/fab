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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "xlinux/xstdlib.h"

#include "pstring.internal.h"

#include "common/assure.h"
#include "macros.h"

/*
 * Default capacity
 */
#define DEFAULT_CAPACITY 100

//
// static
//

/// psassure
//
// SUMMARY
//  ensure allocation matches or exceeds a specified threshold
//
// PARAMETERS
//  ps - pstring
//  l  - required size
//
static void __attribute__((nonnull)) psassure(pstring * const restrict ps, size_t l)
{
  assure(&ps->s, sizeof(*ps->s), l, &ps->a);
}

//
// api
//

void API psinitx(pstring * restrict ps, size_t capacity)
{
  psassure(ps, capacity ?: DEFAULT_CAPACITY);
}

void API psinit(pstring * restrict ps)
{
  psassure(ps, DEFAULT_CAPACITY);
}

void API psdestroy(pstring * restrict ps)
{
  wfree(ps->s);
}

void API pscreatex(pstring ** const restrict rv, size_t capacity)
{
  pstring * ps = 0;

  xmalloc(&ps, sizeof(*ps));
  psinitx(ps, capacity);

  *rv = ps;
}

void API pscreate(pstring ** const restrict ps)
{
  pscreatex(ps, 0);
}

void API psclear(pstring * restrict ps)
{
  ps->size = 0;
}

void API psfree(pstring * ps)
{
  if(ps)
  {
    wfree(ps->s);
  }
  wfree(ps);
}

void API psifree(pstring ** ps)
{
  psfree(*ps);
  *ps = 0;
}

int API pscmp(const pstring * const restrict A, const pstring * const restrict B)
{
  return memncmp(A->s, A->size, B->s, B->size);
}

// mk

void API psvmkf(pstring ** restrict ps, pstring * restrict e, const char * const restrict fmt, va_list va)
{
  psvcatf(*ps, fmt, va);
}

void API psmkf(pstring ** const restrict ps, pstring * restrict e, const char * const restrict fmt, ...)
{
  va_list va;
  va_start(va, fmt);

  psvmkf(ps, e, fmt, va);

  va_end(va);
}

void API psmks(pstring ** restrict ps, pstring * restrict e, const char * const restrict s)
{
  psmkw(ps, e, s, strlen(s));
}

void API psmkw(pstring ** restrict ps, pstring * restrict e, const char * const restrict s, size_t l)
{
  pscatw(*ps, s, l);
}

void API psmkc(pstring ** restrict ps, pstring * restrict e, int c)
{
  pscatc(*ps, c);
}

// load

void API psloadvf(pstring * restrict ps, const char * const restrict fmt, va_list va)
{
  va_list va2;
  va_copy(va2, va);

  // attempt to perform the entire write to the buffer
  int r = vsnprintf(ps->s, ps->a, fmt, va2);
  va_end(va2);

  // check whether output was truncated
  if(r >= ps->a)
  {
    psassure(ps, r + 1);
    vsprintf(ps->s, fmt, va);
  }
  ps->size = r;

  va_end(va);
}

void API psloadf(pstring * restrict ps, const char * const restrict fmt, ...)
{
  va_list va;
  va_start(va, fmt);

  psloadvf(ps, fmt, va);

  va_end(va);
}

void API psloads(pstring * restrict ps, const char * const restrict s)
{
  psloadw(ps, s, strlen(s));
}

void API psloadw(pstring * restrict ps, const char * const restrict s, size_t l)
{
  psassure(ps, l + 1);
  memcpy(ps->s, s, l);
  ps->s[l] = 0;
  ps->size = l;
}

void API psloadc(pstring * restrict ps, int c)
{
  psassure(ps, 1 + 1);
  ps->s[0] = c;
  ps->s[1] = 0;
  ps->size = 1;
}

// cat

void API psvcatf(pstring * restrict ps, const char * const restrict fmt, va_list va)
{
  va_list va2;
  va_copy(va2, va);

  // attempt to perform the entire write to the buffer
  int r = vsnprintf(ps->s + ps->size, ps->a - ps->size, fmt, va2);
  va_end(va2);

  // check whether output was truncated
  if(r >= (ps->a - ps->size))
  {
    psassure(ps, ps->size + r + 1);
    vsprintf(ps->s + ps->size, fmt, va);
  }
  ps->size += r;
}

void API pscatf(pstring * restrict ps, const char * const restrict fmt, ...)
{
  va_list va;
  va_start(va, fmt);

  psvcatf(ps, fmt, va);

  va_end(va);
}

void API pscats(pstring * restrict ps, const char * const restrict s)
{
  pscatw(ps, s, strlen(s));
}

void API pscatw(pstring * restrict ps, const char * const restrict s, size_t l)
{
  psassure(ps, ps->size + l + 1);
  memcpy(ps->s + ps->size, s, l);
  ps->s[ps->size + l] = 0;
  ps->size += l;
}

void API pscatc(pstring * restrict ps, int c)
{
  psassure(ps, ps->size + 1 + 1);
  ps->s[ps->size] = c;
  ps->s[ps->size + 1] = 0;
  ps->size++;
}
