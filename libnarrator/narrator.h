/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

LIBRARY
 narrator

SUMMARY
 A narrator supports write operations, which are propagated to an underlying output channel or store

*/

#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>

#include "types.h"

struct narrator;
struct narrator_vtable;

typedef struct narrator {
  const struct narrator_vtable *vtab;
} narrator;

//
// write to the active narration
//

#define xsayvf(...) narrator_sayvf(N, ##__VA_ARGS__)
#define xsayf(...)  narrator_sayf( N, ##__VA_ARGS__)
#define xsays(...)  narrator_says( N, ##__VA_ARGS__)
#define xsayw(...)  narrator_sayw( N, ##__VA_ARGS__)
#define xsayc(...)  narrator_sayc( N, ##__VA_ARGS__)

/// read-only singleton narrators that write to fixed file descriptors
extern narrator * g_narrator_stdout;
extern narrator * g_narrator_stderr;

/// read-only singleton nullity narrator that discards all output
extern narrator * g_narrator_nullity;

/* formatted write from va_list to the specified narrator */
void narrator_sayvf(narrator * const restrict n, const char * const restrict fmt, va_list va)
  __attribute__((nonnull));

/* formatted write to the specified narrator */
void narrator_sayf(narrator * const restrict n, const char * const restrict fmt, ...)
  __attribute__((nonnull(1, 2)))
  __attribute__((format(printf, 2, 3)));

/* write to the specified narrator */
void narrator_sayw(narrator * const restrict n, const void * const restrict b, size_t l);

/* write to the specified narrator */
void narrator_says(narrator * const restrict n, const char * const restrict s)
  __attribute__((nonnull(1)));

/* write a byte to the specified narrator */
void narrator_sayc(narrator * const restrict n, int c)
  __attribute__((nonnull));

/* flush */
void narrator_flush(narrator * const restrict n)
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

/*
 * reposition the narrator to offset according to whence
 *
 *  n      - narrator
 *  offset - byte offset
 *  whence - one of NARRATOR_SEEK_*, indicates how offset is interpreted
 *  [res]  - (returns) the resulting absolute offset
 */
void narrator_seek(narrator * const restrict n, off_t offset, int whence, off_t * restrict res)
  __attribute__((nonnull(1)));

/* reposition the narrator such that subsequent writes start at the beginning */
void narrator_reset(narrator * const restrict n)
  __attribute__((nonnull));

/*
 * read bytes from the underlying store. not supported for many narrator types.
 *
 * n     - narrator
 * dst   - destination buffer
 * count - number of bytes to read
 * [r]   - (returns) number of bytes read
 */
void narrator_read(narrator * restrict n, void * dst, size_t count, size_t * restrict r)
  __attribute__((nonnull(1, 2)));
