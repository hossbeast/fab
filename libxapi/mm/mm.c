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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "internal.h"
#include "mm.internal.h"

#include "macros.h"
#include "memblk.def.h"

// per-thread storage
__thread struct memblk mm_mb;

#define restrict __restrict

//
// static
//

static void allocate(size_t sz)
{
  int __attribute__((unused)) _r;

#define MEMBLOCK_SMALL			0x1000 /* 1k : first THRESHOLD blocks */
#define MEMBLOCK_THRESHOLD	0x4			
#define MEMBLOCK_LARGE			0x8000 /* 8k : additional blocks */

	// request is too large to satisfy
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
			if((mb->blocks = calloc(sizeof(*mb->blocks) * ns, 1)) == 0)
				goto failed;
			mb->blocksa = ns;
		}

		if((mb->blocksl < MEMBLOCK_THRESHOLD) && (sz <= MEMBLOCK_SMALL))
			mb->blocks[mb->blocksl].a = MEMBLOCK_SMALL;
		else
			mb->blocks[mb->blocksl].a = MEMBLOCK_LARGE;

		if((mb->blocks[mb->blocksl].s = calloc(sizeof(*mb->blocks[0].s) * mb->blocks[mb->blocksl].a, 1)) == 0)
			goto failed;

		// cumulative offset
		if(mb->blocksl)
			mb->blocks[mb->blocksl].o = mb->blocks[mb->blocksl - 1].o + mb->blocks[mb->blocksl - 1].l;

		mb->blocksl++;
	}

	return;
failed:
  _r = write(2, MMS("libxapi failure to allocate while unwinding ; consider calling xapi_preallocate\n"));
}

//
// public
//

void wmalloc(void * restrict p, size_t sz)
{
  allocate(sz);

	struct memblk * mb = &mm_mb;

	*(void**)p = mb->blocks[mb->blocksl - 1].s + mb->blocks[mb->blocksl - 1].l;
	mb->blocks[mb->blocksl - 1].l += sz;
}

void wrealloc(void * restrict p, size_t es, size_t ec, size_t oec)
{
	void * old = *(void**)p;
	wmalloc(p, es * ec);
	if(old)
		memcpy(*(void**)p, old, es * oec);
}

void assure(void * restrict dst, size_t * const restrict dstl, size_t * const restrict dsta, size_t l)
{
	if(l > (*dsta))
	{
		size_t ns = (*dsta) ?: 10;
		while(ns <= l)
			ns = ns * 2 + ns / 2;
		wrealloc(dst, 1, ns, *dsta);
		(*dsta) = ns;
	}
}

void sloadw(char ** const restrict dst, size_t * const restrict dstl, size_t * const restrict dsta, const char * const restrict s, size_t l)
{
  assure(dst, dstl, dsta, l + 1);

	(*dstl) = l;
	memcpy(*dst, s, l);
	(*dst)[l] = 0;
}

void svloadf(char ** const restrict dst, size_t * const restrict dstl, size_t * const restrict dsta, const char * const restrict fmt, va_list va)
{
  // measure
  va_list va2;
  va_copy(va2, va);
  size_t l = vsnprintf(0, 0, fmt, va2);
  va_end(va2);

  assure(dst, dstl, dsta, l + 1);

  (*dstl) = l;
  vsprintf(*dst, fmt, va);
}

//
// api
//

API void xapi_allocate(size_t sz)
{
  allocate(sz);
}
