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

/*
** core functionality
**  listwise.h      - listwise evaluation, iterate lstacks, read lstack rows
**
** extra functionality
**  listwise/xtra.h - register additional operators, tune execution parameters, etc
*/

#define restrict __restrict

/// listwise execution context
//
// an opaque type
//
struct lwx_t;
typedef struct lwx_t lwx;

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
    const char * const restrict s
  , int l
  , const char ** const restrict init
  , const int * const restrict initls
  , const int initl
  , const lwx ** const restrict ls
)
  __attribute__((nonnull(1, 6)));

/// lwx_reset 
//
// SUMMARY
//  reset (but do not deallocate) an lw context
//
// REMARKS
//  no-op with zero-valued parameter
//
void lwx_reset(lwx * const restrict)
  __attribute__((nonnull));

/// lwx_getflags
//
// get application-use flags associated with lw
//
uint64_t lwx_getflags(lwx * const restrict)
	__attribute__((nonnull));

/// lwx_setflags
//
// set application-use flags associated with lw
//
uint64_t lwx_setflags(lwx * const restrict, const uint64_t)
	__attribute__((nonnull));

/// lwx_getptr
//
// get application-use ptr associated with lw
//
void * lwx_getptr(lwx * const restrict)
	__attribute__((nonnull));

/// lwx_setptr
//
// set application-use ptr associated with lw
//
void * lwx_setptr(lwx * const, void * const)
	__attribute__((nonnull(1)));

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

/// lwx_deepcopy
//
// SUMMARY
//  create a deep copy of an lw context
//
// PARAMETERS
//  dst - copy returned here
//  src - source lwx
//
// RETURNS
//  0 for success
//
// REMARKS
//  selection and windows are reset in the copy
//  the data associated with the *ptr and *flags apis are reset in the copy
//
int lwx_deepcopy(lwx ** const restrict, lwx * const restrict)
  __attribute__((nonnull));

///
/// macros for iterating the lstack in an lw context
///

void lwx_iterate_loop(lwx * const restrict lx, const int y, int * const restrict go)
  __attribute__((nonnull));

void lwx_lists(lwx * const restrict lx)
  __attribute__((nonnull));
  
void lwx_rows(lwx * const restrict lx, const int x)
  __attribute__((nonnull));

#define LSTACK_ITERATE_LOOP(lx, y, go)          \
  int go;                                       \
  lwx_iterate_loop(lx, y, &go);

#define LSTACK_ITERATE_HEADER(lx, x, y, go)     \
  if(lwx_lists(lx) > x)                         \
  {                                             \
    for(y = 0; y < lwx_rows(lx, x); y++)        \

#define LSTACK_ITERREV_HEADER(lx, x, y, go)     \
  if(lwx_lists(lx) > x)                         \
  {                                             \
    for(y = lwx_rows(lx, x) - 1; y >= 0; y--)   \
    {

// iterate the selected elements of the 0th list of the lstack
#define LSTACK_ITERATE(lx, y, go)       \
  LSTACK_ITERATE_HEADER(lx, 0, y, go)   \
  LSTACK_ITERATE_LOOP(lx, y, go)

// iterate in reverse the selected elements of the 0th list of the lstack
#define LSTACK_ITERREV(lx, y, go)       \
  LSTACK_ITERREV_HEADER(lx, 0, y, go)   \
  LSTACK_ITERATE_LOOP(lx, y, go)

// iterate the selected elements of the xth list on the lstack
#define LSTACK_ITERATE_LIST(lx, x, y, go) \
  LSTACK_ITERATE_HEADER(lx, x, y, go)     \
  LSTACK_ITERATE_LOOP(lx, y, go)

// iterate in reverse the selected elements of the xth list on the lstack
#define LSTACK_ITERREV_LIST(lx, x, y, go) \
  LSTACK_ITERREV_HEADER(lx, x, y, go)     \
  LSTACK_ITERATE_LOOP(lx, y, go)

// close an iterate block
#define LSTACK_ITEREND }}

// count selection in the top list
#define LSTACK_COUNT(lx) ({   \
  int c = 0;                  \
  int x;                      \
  LSTACK_ITERATE(lx, x, go)   \
  if(go)                      \
  {                           \
    c++;                      \
  }                           \
  LSTACK_ITEREND;             \
  c;                          \
})

/// lstack_getbytes
//
// SUMMARY
//  get pointer/length of string representation for the specified row
//
// PARAMETERS
//  lx - lw context
//  x  - list index
//  y  - entry index
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
//  y  - entry index
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
//  y  - entry index
//
// RETURNS
//  0 on success
//
// REMARKS
//
char * lstack_string(lwx * const restrict lx, int x, int y)
  __attribute__((nonnull));

#undef restrict
#endif
