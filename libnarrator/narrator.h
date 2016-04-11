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
#include <string.h>
#include <unistd.h>

#include "xapi.h"

//
// write to the active narration
//

#define vsayf(fmt, lst)  fatal(narrator_vsayf, N, fmt, lst)
#define sayf(fmt, ...)   fatal(narrator_sayf , N, fmt, ##__VA_ARGS__)
#define says(s)          fatal(narrator_sayw , N, s, strlen(s))
#define sayw(b, l)       fatal(narrator_sayw , N, b, l)
#define sayc(c)          fatal(narrator_sayc , N, c)

struct narrator;
typedef struct narrator narrator;

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
	__attribute__((nonnull(1, 2)));

/// narrator_sayw
//
// SUMMARY
//  write to the specified narrator
//
xapi narrator_sayw(struct narrator * const restrict n, char * const restrict b, size_t l)
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

/// narrator_free
//
// SUMMARY
//  free a narrator with free semantics
//
void narrator_free(struct narrator * restrict n);

/// narrator_xfree
//
// SUMMARY
//  free a narrator with xfree semantics
//
void narrator_xfree(struct narrator ** const restrict n)
  __attribute__((nonnull));

#undef restrict
#endif
