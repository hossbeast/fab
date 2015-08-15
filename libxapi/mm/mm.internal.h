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

#ifndef _XAPI_MM_INTERNAL_H
#define _XAPI_MM_INTERNAL_H

#include <sys/types.h>
#include <stdarg.h>

#include "mm.h"

// per-thread memblocks
extern __thread struct memblk mm_mb;

#define restrict __restrict

/// mm_teardown
//
// SUMMARY
//  release memory
//
void mm_teardown();

void wmalloc(void * restrict p, size_t sz)
  __attribute__((nonnull));

void wrealloc(void * restrict p, size_t es, size_t ec, size_t oec)
  __attribute__((nonnull));

void assure(void * restrict p, size_t * const restrict dstl, size_t * const restrict dsta, size_t z, size_t l)
  __attribute__((nonnull));

void sloadw(char ** const restrict dst, size_t * const restrict dstl, size_t * const restrict dsta, const char * const restrict s, size_t l)
  __attribute__((nonnull));

void svloadf(char ** const restrict dst, size_t * const restrict dstl, size_t * const restrict dsta, const char * const restrict fmt, va_list va)
  __attribute__((nonnull));

#undef restrict
#endif
