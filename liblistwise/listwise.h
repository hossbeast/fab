/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

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

#ifndef _LISTWISE_H
#define _LISTWISE_H

#include <stdint.h>

/* liblistwise API
**
** core functionality
**  listwise.h         - listwise evaluation, read lstack contents
**
** additional functionality
**  listwise/xtra.h    - register additional operators, tune execution parameters, etc
*/

/// listwise execution context
//
// an opaque type
//
struct lwx_t;
typedef struct lwx_t lwx;

// implementation for LSTACK_ITER* macros
#include <listwise/iterate.h>

#define restrict __restrict

/// listwise_exec
//
// SUMMARY
//  parse the listwise generator string and execute it against the lw execution context
//
// PARAMETERS
//  s        - generator string
//  l        - s length, or 0 for strlen
//  [init]   - rows to write to the stack before executing
//  [initls] - lengths for rows in init, 0 for strlen
//  initl    - number of rows in init
//  lx       - input/output lw context
//
// RETURNS
//  0 for success
//
// REMARKS
//  if *lx is null, a new lw context is created and returned
//  otherwise, an existing lw context is reused
//
int listwise_exec(
    char * const restrict s
  , int l
  , char ** const restrict init
  , int * const restrict initls
  , const int initl
  , lwx ** restrict lx
)
  __attribute__((nonnull(1, 6)));

/// lwx_alloc
//
// SUMMARY
//  allocate an lw context
//
int lwx_alloc(lwx ** const restrict)
  __attribute__((nonnull));

/// lwx_free
//
// SUMMARY
//  free an lw context with free-like semantics
//
void lwx_free(lwx * const restrict);

/// lwx_xfree
//
// SUMMARY
//  free an lw context with xfree-like semantics
//
void lwx_xfree(lwx ** const restrict)
  __attribute__((nonnull));

///
/// macros for iterating the lstack in an lw context
///

// iterate the selected elements of the 0th list of the lstack
#define LSTACK_ITERATE(lx, y, go) LSTACK_ITERATE_FWD(lx, 0, y, 1, go)

// iterate in reverse the selected elements of the 0th list of the lstack
#define LSTACK_ITERREV(lx, y, go) LSTACK_ITERREV_REV(lx, 0, y, 1, go)

// iterate elements of the xth list on the lstack
//  iteration limited to selected elements when x == 0
#define LSTACK_ITERATE_LIST(lx, x, y, go) LSTACK_ITERATE_FWD(lx, x, y, x == 0, go)

// iterate in reverse elements of the xth list on the lstack
//  iteration limited to selected elements when x == 0
#define LSTACK_ITERREV_LIST(lx, x, y, go) LSTACK_ITERATE_REV(lx, x, y, x == 0, go)

// close an iterate block
#define LSTACK_ITEREND }}

// get the number of selected elements of the 0th list of the lstack
#define LSTACK_COUNT(lx) LSTACK_COUNT_LIST(lx, 0)

// get the number of lists in the lstack
#define LSTACK_LISTS(lx) lwx_lists(lx)

/// lstack_getbytes
//
// SUMMARY
//  get pointer/length of string representation for the specified row
//
// PARAMETERS
//  lx - lw context
//  x  - list index
//  y  - row index
//  r  - string returned here
//  rl - length returned here
//
// RETURNS
//  0 on success
//
// REMARKS
//  use lstack_getstring when a string (or tmp space) is required
//
int lstack_getbytes(lwx * const restrict lx, int x, int y, char ** const restrict r, int * const restrict rl)
  __attribute__((nonnull));

/// lstack_getstring
//
// SUMMARY
//  get pointer/length for a null-terminated string representation of the specified row
//
// PARAMETERS
//  lx - lw context
//  x  - list index
//  y  - row index
//  r  - string returned here
//  rl - length returned here
//
// RETURNS
//  0 on success
//
// REMARKS
//  getstring is guaranteed to return a string that is 1) null-terminated, and 2) in tmp space
//
int lstack_getstring(lwx * const restrict lx, int x, int y, char ** const restrict r, int * const restrict rl)
  __attribute__((nonnull));

/// lstack_string
//
// SUMMARY
//  get a null-terminated string representation of the specified row
//
// PARAMETERS
//  lx - lw context
//  x  - list index
//  y  - row index
//
// RETURNS
//  0 on success
//
// REMARKS
//
char * lstack_string(lwx * const restrict lx, int x, int y)
  __attribute__((nonnull));

/// lwx_reset 
//
// SUMMARY
//  reset (but do not deallocate) an lw context
//
// REMARKS
//  no-op with zero-valued parameter
//
int lwx_reset(lwx * const restrict)
  __attribute__((nonnull));

#undef restrict
#endif
