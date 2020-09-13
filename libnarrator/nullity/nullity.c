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

#include "xapi.h"
#include "xlinux/xstdlib.h"

#include "nullity.internal.h"
#include "vtable.h"

#include "macros.h"

narrator * APIDATA g_narrator_nullity = 0;

//
// static
//

static xapi __attribute__((nonnull)) nullity_sayvf(narrator * const restrict N, const char * const restrict fmt, va_list va)
{
  enter;

  finally : coda;
}

static xapi __attribute__((nonnull)) nullity_sayw(narrator * const restrict N, const void * const restrict b, size_t l)
{
  enter;

  finally : coda;
}

static xapi __attribute__((nonnull)) nullity_seek(narrator * const restrict N, off_t offset, int whence, off_t * restrict res)
{
  enter;

  finally : coda;
}

static xapi __attribute__((nonnull)) nullity_destroy(narrator * const restrict N)
{
  enter;

  finally : coda;
}

static xapi __attribute__((nonnull)) nullity_read(narrator * restrict N, void * dst, size_t count, size_t * restrict r)
{
  enter;

  finally : coda;
}

static xapi __attribute__((nonnull)) nullity_flush(narrator * restrict N)
{
  enter;

  finally : coda;
}

static struct narrator_vtable nullity_vtable = NARRATOR_VTABLE(nullity);

//
// public
//

xapi nullity_setup()
{
  enter;

  narrator_nullity *n;

  fatal(narrator_nullity_create, &n);
  g_narrator_nullity = &n->base;

  finally : coda;
}

xapi nullity_cleanup()
{
  enter;

  if(g_narrator_nullity) {
    fatal(narrator_nullity_free, containerof(g_narrator_nullity, narrator_nullity, base));
  }

  finally : coda;
}

//
// api
//

xapi API narrator_nullity_create(narrator_nullity ** const restrict rv)
{
  enter;

  narrator_nullity *n;

  fatal(xmalloc, &n, sizeof(*n));
  n->base.vtab = &nullity_vtable;

  *rv = n;

  finally : coda;
}

xapi API narrator_nullity_free(narrator_nullity * restrict n)
{
  enter;

  fatal(nullity_destroy, &n->base);
  wfree(n);

  finally : coda;
}
