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

#ifndef _LISTWISE_ITERATE_H
#define _LISTWISE_ITERATE_H

///
/// macros for iterating the lstack in an lw context
///

#define restrict __restrict

void lwx_iterate_loop(struct lwx_t * const restrict lx, const int x, const int y, const int sel, int * const restrict go)
  __attribute__((nonnull));

int lwx_lists(struct lwx_t * const restrict lx)
  __attribute__((nonnull));
  
int lwx_rows(struct lwx_t * const restrict lx, const int x)
  __attribute__((nonnull));

#undef restrict

#define LSTACK_ITERATE_FWD(lx, x, y, sel, go) \
  LSTACK_ITERATE_HEADER(lx, x, y, go)         \
  LSTACK_ITERATE_LOOP(lx, x, y, sel, go)

#define LSTACK_ITERREV_REV(lx, x, y, sel, go) \
  LSTACK_ITERREV_HEADER(lx, x, y, go)         \
  LSTACK_ITERATE_LOOP(lx, x, y, sel, go)

#define LSTACK_ITERATE_LOOP(lx, __x, __y, __sel, go)    \
  int go;                                               \
  lwx_iterate_loop(lx, __x, __y, __sel, &go);

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
