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

#include "internal.h"
#include "dictionary.internal.h"
#include "map.internal.h"
#include "maputils.internal.h"

struct dictionary
{
  struct map;
};

//
// api
//

API xapi dictionary_create(dictionary ** const restrict m, size_t vsz)
{
  xproxy(map_allocate, (void*)m, MAP_PRIMARY, vsz, 0, 0, 0);
}

API xapi dictionary_createx(
    dictionary ** const restrict m
  , size_t vsz
  , void * free_value
  , void * xfree_value
  , size_t capacity
)
{
  xproxy(map_allocate, (void*)m, MAP_PRIMARY, vsz, free_value, xfree_value, capacity);
}

API xapi dictionary_set(dictionary * const restrict m, const char * const restrict key, size_t keyl, void * const restrict value)
{
  xproxy(map_put, m, key, keyl, 0, value);
}

API void * dictionary_get(const dictionary* const restrict m, const char * const restrict k, size_t kl)
{
  return map_get(m, k, kl);
}

API size_t dictionary_size(const dictionary * const restrict m)
{
  return map_size(m);
}

API xapi dictionary_recycle(dictionary* const restrict m)
{
  xproxy(map_recycle, m);
}

API xapi dictionary_delete(dictionary * const restrict m, const char * const restrict key, size_t keyl)
{
  xproxy(map_delete, m, key, keyl);
}

API xapi dictionary_keys(const dictionary * const restrict m, const char *** const restrict keys, size_t * const restrict keysl)
{
  xproxy(map_keys, m, keys, keysl);
}

API xapi dictionary_values(const dictionary* const restrict m, void * restrict values, size_t * const restrict valuesl)
{
  xproxy(map_values, m, values, valuesl);
}

API xapi dictionary_xfree(dictionary* const restrict m)
{
  xproxy(map_xfree, m);
}

API xapi dictionary_ixfree(dictionary** const restrict m)
{
  xproxy(map_ixfree, (void*)m);
}

API size_t dictionary_table_size(const dictionary * const restrict m)
{
  return map_table_size(m);
}

API const char * dictionary_table_key(const dictionary * const restrict m, size_t x)
{
  return map_table_key(m, x);
}

API void * dictionary_table_value(const dictionary * const restrict m, size_t x)
{
  return map_table_value(m, x);
}
