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

#ifndef _NARRATOR_H
#define _NARRATOR_H

/*

LIBRARY
 narrator

SUMMARY
 A narrator supports write operations, which are propagated to an underlying output channel or store

*/

#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>

#include "xapi.h"

struct narrator;
typedef struct narrator narrator;

//
// write to the active narration
//

#define vsayf(...)       fatal(narrator_vsayf, N, ##__VA_ARGS__)
#define sayf(...)        fatal(narrator_sayf , N, ##__VA_ARGS__)
#define says(...)        fatal(narrator_says , N, ##__VA_ARGS__)
#define sayw(...)        fatal(narrator_sayw , N, ##__VA_ARGS__)
#define sayc(...)        fatal(narrator_sayc , N, ##__VA_ARGS__)

#define restrict __restrict

/// narrator_vsayf
//
// SUMMARY
//  formatted write from va_list to the specified narrator
//
xapi narrator_vsayf(struct narrator * const restrict n, const char * const restrict fmt, va_list va)
  __attribute__((nonnull));

/// narrator_sayf
//
// SUMMARY
//  formatted write to the specified narrator
//
xapi narrator_sayf(struct narrator * const restrict n, const char * const restrict fmt, ...)
  __attribute__((nonnull(1, 2)))
  __attribute__((format(printf, 2, 3)));

/// narrator_sayw
//
// SUMMARY
//  write to the specified narrator
//
xapi narrator_sayw(struct narrator * const restrict n, const char * const restrict b, size_t l)
  __attribute__((nonnull));

/// narrator_says
//
// SUMMARY
//  write to the specified narrator
//
xapi narrator_says(struct narrator * const restrict n, const char * const restrict s)
  __attribute__((nonnull));

/// narrator_sayc
//
// SUMMARY
//  write a byte to the specified narrator
//
xapi narrator_sayc(struct narrator * const restrict n, int c)
  __attribute__((nonnull));

#define NARRATOR_SEEK_TABLE(x)                                                        \
  NARRATOR_SEEK_DEF(SET , SEEK_SET  , x)  /* absolute offset */                       \
  NARRATOR_SEEK_DEF(CUR , SEEK_CUR  , x)  /* offset relative to current position */   \
  NARRATOR_SEEK_DEF(END , SEEK_END  , x)  /* offset relative to size of the store */

enum {
#define NARRATOR_SEEK_DEF(a, b, x) NARRATOR_SEEK_ ## a = b,
NARRATOR_SEEK_TABLE(0)
#undef NARRATOR_SEEK_DEF
};

/// narrator_seek
//
// SUMMARY
//  reposition the narrator to offset according to whence
//
// PARAMETERS
//  n      - narrator
//  offset - byte offset
//  whence - one of NARRATOR_SEEK_*, indicates how offset is interpreted
//  [res]  - (returns) the resulting absolute offset
//
xapi narrator_seek(struct narrator * const restrict n, off_t offset, int whence, off_t * restrict res)
  __attribute__((nonnull(1)));

/// narrator_reset
//
// SUMMARY
//  reposition the narrator such that subsequent writes start at the beginning
//
xapi narrator_reset(struct narrator * const restrict n)
  __attribute__((nonnull));

/// narrator_xfree
//
// SUMMARY
//  release resources associated with a narrator
//
// PARAMETERS
//  [n]  - pointer to a narrator, or a pointer to null
//
// REMARKS
//  sets *n = 0
//
xapi narrator_xfree(struct narrator * restrict n);

/// narrator_ixfree
//
// narrator_xfree(*n) ; *n = 0;
//
xapi narrator_ixfree(struct narrator ** const restrict n)
  __attribute__((nonnull));

/// read-only singleton narrators that write to fixed file descriptors
extern narrator * g_narrator_stdout;
extern narrator * g_narrator_stderr;

/// read-only singleton nullity narrator that discards all output
extern narrator * g_narrator_nullity;

#undef restrict
#endif
