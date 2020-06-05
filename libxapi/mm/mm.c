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
#include <unistd.h>
#include <stdio.h>

#include "types.h"

#include "internal.h"
#include "mm.internal.h"

#include "macros.h"
#include "memblk.def.h"

// per-thread storage
__thread struct memblk mm_mb;

/// assure
//
// SUMMARY
//  ensures that the top block has enough space to satisfy an allocation of the specified size
//
static void assure(size_t sz)
{
  int __attribute__((unused)) _r;

#define MEMBLOCK_SMALL      0x1000 /* 1k : first THRESHOLD blocks */
#define MEMBLOCK_THRESHOLD  0x4
#define MEMBLOCK_LARGE      0x8000 /* 8k : additional blocks */

  // request is too large to satisfy
  // request cannot fit in a single block ; too large to satisfy
  if(sz > MEMBLOCK_LARGE)
    goto failed;

  struct memblk * mb = &mm_mb;

  /* current block is full */
  if(mb->blocksl == 0 || ((mb->blocks[mb->blocksl - 1].l + sz) > mb->blocks[mb->blocksl - 1].a))
  {
    /* reallocate the block container */
    if(mb->blocksl == mb->blocksa)
    {
      size_t ns = mb->blocksa ?: 3;
      ns = ns * 2 + ns / 2;
      if((mb->blocks = realloc(mb->blocks, sizeof(*mb->blocks) * ns)) == 0)
        goto failed;

      // zero out the new section
      memset(
          mb->blocks + mb->blocksa
        , 0
        , (ns - mb->blocksa) * sizeof(*mb->blocks)
      );

      mb->blocksa = ns;
    }

    typeof(mb->blocks[0]) * block = &mb->blocks[mb->blocksl];
    block->l = 0;
    block->a = 0;
    block->o = 0;

    /* reallocate the block */
    size_t ns = MEMBLOCK_LARGE;
    if((mb->blocksl < MEMBLOCK_THRESHOLD) && (sz <= MEMBLOCK_SMALL))
      block->a = MEMBLOCK_SMALL;

    if((block->s = realloc(block->s, sizeof(*block->s) * ns)) == 0)
      goto failed;

    block->a = ns;

    // cumulative offset
    if(mb->blocksl)
      block->o = mb->blocks[mb->blocksl - 1].o + mb->blocks[mb->blocksl - 1].l;

    memset(block->s, 0, sizeof(*block->s) * block->a);
    mb->blocksl++;
  }

  return;
failed:
  _r = write(2, MMS("libxapi failure to allocate while unwinding ; consider calling xapi_allocate\n"));
}

//
// public
//

void mm_teardown()
{
  struct memblk * mb = &mm_mb;

  int x;
  for(x = 0; x < mb->blocksl; x++)
    free(mb->blocks[x].s);

  free(mb->blocks);
  mb->blocks = 0;
  mb->blocksl = 0;
  mb->blocksa = 0;
}

void mm_malloc(void * restrict p, size_t sz)
{
  assure(sz);

  struct memblk * mb = &mm_mb;

  // avoid unaligned accesses by requiring a minimum object size
  sz = roundup(sz, sizeof(mb->blocks[0]));

  // allocate from the block
  *(void**)p = mb->blocks[mb->blocksl - 1].s + mb->blocks[mb->blocksl - 1].l;
  mb->blocks[mb->blocksl - 1].l += sz;
}

void mm_realloc(void * restrict p, size_t es, size_t ec, size_t oec)
{
  void * old = *(void**)p;
  mm_malloc(p, es * ec);
  if(old)
    memcpy(*(void**)p, old, es * oec);
}

void mm_assure(void * restrict dst, size_t * const restrict dstl, size_t * const restrict dsta, size_t z, size_t l)
{
  if(l > (*dsta))
  {
    size_t ns = (*dsta) ?: 10;
    while(ns <= l)
      ns = ns * 2 + ns / 2;
    mm_realloc(dst, z, ns, *dsta);
    (*dsta) = ns;
  }
}

void mm_sloadw(char ** const restrict dst, size_t * const restrict dstl, const char * const restrict s, size_t l)
{
  mm_malloc(dst, l + 1);

  (*dstl) = l;
  memcpy(*dst, s, l);
  (*dst)[l] = 0;
}

void mm_svloadf(char ** const restrict dst, size_t * const restrict dstl, const char * const restrict fmt, va_list va)
{
  va_list va2;
  size_t l;

  // measure
  va_copy(va2, va);
  l = vsnprintf(0, 0, fmt, va2);
  va_end(va2);

  // allocate
  mm_malloc(dst, l + 1);

  // copy
  (*dstl) = l;
  vsprintf(*dst, fmt, va);
}

//
// api
//

#if XAPI_STACKTRACE
void API xapi_allocate(size_t sz)
{
  assure(sz);
}
#endif
