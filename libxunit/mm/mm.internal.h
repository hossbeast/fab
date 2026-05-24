/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

mm manages a per-thread allocation memblk that stores the per-thread calltree and staging infos

*/

#include <sys/types.h>
#include <stdarg.h>

// per-thread memblocks
extern __thread struct memblk mm_mb;

/// mm_teardown
//
// SUMMARY
//  release the memblk for this thread
//
void mm_teardown(void);

/// mm_malloc
//
// SUMMARY
//  xmalloc semantics
//
void mm_malloc(void * restrict p, size_t sz)
  __attribute__((nonnull));

/// wrealloc
//
// SUMMARY
//  xrealloc semantics
//
void mm_realloc(void * restrict p, size_t es, size_t ec, size_t oec)
  __attribute__((nonnull));

/// mm_assure
//
// SUMMARY
//
void mm_assure(void * restrict p, size_t * const restrict dstl, size_t * const restrict dsta, size_t z, size_t l)
  __attribute__((nonnull));

/// mm_sloadw
//
// SUMMARY
//  allocate storage for and copy a string, specified as a pointer/length pair
//
// PARAMETERS
//  dst  - (returns) string
//  dstl - (returns) string length
//  s    - string to copy from
//  [l]  - length of s, or 0 for strlen
//
void mm_sloadw(char ** const restrict dst, size_t * const restrict dstl, const char * const restrict s, size_t l)
  __attribute__((nonnull));

/// mm_sloadf
//
// SUMMARY
//  allocate storage for and copy a string, specified in printf style
//
// PARAMETERS
//  dst  - (returns) string
//  dstl - (returns) string length
//  fmt  - format stringn
//  va   - va_list
//
void mm_svloadf(char ** const restrict dst, size_t * const restrict dstl, const char * const restrict fmt, va_list va)
  __attribute__((nonnull));
