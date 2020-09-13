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

#ifndef _MACROS_H
#define _MACROS_H

#include <string.h>
#include <stddef.h>
#include <stdint.h>

#define QUOTE(x) #x
#define XQUOTE(x) QUOTE(x)

#define MIN(a,b)           \
 ({ typeof (a) _a = (a);   \
    typeof (b) _b = (b);   \
   _a > _b ? _b : _a; })

#define MAX(a,b)           \
 ({ typeof (a) _a = (a);   \
    typeof (b) _b = (b);   \
   _a > _b ? _a : _b; })

#define INTCMP(a, b)       \
 ({ typeof (a) _a = (a);   \
    typeof (b) _b = (b);   \
      _a > _b ? 1          \
    : _a < _b ? -1         \
    : 0; })

/// sentinel
//
// SUMMARY
//  counts the number of elements in an array terminated by a zeroed-entry
//
// RETURNS
//  size_t
//
#define sentinel(x) ({                                \
  typeof((x)[0]) * p = (x);                           \
  while(memcmp(p, (char[sizeof(*p)]){}, sizeof(*p)))  \
    p++;                                              \
  p - (x);                                            \
})

#define ARRAY_LEN(x) (sizeof((x)) / sizeof(*(x)))

/// NARGS
//
// SUMMARY
//  evaluates to the number of arguments passed to it
//
#define NARGS(...) ((sizeof((int[]){0, ##__VA_ARGS__}) / sizeof(int)) - 1)

/// MM - helper macro
//
// many map api's require an item and its length (get, set, delete)
//
// when the items size is known at compile time, MM makes it simpler to pass
//
#define MM(x) (void*)&(x), sizeof(x)
#define MMS(x) (x), strlen(x)

/// memncmp
//
// SUMMARY
//  compare two regions of memory
//
// PARAMETERS
//  a    - pointer to the first region
//  alen - size of the first region
//  b    - pointer to the second region
//  blen - size of the second region
//
// RETURNS
//  a la strcmp
//
static inline int memncmp(const void * a, size_t alen, const void * b, size_t blen)
{
  int r;
  size_t len;

  r = 0;
  len = MIN(alen, blen);
  if(len)
    r = memcmp(a, b, MIN(alen, blen));
  if(r != 0)
    return r;
  if(alen > blen)
    return 1;
  if(blen > alen)
    return -1;

  return 0;
}

static inline size_t memncpy(void *dst, size_t dst_sz, const void *src, size_t n)
{
  if(dst_sz < n) {
    return 0;
  }

  memcpy(dst, src, n);
  return n;
}

static inline int ptrcmp(const void *_A, const void *_B)
{
  const void * const * A = _A;
  const void * const * B = _B;

  return INTCMP(*A, *B);
}

/// roundup2
//
// round up to the next highest power of 2
//
static inline size_t roundup2(size_t x)
{
  x--;
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  x++;

  return x;
}

#define sizeof_member(s, m) sizeof(((s*)((void*)0))->m)

#define fieldof(type, member)         \
  typeof(((type*)0)->member) member;  \

#define ALIGNEDOF(A, B, member)  \
  STATIC_ASSERT(offsetof(A, member) == offsetof(B, member))

#define die_expr(expr) \
  char (*__kaboom)[expr] = 1

#define refas(expr, as) ({    \
  typeof(expr->as) * ptr = 0; \
  if(expr) {                  \
    ptr = &expr->as;          \
  }                           \
  ptr;                        \
})

#define refas2(expr, section, as) ({   \
  typeof(expr->section->as) * ptr = 0; \
  if(expr && expr->section) {          \
    ptr = &expr->section->as;          \
  }                                    \
  ptr;                                 \
})

#if DEBUG || DEVEL || XUNIT
#include <stdlib.h>
#include <stdio.h>
#define RUNTIME_ASSERT(x) do {                          \
  if (!(x)) {                                           \
    fprintf(stderr, "%s:%d failed runtime assert: %s\n", __FILE__, __LINE__, #x); \
    RUNTIME_ABORT();                                    \
  }                                                     \
} while(0)
#else
#define RUNTIME_ASSERT(x)
#endif

#if DEBUG || DEVEL || XUNIT
#define RUNTIME_ABORT() do {  \
  fprintf(stderr, "%s:%d aborting\n", __FILE__, __LINE__); \
  abort();  \
} while(0)
#else
#define RUNTIME_ABORT()
#endif

#define ffsll(x)   __builtin_ffsll(x)
#define clz_u32(x) __builtin_clz(x)

static inline uint32_t align(size_t x, size_t pow2)
{
  size_t r = x & (pow2 - 1);
  if(!r)
    return 0;

  return pow2 - r;
}

static inline uint32_t alignp(const void *x, uint32_t pow2)
{
  uintptr_t xp = (uintptr_t)x;
  uintptr_t r = xp & (pow2 - 1);

  if(!r)
    return 0;

  return pow2 - r;
}

#endif
