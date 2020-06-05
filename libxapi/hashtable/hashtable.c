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

#include <errno.h>

#include "types.h"

#include "xapi.h"
#include "hashtable.internal.h"

#include "macros.h"

/// probe
//
// SUMMARY
//  search for a key by linear probe and get its index
//
// RETURNS
//  true if the specified key was found
//
static bool __attribute__((nonnull)) probe(const hashtable * restrict ht, uint16_t key, uint16_t * restrict i)
{
  *i = key & ht->lm;

  uint16_t x = *i;
  while(1)
  {
    if(ht->tk[*i] == 0)
      return false;

    if(ht->tk[*i] == key)
      return true;

    (*i)++;
    (*i) &= ht->lm;

    if(*i == x)
      return false;
  }
}

//
// api
//

int hashtable_init(struct hashtable * restrict ht, uint16_t * restrict _tk, void ** restrict _tv, uint16_t _size)
{
  if((_size & (_size - 1)) != 0)
    return EINVAL;

  ht->tk = _tk;
  ht->tv = _tv;
  ht->size = _size;
  ht->lm = ht->size - 1;

  return 0;
}

int hashtable_set(hashtable * restrict ht, uint16_t key, void * restrict v)
{
  if(key == 0)
    return EINVAL;

  uint16_t i;
  if(!probe(ht, key, &i))
  {
    if(ht->entries == ht->size)
      return ENOMEM;

    ht->tk[i] = key;
    ht->entries++;
  }

  // copy the value
  if(v)
    ((void**)ht->tv)[i] = v;

  return 0;
}

void * hashtable_get(const hashtable * restrict ht, uint16_t key)
{
  uint16_t i = 0;
  if(!probe(ht, key, &i))
    return 0;

  return ht->tv[i];
}

uint16_t hashtable_entries(const hashtable * restrict ht)
{
  return ht->entries;
}

uint16_t hashtable_size(const hashtable * restrict ht)
{
  return ht->size;
}

uint16_t hashtable_keyat(const hashtable * restrict ht, uint16_t x)
{
  return ht->tk[x];
}

void * hashtable_valueat(const hashtable * restrict ht, uint16_t x)
{
  return ht->tv[x];
}
