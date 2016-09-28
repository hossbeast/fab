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

#include <stdlib.h>
#include <stdio.h>

#include "xapi.h"
#include "xapi/trace.h"

#include "xlinux/xstdlib.h"

struct item;
#define MAP_VALUE_TYPE struct item
#include "map.h"

#include "test_util.h"
#include "macros.h"

struct item
{
  int a;
  int x;
};
typedef struct item item;

static void destructor(item * value)
{
  free(value);
}

static xapi validate(map * mapp)
{
  enter;

  const char ** keys = 0;
  size_t keysl = 0;
  item ** values = 0;
  size_t valuesl = 0;

  // retrieve by key
  item * itemp;
  itemp = map_get(mapp, MMS("one"));
  assertf(itemp->x == 1, "%d", "%d", 1, itemp->x);

  itemp = map_get(mapp, MMS("two"));
  assertf(itemp->x == 2, "%d", "%d", 2, itemp->x);

  itemp = map_get(mapp, MMS("three"));
  assertf(itemp->x == 3, "%d", "%d", 3, itemp->x);

  // lists of keys
  fatal(map_keys, mapp, &keys, &keysl);
  assertf(keysl == 3, "keys %d", "keys %zu", 3, keysl);

  int stringsort(const void * A, const void * B) { return strcmp(*(char **)A, *(char**)B); };
  qsort(keys, keysl, sizeof(*keys), stringsort);

  assertf(strcmp(keys[0], "one") == 0, "%s", "%s", "one", keys[0]);
  assertf(strcmp(keys[1], "three") == 0, "%s", "%s", "three", keys[1]);
  assertf(strcmp(keys[2], "two") == 0, "%s", "%s", "two", keys[2]);

  // list of values
  fatal(map_values, mapp, &values, &valuesl);
  assertf(keysl == 3, "values %d", "values %zu", 3, keysl);

  int itemsort(const void * A, const void * B) { return (*(item **)A)->x - (*(item **)B)->x; };
  qsort(values, valuesl, sizeof(*values), itemsort);

  assertf(values[0]->x == 1, "%d", "%d", 1, values[0]->x);
  assertf(values[1]->x == 2, "%d", "%d", 2, values[1]->x);
  assertf(values[2]->x == 3, "%d", "%d", 3, values[2]->x);

  // by slot
  int x;
  for(x = 0; x < map_table_size(mapp); x++)
  {
    const char * key = map_table_key(mapp, x);
    itemp = map_table_value(mapp, x);

    if(!key ^ !itemp)
      ufail();

    if(key)
    {
      if(strcmp(key, "one") == 0)
      {
        assertf(itemp->x == 1, "%d", "%d", 1, itemp->x);
      }
      else if(strcmp(key, "two") == 0)
      {
        assertf(itemp->x == 2, "%d", "%d", 2, itemp->x);
      }
      else if(strcmp(key, "three") == 0)
      {
        assertf(itemp->x == 3, "%d", "%d", 3, itemp->x);
      }
      else
      {
        ufail();
      }
    }
  }

finally:
  free(keys);
  free(values);
coda;
}

int main()
{
  enter;

  xapi r;
  map * mapp = 0;
  item * itemp = 0;

  fatal(map_createx, &mapp, destructor, 0, 0);

  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 1;
  fatal(map_set, mapp, MMS("one"), itemp);
  itemp = 0;

  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 2;
  fatal(map_set, mapp, MMS("two"), itemp);
  itemp = 0;

  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = 3;
  fatal(map_set, mapp, MMS("three"), itemp);
  itemp = 0;

  fatal(validate, mapp);

  success;

finally:
  if(XAPI_UNWINDING)
  {
    xapi_backtrace();
  }

  fatal(map_xfree, mapp);
  free(itemp);

conclude(&r);

  xapi_teardown();
  return !!r;
}
