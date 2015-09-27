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

#ifndef _LISTWISE_LSTACK_H
#define _LISTWISE_LSTACK_H

#include <sys/types.h>
#include "xapi.h"

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
//  [win] - whether windows is resolved
//  [str] - whether the return value is required to be null-terminated and in tmp space
//  [raw] - indicates whether the raw storage was returned (object entry not resolved, window not resolved, and str was not specified)
//
xapi lstack_readrow(lwx * const restrict lx, int x, int y, char ** const restrict r, int * const restrict rl, uint8_t * const restrict rt, int obj, int win, int str, int * const restrict raw)
	__attribute__((nonnull(1)));

/// lstack_write
//
// SUMMARY
//  write text to the entry at x:y
//
// PARAMETERS
//  lx    - lw context
//  x     - list
//  y     - row
//  [s]   - pointer to string
//  [l]   - string length
//  [fmt] - format string
//
// VARIANTS
//  writes - string specified as null-terminated string
//  writew - string specified as pointer/length pair
//  writef - string specified as printf-style arglist// PARAMETERS
//
xapi lstack_writes(lwx * const restrict lx, int x, int y, const char* const restrict s)
	__attribute__((nonnull));

xapi lstack_writew(lwx * const restrict lx, int x, int y, const char* const restrict s, size_t l)
	__attribute__((nonnull));

xapi lstack_writef(lwx * const restrict lx, int x, int y, const char* const restrict fmt, ...)
	__attribute__((nonnull(1, 4)));

/// lstack_alt_write
//
// SUMMARY
//  write text to the entry at x:y
//
// PARAMETERS
//  lx    - lw context
//  x     - list
//  y     - row
//  [s]   - pointer to string
//  [l]   - string length
//  [fmt] - format string
//
// VARIANTS
//  writes - string specified as null-terminated string
//  writew - string specified as pointer/length pair
//  writef - string specified as printf-style arglist// PARAMETERS
//
// REMARKS
//  used in conjunction with lstack_displace
//
xapi lstack_alt_writes(lwx * const restrict lx, int x, int y, const char* const restrict s)
	__attribute__((nonnull));

xapi lstack_alt_writew(lwx * const restrict lx, int x, int y, const char* const restrict s, size_t l)
	__attribute__((nonnull));

xapi lstack_alt_writef(lwx * const restrict lx, int x, int y, const char* const restrict fmt, ...)
	__attribute__((nonnull(1, 4)));

/// lstack_cat
//
// SUMMARY
//  append text to the entry at x:y
//
// PARAMETERS
//  lx    - lw context
//  x     - list
//  y     - row
//  [s]   - pointer to string
//  [l]   - string length
//  [fmt] - format string
//
// VARIANTS
//  cats - string specified as null-terminated string
//  catw - string specified as pointer/length pair
//  catf - string specified as printf-style arglist
//
xapi lstack_cats(lwx * const restrict lx, int x, int y, const char* const restrict s)
	__attribute__((nonnull));

xapi lstack_catw(lwx * const restrict lx, int x, int y, const char* const restrict s, size_t l)
	__attribute__((nonnull));

xapi lstack_catf(lwx * const restrict lx, int x, int y, const char* const restrict fmt, ...)
	__attribute__((nonnull(1, 4)));

/// lstack_add
//
// SUMMARY
//  write text to the next unused entry of list 0
//
// PARAMETERS
//  lx    - lw context
//  [s]   - pointer to string
//  [l]   - string length
//  [fmt] - format string
//
// VARIANTS
//  writes - string specified as null-terminated string
//  writew - string specified as pointer/length pair
//  writef - string specified as printf-style arglist
//
xapi lstack_adds(lwx * const restrict lx, const char* const restrict s)
	__attribute__((nonnull));

xapi lstack_addw(lwx * const restrict lx, const char* const restrict s, size_t l)
	__attribute__((nonnull));

xapi lstack_addf(lwx * const restrict lx, const char* const restrict fmt, ...)
	__attribute__((nonnull(1, 2)));

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
xapi lstack_shift(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_unshift
//
// unshift an empty list onto the stack at index zero - all other lists are shifted up
//
xapi lstack_unshift(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_pop
//
// delete the Nth list
//
xapi lstack_pop(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_push
//
// push an empty list onto the end of the stack
//
xapi lstack_push(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_merge
//
// SUMMARY
//  merge list {src} into list {dst}, reducing the number of lists by 1
//
xapi lstack_merge(lwx * const restrict lx, int dst, int src)
	__attribute__((nonnull));

/// lstack_allocate
//
//
//
xapi lstack_allocate(lwx * const restrict lx, int x, int y, int z);

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
xapi lstack_clear(lwx * const restrict lx, int x, int y)
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
xapi lstack_ensure(lwx * const restrict lx, int x, int y, int z);

/// lstack_move
//
// SUMMARY
//  move the entry at bx:by to position ax:ay, discarding the entry at ax:ay
//
// DETAILS
//  1. copy ax:ay
//  2. set ax:ay = bx:by
//  3. copy entries following bx:by down by 1, overwriting it
//  4. place the original ax:ay at the end of ls->s[bx]
//  5. ls->s[bx].l--
//
// NOTES
//  an operator which calls lstack_move with ax == 0 should use LWOP_WINDOWS_RESET
//
// RETURNS
//  lstack_move returns 0, indicating success
//
xapi lstack_move(lwx * const restrict lx, int ax, int ay, int bx, int by)
	__attribute__((nonnull));

/// lstack_swap
//
// SUMMARY
//  swap the entries at ax:ay and bx:by
//
// RETURNS
//  lstack_swap returns 0, indicating success
//
xapi lstack_swap(lwx * const restrict lx, int ax, int ay, int bx, int by)
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
xapi lstack_swaptop(lwx * const restrict lx, int ay, int by)
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
// NOTES
//  an operator which calls lstack_delete with x == 0 should use LWOP_WINDOWS_RESET
//
// RETURNS
//  lstack_delete returns 0, indicating success
//
xapi lstack_delete(lwx * const restrict lx, int x, int y)
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
//  an operator which calls lstack_displace with x == 0 should use LWOP_WINDOWS_RESET
//
// RETURNS
//  0 on success
//
xapi lstack_displace(lwx * const restrict lx, int x, int y, int l)
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

/*
 * unimplemented
 *
 */

/// lstack_cycle
//
// move the first list on the stack to the last position
//
xapi lstack_cycle(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_recycle
//
// move the last list on the stack to the first position
//
xapi lstack_recycle(lwx * const restrict lx)
	__attribute__((nonnull));

/// lstack_xchg
//
// exchange the first and second lists on the stack
//
xapi lstack_xchg(lwx * const restrict lx)
	__attribute__((nonnull));

#undef restrict
#endif
