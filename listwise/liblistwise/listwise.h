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

#ifndef _LISTWISE_H
#define _LISTWISE_H

#include <sys/types.h>

#include "xapi.h"

// listwise execution context
struct lwx;
typedef struct lwx lwx;

// implementation for LSTACK_ITER* macros
#include "listwise/iterate.h"

#define restrict __restrict

/// listwise_exec
//
// SUMMARY
//  parse the transform-string and execute it against the lw execution context
//
// PARAMETERS
//  ts       - transform-string
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
//
xapi listwise_exec(
    const char * const restrict ts
  , char ** const restrict init
  , size_t * const restrict initls
  , const size_t initl
  , struct lwx ** restrict lx
)
  __attribute__((nonnull(1, 5)));

/// lwx_alloc
//
// SUMMARY
//  allocate an lw context
//
xapi lwx_alloc(struct lwx ** const restrict)
  __attribute__((nonnull));

/// lwx_free
//
// SUMMARY
//  free an lw context with free semantics
//
void lwx_free(struct lwx * const restrict);

/// lwx_ifree
//
// SUMMARY
//  free an lw context with iwfree semantics
//
void lwx_ifree(struct lwx ** const restrict)
  __attribute__((nonnull));

///
/// macros for iterating the lstack in an lw context
///

// iterate the selected elements of the 0th list of the lstack
#define LSTACK_ITERATE(lx, y, go) LSTACK_ITERATE_FWD(lx, 0, y, 1, 1, go)

// iterate in reverse the selected elements of the 0th list of the lstack
#define LSTACK_ITERREV(lx, y, go) LSTACK_ITERREV_REV(lx, 0, y, 1, 1, go)

// iterate elements of the xth list on the lstack
//  iteration limited to selected elements when x == 0
#define LSTACK_ITERATE_LIST(lx, x, y, go) LSTACK_ITERATE_FWD(lx, x, y, x == 0, x == 0, go)

// iterate in reverse elements of the xth list on the lstack
//  iteration limited to selected elements when x == 0
#define LSTACK_ITERREV_LIST(lx, x, y, go) LSTACK_ITERATE_REV(lx, x, y, x == 0, x == 0, go)

// close an iterate block
#define LSTACK_ITEREND }}

// get the number of selected elements of the 0th list of the lstack
#define LSTACK_COUNT(lx) LSTACK_COUNT_LIST(lx, 0)

// get the number of lists in the lstack
#define LSTACK_LISTS(lx) lwx_lists(lx)

/// lstack_getbytes
//
// SUMMARY
//  get a pointer/length for the string representation of the specified row
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
xapi lstack_getbytes(struct lwx * const restrict lx, int x, int y, char ** const restrict r, int * const restrict rl)
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
xapi lstack_getstring(struct lwx * const restrict lx, int x, int y, char ** const restrict r, int * const restrict rl)
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
xapi lstack_string(struct lwx * const restrict lx, int x, int y, char ** restrict r)
  __attribute__((nonnull));

/// lwx_reset
//
// SUMMARY
//  reset (but do not deallocate) an lw context
//
// REMARKS
//  no-op with zero-valued parameter
//
xapi lwx_reset(struct lwx * const restrict lx)
  __attribute__((nonnull));

/// listwise_identity
//
// SUMMARY
//  pointer to singleton liblistwise-managed lwx object with a single 0-element list
//
extern struct lwx * listwise_identity;

#undef restrict
#endif
