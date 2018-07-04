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

#ifndef _MAP_INTERNAL_H
#define _MAP_INTERNAL_H

#include "map.h"

typedef struct mapping
{
  char * k;
  uint16_t ka; // allocated size
  uint16_t kl; // length

  void * v;
} mapping;

/// map_put
//
// SUMMARY
//  create or update a key/value pair mapping
//
// PARAMETERS
//  map   - map
//  key   - pointer to key
//  keyl  - key length
//  [r]   - pointer to value
//  [rv]  - (returns) pointer to map-owned value
//
xapi map_put(
    struct map * const restrict m
  , const void * const restrict key
  , size_t keyl
  , void * const restrict r
  , size_t vsz
  , void * const * const restrict rv
)
  __attribute__((nonnull(1, 2)));

xapi map_init(
    map * const restrict m
  , size_t capacity
  , void * free_value
  , void * xfree_value
)
  __attribute__((nonnull(1)));

#endif
