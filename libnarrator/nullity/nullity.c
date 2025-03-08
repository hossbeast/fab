/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"

#include "nullity.internal.h"
#include "vtable.h"

#include "macros.h"

narrator * g_narrator_nullity = 0;

//
// static
//

static int __attribute__((nonnull)) nullity_sayvf(narrator * const restrict N, const char * const restrict fmt, va_list va)
{
  return 0;
}

static int __attribute__((nonnull)) nullity_sayw(narrator * const restrict N, const void * const restrict b, size_t l)
{
  return 0;
}

static void __attribute__((nonnull)) nullity_seek(narrator * const restrict N, off_t offset, int whence, off_t * restrict res)
{

}

static void __attribute__((nonnull)) nullity_destroy(narrator * const restrict N)
{

}

static void __attribute__((nonnull)) nullity_read(narrator * restrict N, void * dst, size_t count, size_t * restrict r)
{

}

static void __attribute__((nonnull)) nullity_flush(narrator * restrict N)
{

}

static struct narrator_vtable nullity_vtable = NARRATOR_VTABLE(nullity);

//
// public
//

void nullity_setup()
{
  narrator_nullity *n;

  narrator_nullity_create(&n);
  g_narrator_nullity = &n->base;
}

void nullity_cleanup()
{
  if(g_narrator_nullity) {
    narrator_nullity_free(containerof(g_narrator_nullity, narrator_nullity, base));
  }
}

//
// api
//

void narrator_nullity_create(narrator_nullity ** const restrict rv)
{
  narrator_nullity *n;

  xmalloc(&n, sizeof(*n));
  n->base.vtab = &nullity_vtable;
  n->base.off = 0;

  *rv = n;
}

void narrator_nullity_free(narrator_nullity * restrict n)
{
  nullity_destroy(&n->base);
  free(n);
}
