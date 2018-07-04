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

#include "xlinux/xstdlib.h"

#include "internal.h"
#include "dictionary.internal.h"
#include "map.internal.h"

static void mapping_free(mapping * mg)
{
  if(mg)
  {
    wfree(mg->k);
    wfree(mg->v);
  }
  wfree(mg);
}

//
// api
//

API xapi dictionary_createx(
    dictionary ** const restrict dict
  , size_t vsz
  , size_t capacity
  , void * free_value
  , void * xfree_value
)
{
  enter;

  fatal(xmalloc, dict, sizeof(**dict));
  fatal(map_init, *dict, capacity, free_value, xfree_value);

  (*dict)->vsz = vsz;
  (*dict)->free_mapping = mapping_free;

  finally : coda;
}

API xapi dictionary_create(dictionary ** const restrict d, size_t vsz)
{
  xproxy(dictionary_createx, d, vsz, 0, 0, 0);
}

API xapi dictionary_set(dictionary * const restrict d, const void * restrict k, size_t kl, void * const restrict rv)
{
  xproxy(map_put, d, k, kl, 0, d->vsz, rv);
}

API void * dictionary_get(const dictionary* const restrict d, const char * const restrict k, size_t kl)
{
  return map_get(d, k, kl);
}

API xapi dictionary_recycle(dictionary* const restrict d)
{
  xproxy(map_recycle, d);
}

API xapi dictionary_delete(dictionary * const restrict d, const char * const restrict k, size_t kl)
{
  xproxy(map_delete, d, k, kl);
}

API xapi dictionary_keys(const dictionary * const restrict d, const char *** const restrict keys, size_t * const restrict keysl)
{
  xproxy(map_keys, d, keys, keysl);
}

API xapi dictionary_values(const dictionary* const restrict d, void * restrict values, size_t * const restrict valuesl)
{
  xproxy(map_values, d, values, valuesl);
}

API xapi dictionary_xfree(dictionary* const restrict d)
{
  xproxy(map_xfree, d);
}

API xapi dictionary_ixfree(dictionary** const restrict d)
{
  enter;

  fatal(dictionary_xfree, *d);
  *d = 0;

  finally : coda;
}

API const void * dictionary_table_key(const dictionary * const restrict d, size_t x)
{
  return map_table_key(d, x);
}

API void * dictionary_table_value(const dictionary * const restrict d, size_t x)
{
  return map_table_value(d, x);
}
