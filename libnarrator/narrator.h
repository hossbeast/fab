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
#include "types.h"

struct narrator;
struct narrator_vtable;

typedef struct narrator {
  const struct narrator_vtable *vtab;
} narrator;

//
// fatal-write to the active narration
//

#define xsayvf(...) fatal(narrator_xsayvf, N, ##__VA_ARGS__)
#define xsayf(...)  fatal(narrator_xsayf , N, ##__VA_ARGS__)
#define xsays(...)  fatal(narrator_xsays , N, ##__VA_ARGS__)
#define xsayw(...)  fatal(narrator_xsayw , N, ##__VA_ARGS__)
#define xsayc(...)  fatal(narrator_xsayc , N, ##__VA_ARGS__)

/// read-only singleton narrators that write to fixed file descriptors
extern narrator * g_narrator_stdout;
extern narrator * g_narrator_stderr;

/// read-only singleton nullity narrator that discards all output
extern narrator * g_narrator_nullity;

/// narrator_sayvf
//
// SUMMARY
//  formatted write from va_list to the specified narrator
//
xapi narrator_xsayvf(narrator * const restrict n, const char * const restrict fmt, va_list va)
  __attribute__((nonnull));

/// narrator_sayf
//
// SUMMARY
//  formatted write to the specified narrator
//
xapi narrator_xsayf(narrator * const restrict n, const char * const restrict fmt, ...)
  __attribute__((nonnull(1, 2)))
  __attribute__((format(printf, 2, 3)));

/// narrator_sayw
//
// SUMMARY
//  write to the specified narrator
//
xapi narrator_xsayw(narrator * const restrict n, const void * const restrict b, size_t l);

/// narrator_says
//
// SUMMARY
//  write to the specified narrator
//
xapi narrator_xsays(narrator * const restrict n, const char * const restrict s)
  __attribute__((nonnull(1)));

/// narrator_sayc
//
// SUMMARY
//  write a byte to the specified narrator
//
xapi narrator_xsayc(narrator * const restrict n, int c)
  __attribute__((nonnull));

/// narrator_flush
//
// SUMMARY
//  flush
//
xapi narrator_flush(narrator * const restrict n)
  __attribute__((nonnull));

#define NARRATOR_SEEK_TABLE(x)                                                                      \
  NARRATOR_SEEK_DEF(NARRATOR_SEEK_SET , SEEK_SET  , x)  /* absolute offset */                       \
  NARRATOR_SEEK_DEF(NARRATOR_SEEK_CUR , SEEK_CUR  , x)  /* offset relative to current position */   \
  NARRATOR_SEEK_DEF(NARRATOR_SEEK_END , SEEK_END  , x)  /* offset relative to size of the store */

enum {
#define NARRATOR_SEEK_DEF(a, b, x) a = b,
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
xapi narrator_xseek(narrator * const restrict n, off_t offset, int whence, off_t * restrict res)
  __attribute__((nonnull(1)));

/// narrator_reset
//
// SUMMARY
//  reposition the narrator such that subsequent writes start at the beginning
//
xapi narrator_xreset(narrator * const restrict n)
  __attribute__((nonnull));

/// narrator_read
//
// SUMMARY
//  read bytes from the underlying store
//
// PARAMETERS
//  n     - narrator
//  dst   - destination buffer
//  count - number of bytes to read
//  [r]   - (returns) number of bytes read
//
// REMARKS
//  not supported for many narrator types
//
xapi narrator_xread(narrator * restrict n, void * dst, size_t count, size_t * restrict r)
  __attribute__((nonnull(1, 2)));

#endif
