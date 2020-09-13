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
static xapi __attribute__((nonnull)) psassure(pstring * const restrict ps, size_t l)
{
  xproxy(assure, &ps->s, sizeof(*ps->s), l, &ps->a);
}

//
// api
//

xapi API psinitx(pstring * restrict ps, size_t capacity)
{
  enter;

  fatal(psassure, ps, capacity ?: DEFAULT_CAPACITY);

  finally : coda;
}

xapi API psinit(pstring * restrict ps)
{
  enter;

  fatal(psassure, ps, DEFAULT_CAPACITY);

  finally : coda;
}

xapi API psdestroy(pstring * restrict ps)
{
  enter;

  wfree(ps->s);

  finally : coda;
}

xapi API pscreatex(pstring ** const restrict rv, size_t capacity)
{
  enter;

  pstring * ps = 0;

  fatal(xmalloc, &ps, sizeof(*ps));
  fatal(psinitx, ps, capacity);

  *rv = ps;
  ps = 0;

finally:
  psfree(ps);
coda;
}

xapi API pscreate(pstring ** const restrict ps)
{
  xproxy(pscreatex, ps, 0);
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

xapi API psvmkf(pstring ** restrict ps, pstring * restrict e, const char * const restrict fmt, va_list va)
{
  enter;

  pstring * lp;
  if(!ps)
    ps = &lp;
  *ps = e;

  fatal(psvcatf, *ps, fmt, va);

  finally : coda;
}

xapi API psmkf(pstring ** const restrict ps, pstring * restrict e, const char * const restrict fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);
  fatal(psvmkf, ps, e, fmt, va);
  va_end(va);

  finally : coda;
}

xapi API psmks(pstring ** restrict ps, pstring * restrict e, const char * const restrict s)
{
  xproxy(psmkw, ps, e, s, strlen(s));
}

xapi API psmkw(pstring ** restrict ps, pstring * restrict e, const char * const restrict s, size_t l)
{
  enter;

  pstring * lp;
  if(!ps)
    ps = &lp;
  *ps = e;

  fatal(pscatw, *ps, s, l);

  finally : coda;
}

xapi API psmkc(pstring ** restrict ps, pstring * restrict e, int c)
{
  enter;

  pstring * lp;
  if(!ps)
    ps = &lp;
  *ps = e;

  fatal(pscatc, *ps, c);

  finally : coda;
}

// load

xapi API psloadvf(pstring * restrict ps, const char * const restrict fmt, va_list va)
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
  ps->size = r;

finally:
  va_end(va);
coda;
}

xapi API psloadf(pstring * restrict ps, const char * const restrict fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);
  fatal(psloadvf, ps, fmt, va);

finally:
  va_end(va);
coda;
}

xapi API psloads(pstring * restrict ps, const char * const restrict s)
{
  xproxy(psloadw, ps, s, strlen(s));
}

xapi API psloadw(pstring * restrict ps, const char * const restrict s, size_t l)
{
  enter;

  fatal(psassure, ps, l + 1);
  memcpy(ps->s, s, l);
  ps->s[l] = 0;
  ps->size = l;

  finally : coda;
}

xapi API psloadc(pstring * restrict ps, int c)
{
  enter;

  fatal(psassure, ps, 1 + 1);
  ps->s[0] = c;
  ps->s[1] = 0;
  ps->size = 1;

  finally : coda;
}

// cat

xapi API psvcatf(pstring * restrict ps, const char * const restrict fmt, va_list va)
{
  enter;

  va_list va2;
  va_copy(va2, va);

  // attempt to perform the entire write to the buffer
  int r = vsnprintf(ps->s + ps->size, ps->a - ps->size, fmt, va2);
  va_end(va2);

  // check whether output was truncated
  if(r >= (ps->a - ps->size))
  {
    fatal(psassure, ps, ps->size + r + 1);
    vsprintf(ps->s + ps->size, fmt, va);
  }
  ps->size += r;

  finally : coda;
}

xapi API pscatf(pstring * restrict ps, const char * const restrict fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);
  fatal(psvcatf, ps, fmt, va);

finally:
  va_end(va);
coda;
}

xapi API pscats(pstring * restrict ps, const char * const restrict s)
{
  xproxy(pscatw, ps, s, strlen(s));
}

xapi API pscatw(pstring * restrict ps, const char * const restrict s, size_t l)
{
  enter;

  fatal(psassure, ps, ps->size + l + 1);
  memcpy(ps->s + ps->size, s, l);
  ps->s[ps->size + l] = 0;
  ps->size += l;

  finally : coda;
}

xapi API pscatc(pstring * restrict ps, int c)
{
  enter;

  fatal(psassure, ps, ps->size + 1 + 1);
  ps->s[ps->size] = c;
  ps->s[ps->size + 1] = 0;
  ps->size++;

  finally : coda;
}
