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
 A narrator supports write operations, which are propagated to an underlying store

*/

#include <stdarg.h>
#include <sys/types.h>
#include <string.h>

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

/// narrator_mark
//
// SUMMARY
//  get a mark indicating position in the output
//
// PARAMETERS
//  mark - (returns) nonnegative position, if the narration supports marks, -1 otherwise
//
xapi narrator_mark(struct narrator * const restrict n, size_t * const restrict mark)
  __attribute__((nonnull));

/// narrator_first
//
// SUMMARY
//  get a pointer to the first record
//
const char * narrator_first(struct narrator * const restrict n)
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
