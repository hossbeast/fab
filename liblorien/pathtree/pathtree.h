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

#ifndef _LORIEN_PATHTREE_H
#define _LORIEN_PATHTREE_H

/*

prefix tree

SUMMARY
 dynamically resizing ordered collection of elements addressable by index

REMARKS
 meant to be used as secondary storage for the elements in the collection

*/

#include "xapi.h"

struct pathtree;
typedef struct pathtree pathtree;

#ifndef PATHTREE_VALUE_TYPE
# define PATHTREE_VALUE_TYPE void
#endif

#define restrict __restrict

/// pathtree_create
//
// SUMMARY
//  create an empty pathtree
//
// PARAMETERS
//  pathtree        - (returns) instance
//
// REMARKS
//  free_element follows the free idiom
//
xapi pathtree_create(pathtree ** restrict pt)
  __attribute__((nonnull(1)));

/// pathtree_free
//
// SUMMARY
//  free a pathtree with free semantics
//
void pathtree_free(pathtree * const restrict pt);

/// pathtree_ifree
//
// SUMMARY
//  free a pathtree with ifree semantics
//
void pathtree_ifree(pathtree ** const restrict pt)
  __attribute__((nonnull));

/// pathtree_insert
//
// SUMMARY
//  insert a key / value pair
//
// PARAMETERS
//  pt  - pathtree instance
//  key - key
//  val - value
//
xapi pathtree_insert(pathtree * const restrict pt, char * const restrict key, PATHTREE_VALUE_TYPE * val)
  __attribute__((nonnull));

/// pathtree_search
//
// SUMMARY
//   lookup a value by prefix
//
// PARAMETERS
//  pt     - pathtree instance
//  prefix - 
//  attrs  - bitwise combo of PATHTREE_*
//
// RETURNS
//  value associated with the most closely matching prefix, or null if no such match is found
//
PATHTREE_VALUE_TYPE * pathtree_search(pathtree * const restrict pt, char * const restrict prefix)
  __attribute__((nonnull));

#endif
