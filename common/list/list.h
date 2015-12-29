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

#ifndef _LIST_H
#define _LIST_H

#include "list.h"

/*

example usage

union { list c; struct { int * v; size_t l; }; } ints = {{ .es = sizeof(int) }};

ints.v[0];

*/

typedef struct list
{
  void *  list_v;    // vector
  size_t  list_l;    // length in elements
  size_t  list_es;   // element size
  size_t  list_a;    // allocated size in elements
} list;

#define restrict __restrict

/// list_append
//
// SUMMARY
//
// PARAMETERS
//  s  - list
//  el - pointer to element to append
//
xapi list_append(list * const restrict s, void * const restrict el);

/// list_append_range
//
// SUMMARY
//
// PARAMETERS
//  s   - list
//  el  - pointer to first element to append
//  len - number of elements
//
xapi list_append_range(list * const restrict s, void * const restrict el, size_t len);

/// list_grow
//
// SUMMARY
// 
// PARAMETERS
//
xapi list_grow(list * const restrict s, size_t len);

/// list_clear
//
// SUMMARY
//  reset the number of elements in the list - the allocation remains intact
//
void list_clear(list * const restrict s);

#undef restrict
#endif
