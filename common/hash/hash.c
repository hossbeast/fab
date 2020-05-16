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

#include "hash.h"

/*

Constants taken from the jenkins has function
  http://en.wikipedia.org/wiki/Jenkins_hash_function

*/

static inline uint32_t mix32(uint32_t h, uint8_t c)
{
  h += c;
  h += (h << 10);
  h += (h >> 6);
  return h;
}

static inline uint32_t final32(uint32_t h)
{
  h += (h << 3);
  h ^= (h >> 11);
  h += (h << 15);
  return h;
}

static inline uint64_t mix64(uint64_t h, uint8_t c)
{
  h += c;
  h += (h << 13);
  h += (h >> 3);
  return h;
}

static inline uint64_t final64(uint64_t h)
{
  h += (h << 3);
  h ^= (h >> 23);
  h += (h << 34);
  return h;
}

//
// public
//

uint32_t strhash32(uint32_t h, const char * restrict s)
{
  while(*s)
  {
    h = mix32(h, *s);
    s++;
  }

  return final32(h);
}

uint32_t hash32(uint32_t h, const void * const restrict v, size_t l)
{
  const char * s = v;

  size_t x;
  for(x = 0; x < l; x++)
    h = mix32(h, s[x]);

  return final32(h);
}

uint64_t strhash64(uint64_t h, const char * restrict s)
{
  while(*s)
  {
    h = mix64(h, *s);
    s++;
  }

  return final64(h);
}

uint64_t hash64(uint64_t h, const void * const restrict v, size_t l)
{
  const char * s = v;

  size_t x;
  for(x = 0; x < l; x++)
    h = mix64(h, s[x]);

  return final64(h);
}
