/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"

#include "assure.h"

#define DEFAULT_SEED 10

void assure(void * target, size_t es, size_t len, size_t * const restrict ac)
{
  assurex(target, es, len, ac, DEFAULT_SEED);
}

void assurex(void * target, size_t es, size_t len, size_t * const restrict ac, size_t seed)
{
  size_t nc;
  void ** p;

  p = (void**)target;
  if(!*p || len > *ac)
  {
    nc = *ac ?: seed;
    while(nc < len) {
      nc = nc * 2 + nc / 2;
    }

    xrealloc(p, es, nc, *ac);
    *ac = nc;
  }
}
