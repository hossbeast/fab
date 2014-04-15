/* Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>

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

#ifndef _LISTWISE_LSTACK_H
#define _LISTWISE_LSTACK_H

#include "listwise.h"

#define restrict __restrict

/// lstack_readrow
//
// SUMMARY
//  read the contents of an lstack row
//
// PARAMETERS
//  lx    - lw context
//  x     - list offset
//  y     - row offset
//  [r]   - return value - ptr
//  [rl]  - return value - length
//  [rt]  - return value - type
//  [obj] - whether object references are resolved
//  [win] - whether windowing is resolved
//  [str] - whether the return value is required to be null-terminated and in tmp space
//  [raw] - indicates whether the raw storage was returned (object entry not resolved, window not resolved, and str was not specified)
//
int lstack_readrow(lwx * const restrict lx, int x, int y, char ** const restrict r, int * const restrict rl, uint8_t * const restrict rt, int obj, int win, int str, int * const restrict raw)
	__attribute__((nonnull(1)));

/// lstack_append
//
// append text to the entry at x:y
//
int lstack_append(lwx * const restrict lx, int x, int y, const char* const restrict s, int l)
	__attribute__((nonnull));

/// lstack_appendf
//
// append text to the entry at x:y using printf-style args
//
int lstack_appendf(lwx * const restrict lx, int x, int y, const char* const restrict s, ...)
	__attribute__((nonnull));

/// lstack_write
//
// write text to the entry at x:y
//
int lstack_write(lwx * const restrict lx, int x, int y, const char* const restrict s, int l)
	__attribute__((nonnull));

int lstack_write_alt(lwx * const restrict lx, int x, int y, const char* const restrict s, int l)
	__attribute__((nonnull));

/// lstack_writef
//
// write text to the entry at x:y using printf-style args
//
int lstack_writef(lwx * const restrict lx, int x, int y, const char* const restrict s, ...)
	__attribute__((nonnull));

/// lstack_add
//
// write text to the next unused entry of list 0
//
int lstack_add(lwx * const restrict lx, const char* const restrict s, int l)
	__attribute__((nonnull));

/// lstack_addf
//
// write text to the next unused entry of list 0 using printf-style args
//
int lstack_addf(lwx * const restrict lx, const char* const restrict fmt, ...)
	__attribute__((nonnull));

///
/// STACKOPS API
///
///  typically after performing one of these operations, selection and/or windows need to be
///  reset or adjusted
///

/// lstack_shift
//
// delete the 0th list - all other lists are shifted down
//
int lstack_shift(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_unshift
//
// unshift an empty list onto the stack at index zero - all other lists are shifted up
//
int lstack_unshift(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_pop
//
// delete the Nth list
//
int lstack_pop(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_push
//
// push an empty list onto the end of the stack
//
int lstack_push(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_cycle
//
// move the first list on the stack to the last position
//
int lstack_cycle(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_recycle
//
// move the last list on the stack to the first position
//
int lstack_recycle(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_xchg
//
// exchange the first and second lists on the stack
//
int lstack_xchg(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_merge
//
// SUMMARY
//  append the rows in list b to the end of list a
//
int lstack_merge(lwx * const restrict lx, int a, int b)
	__attribute__((nonnull));

/// allocate
//
//
//
int lstack_allocate(lwx * const restrict lx, int x, int y, int z);

/// lstack_clear
//
// SUMMARY
//  clear the row at x:y
//
// PARAMETERS
//  lx - lw context
//  x  - list index
//  y  - row index
//
int lstack_clear(lwx * const restrict lx, int x, int y)
  __attribute__((nonnull));

/// ensure
//
// ensure stack/list allocation up to the specified dimensions
//
// parameters
//   x - zero based list index       ( or -1 to skip stack allocation)
//   y - zero based string index     ( or -1 to skip list allocation)
//   z - zero based character index  ( or -1 to skip string allocation)
//
// updates stack and list length to be at least the dimension specified
//  ** this is NOT done for string length **
//
int lstack_ensure(lwx * const restrict lx, int x, int y, int z);

/// lstack_move
//
// SUMMARY
//  move the entry at bx:by to position ax:ay
//
// DETAILS
//  1. copy ax:ay
//  2. set ax:ay = bx:by
//  3. copy entries following bx:by down by 1, overwriting it
//  4. place the original ax:ay at the end of ls->s[bx]
//  5. ls->s[bx].l--
//
// RETURNS
//  lstack_move returns 0
//
int lstack_move(lwx * const restrict lx, int ax, int ay, int bx, int by)
	__attribute__((nonnull));

/// lstack_delete
//
// SUMMARY
//  delete the entry at position x:y
//
// DETAILS
//  1. copy x:y
//  2. copy entries following x:y down by 1, overwriting it
//  3. place the original x:y at the end of ls->s[x]
//  4. ls->s[x].l--
//
// RETURNS
//  lstack_delete returns 1
//
int lstack_delete(lwx * const restrict lx, int x, int y)
	__attribute__((nonnull));

/// lstack_displace
//
// SUMMARY
//  prepare l entries at x:y to receive new writes
//  these entries should be immediately populated with lstack_write_alt/lstack_obj_write_alt
//
// DETAILS
//	1. ensure s[x] has s[x].l + l allocated entries
//  2. copy l unused entries from s[x].s[s[x].l]
//  3. copy l entries x:y -> x:y+l
//	4. copy l unused entries into x:y
//  5. s[x].l += l
//
// NOTES
//  no-op when l == 0
//
// RETURNS
//  1 on error
//
int lstack_displace(lwx * const restrict lx, int x, int y, int l)
	__attribute__((nonnull));

/// lstack_swaptop
//
// SUMMARY
//  swap two entries in the top list
//
// PARAMETERS
//  ay - list index of entry 1
//  by - list index of entry 2
//
int lstack_swaptop(lwx * const restrict lx, int ay, int by)
	__attribute__((nonnull));

/// lwx_getflags
//
// get application-use flags associated with an lwx
//
uint64_t lwx_getflags(lwx * const restrict)
	__attribute__((nonnull));

/// lwx_setflags
//
// set application-use flags associated with an lwx
//
uint64_t lwx_setflags(lwx * const restrict, const uint64_t)
	__attribute__((nonnull));

/// lwx_getptr
//
// get application-use ptr associated with an lwx
//
void * lwx_getptr(lwx * const restrict)
	__attribute__((nonnull));

/// lwx_setptr
//
// set application-use ptr associated with an lwx
//
void * lwx_setptr(lwx * const, void * const)
	__attribute__((nonnull(1)));

#undef restrict
#endif
