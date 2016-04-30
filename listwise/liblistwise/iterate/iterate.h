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

#ifndef _LISTWISE_ITERATE_H
#define _LISTWISE_ITERATE_H

struct lwx;

///
/// macros for iterating the lstack in an lw context
///

#define restrict __restrict

/// lwx_iterate_loop
//
// SUMMARY
//  determine whether to include row {x,y} in the iteration
//
// PARAMETERS
//  lx  - lwx instance
//  x   - list index
//  y   - row index
//  sel - whether to consult the selection
//  win - whether to consult the windows
//  go  - (returns) whether to include the row in the iteration
//
void lwx_iterate_loop(struct lwx * const restrict lx, const int x, const int y, const int sel, const int win, int * const restrict go)
  __attribute__((nonnull));

int lwx_lists(struct lwx * const restrict lx)
  __attribute__((nonnull));
  
int lwx_rows(struct lwx * const restrict lx, const int x)
  __attribute__((nonnull));

#undef restrict

/// LSTACK_ITERATE_FWD
//
// SUMMARY
//  setup a loop for iterating the rows of a list in an lwx
//
// PARAMETERS
//  lx   - lwx instance
//  x    - list index
//  y    - on each iteration, set to a value indicating the row index
//  sel  - whether to consult the selection
//  win  - whether to consult the windows
//  go   - on each iteration, set to a boolean value indicating whether to include that row in the iteration
//
#define LSTACK_ITERATE_FWD(lx, x, y, sel, win, go) \
  LSTACK_ITERATE_HEADER(lx, x, y, go)         \
  LSTACK_ITERATE_LOOP(lx, x, y, sel, win, go)

/// LSTACK_ITERATE_REV
//
// SUMMARY
//  see LSTACK_ITERATE_FWD : reverse order
//
#define LSTACK_ITERREV_REV(lx, x, y, sel, win, go) \
  LSTACK_ITERREV_HEADER(lx, x, y, go)         \
  LSTACK_ITERATE_LOOP(lx, x, y, sel, win, go)

#define LSTACK_ITERATE_LOOP(lx, __x, __y, __sel, __win, go)  \
  int go;                                                    \
  lwx_iterate_loop(lx, __x, __y, __sel, __win, &go);

#define LSTACK_ITERATE_HEADER(lx, __x, __y, go)         \
  if(lwx_lists(lx) > __x)                               \
  {                                                     \
    for(__y = 0; __y < lwx_rows(lx, __x); __y++)        \
    {

#define LSTACK_ITERREV_HEADER(lx, __x, __y, go)         \
  if(lwx_lists(lx) > __x)                               \
  {                                                     \
    for(__y = lwx_rows(lx, __x) - 1; __y >= 0; __y--)   \
    {

// get the number of selected elements of the xth list
#define LSTACK_COUNT_LIST(lx, __x) ({   \
  int c = 0;                            \
  int __y;                              \
  LSTACK_ITERATE_LIST(lx, __x, __y, go) \
  if(go)                                \
  {                                     \
    c++;                                \
  }                                     \
  LSTACK_ITEREND;                       \
  c;                                    \
})


#endif
