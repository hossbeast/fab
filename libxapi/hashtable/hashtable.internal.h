/* Copyright (c) 2012-2018 Todd Freed <todd.freed@gmail.com>

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

#ifndef _XAPI_HASHTABLE_INTERNAL_H
#define _XAPI_HASHTABLE_INTERNAL_H

/*

MODULE
 hashtable

SUMMARY
 statically allocated unordered collection of key/value pairs

*/

#include "types.h"
#include "hashtable.h"

typedef struct hashtable
{
  uint16_t    entries;    // number of active entries
  uint16_t    size;       // table length, in elements, a power of two
  uint16_t    lm;

  uint16_t *  tk;         // key table
  void **     tv;         // value table
} hashtable;

/// hashtable_init
//
// SUMMARY
//  initialize a hashtable
//
// PARAMETERS
//  ht   - hashtable
//  tk   - key table
//  tv   - value table
//  size - entries in tk and tv, a power of 2
//
// RETURNS
//  EINVAL if size is not a power of 2
//
int lx_hashtable_init(struct hashtable * restrict ht, uint16_t * restrict tk, void ** restrict tv, uint16_t size)
  __attribute__((nonnull));

/// hashtable_set
//
// SUMMARY
//  add or update an element
//
// PARAMETERS
//  hashtable     - hashtable
//  key     - pointer to key
//  [value] - pointer to value
//
int lx_hashtable_set(hashtable * restrict ht, uint16_t key, void * restrict value)
  __attribute__((nonnull(1)));

/// hashtable_get
//
// SUMMARY
//  given a key, returns pointer to associated value, or 0 if not found
//
// PARAMETERS
//  hashtable     - hashtable
//  key     - pointer to key
//
// EXAMPLE
//  int k = 15;
//  int* v = 0;
//  if((v = hashtable_get(hashtable, MM(k))) == 0)
//    /* key is not set */
//
// SUMMARY
//  returns pointer to the stored value, or 0 if not found
//
void * lx_hashtable_get(const hashtable * restrict ht, uint16_t key)
  __attribute__((nonnull));

#endif
