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

#include "xapi.h"
#include "xlinux/xstdlib.h"

#include "internal.h"
#include "pstring.internal.h"

#include "assure.h"
#include "strutil.h"

/*
 * Default capacity
 */
#define DEFAULT_CAPACITY 100

#define restrict __restrict

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
static xapi __attribute__((nonnull)) psassure(pstring * const restrict ps, size_t l)
{
  xproxy(assure, &ps->s, sizeof(*ps->s), l, &ps->a);
}

//
// api
//

API xapi pscreatex(pstring ** const restrict rv, size_t capacity)
{
  enter;

  pstring * ps = 0;
  fatal(xmalloc, &ps, sizeof(*ps));
  fatal(psassure, ps, capacity ?: DEFAULT_CAPACITY);

  *rv = ps;
  ps = 0;

finally:
  psfree(ps);
coda;
}

API xapi pscreate(pstring ** const restrict ps)
{
  xproxy(pscreatex, ps, 0);
}

// mk

API xapi psvmkf(pstring ** restrict ps, pstring * restrict e, const char * const restrict fmt, va_list va)
{
  enter;

  pstring * lp;
  if(!ps)
    ps = &lp;
  *ps = e;

  fatal(psvcatf, *ps, fmt, va);

  finally : coda;
}

API xapi psmkf(pstring ** const restrict ps, pstring * restrict e, const char * const restrict fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);
  fatal(psvmkf, ps, e, fmt, va);
  va_end(va);

  finally : coda;
}

API xapi psmks(pstring ** restrict ps, pstring * restrict e, const char * const restrict s)
{
  xproxy(psmkw, ps, e, s, strlen(s));
}

API xapi psmkw(pstring ** restrict ps, pstring * restrict e, const char * const restrict s, size_t l)
{
  enter;

  pstring * lp;
  if(!ps)
    ps = &lp;
  *ps = e;

  fatal(pscatw, *ps, s, l);

  finally : coda;
}

API xapi psmkc(pstring ** restrict ps, pstring * restrict e, int c)
{
  enter;

  pstring * lp;
  if(!ps)
    ps = &lp;
  *ps = e;

  fatal(pscatc, *ps, c);

  finally : coda;
}

API void psclear(pstring * restrict ps)
{
  ps->l = 0;
}

API void psfree(pstring * ps)
{
  if(ps)
  {
    wfree(ps->s);
  }
  wfree(ps);
}

API void psifree(pstring ** ps)
{
  psfree(*ps);
  *ps = 0;
}

// load

API xapi psloadvf(pstring * restrict ps, const char * const restrict fmt, va_list va)
{
  enter;

  va_list va2;
  va_copy(va2, va);

  // attempt to perform the entire write to the buffer
  int r = vsnprintf(ps->s, ps->a, fmt, va2);
  va_end(va2);

  // check whether output was truncated
  if(r >= ps->a)
  {
    fatal(psassure, ps, r + 1);
    vsprintf(ps->s, fmt, va);
  }
  ps->l = r;

finally:
  va_end(va);
coda;
}

API xapi psloadf(pstring * restrict ps, const char * const restrict fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);
  fatal(psloadvf, ps, fmt, va);

finally:
  va_end(va);
coda;
}

API xapi psloads(pstring * restrict ps, const char * const restrict s)
{
  xproxy(psloadw, ps, s, strlen(s));
}

API xapi psloadw(pstring * restrict ps, const char * const restrict s, size_t l)
{
  enter;

  fatal(psassure, ps, l + 1);
  memcpy(ps->s, s, l);
  ps->s[l] = 0;
  ps->l = l;

  finally : coda;
}

API xapi psloadc(pstring * restrict ps, int c)
{
  enter;

  fatal(psassure, ps, 1 + 1);
  ps->s[0] = c;
  ps->s[1] = 0;
  ps->l = 1;

  finally : coda;
}

// cat

API xapi psvcatf(pstring * restrict ps, const char * const restrict fmt, va_list va)
{
  enter;

  va_list va2;
  va_copy(va2, va);

  // attempt to perform the entire write to the buffer
  int r = vsnprintf(ps->s + ps->l, ps->a - ps->l, fmt, va2);
  va_end(va2);

  // check whether output was truncated
  if(r >= (ps->a - ps->l))
  {
    fatal(psassure, ps, ps->l + r + 1);
    vsprintf(ps->s + ps->l, fmt, va);
  }
  ps->l += r;

  finally : coda;
}

API xapi pscatf(pstring * restrict ps, const char * const restrict fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);
  fatal(psvcatf, ps, fmt, va);

finally:
  va_end(va);
coda;
}

API xapi pscats(pstring * restrict ps, const char * const restrict s)
{
  xproxy(pscatw, ps, s, strlen(s));
}

API xapi pscatw(pstring * restrict ps, const char * const restrict s, size_t l)
{
  enter;

  fatal(psassure, ps, ps->l + l + 1);
  memcpy(ps->s + ps->l, s, l);
  ps->s[ps->l + l] = 0;
  ps->l += l;

  finally : coda;
}

API xapi pscatc(pstring * restrict ps, int c)
{
  enter;

  fatal(psassure, ps, ps->l + 1 + 1);
  ps->s[ps->l] = c;
  ps->s[ps->l + 1] = 0;
  ps->l++;

  finally : coda;
}

API int pscmp(const pstring * const restrict A, const pstring * const restrict B)
{
  return estrcmp(A->s, A->l, B->s, B->l, 0);
}
