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
#include "xlinux.h"
#include "xlinux/SYS.errtab.h"

struct item;
#define MAP_VALUE_TYPE struct item
#include "map.h"

#include "macros.h"

struct item
{
  int a;
  int x;
};
typedef struct item item;

static void destructor(const char * key, item * value)
{
  free(value);
}

xapi validate(map * mapp)
{
  enter;

  const char ** keys = 0;
  size_t keysl = 0;
  item ** values = 0;
  size_t valuesl = 0;

  // retrieve by key
  item * itemp;
  itemp = map_get(mapp, MMS("one"));
  if(itemp->x != 1)
    tfail(perrtab_SYS, SYS_ENOMEM);

  itemp = map_get(mapp, MMS("two"));
  if(itemp->x != 2)
    tfail(perrtab_SYS, SYS_ENOMEM);

  itemp = map_get(mapp, MMS("three"));
  if(itemp->x != 3)
    tfail(perrtab_SYS, SYS_ENOMEM);

  // lists of keys
  fatal(map_keys, mapp, &keys, &keysl);

  if(keysl != 3)
    tfail(perrtab_SYS, SYS_ENOMEM);

  int stringsort(const void * A, const void * B) { return strcmp(*(char **)A, *(char**)B); };
  qsort(keys, keysl, sizeof(*keys), stringsort);

  if(strcmp(keys[0], "one"))
    tfailf(perrtab_SYS, SYS_ENOMEM, "expected : %s, actual: %s", "one", keys[0]);

  if(strcmp(keys[1], "three"))
    tfailf(perrtab_SYS, SYS_ENOMEM, "expected : %s, actual: %s", "three", keys[1]);

  if(strcmp(keys[2], "two"))
    tfailf(perrtab_SYS, SYS_ENOMEM, "expected : %s, actual: %s", "two", keys[2]);

  // list of values
  fatal(map_values, mapp, &values, &valuesl);

  if(valuesl != 3)
    tfail(perrtab_SYS, SYS_ENOMEM);

  int itemsort(const void * A, const void * B) { return (*(item **)A)->x - (*(item **)B)->x; };
  qsort(values, valuesl, sizeof(*values), itemsort);

  if(values[0]->x != 1)
    tfailf(perrtab_SYS, SYS_ENOMEM, "expected : %d, actual: %d", 1, values[0]->x);

  if(values[1]->x != 2)
    tfailf(perrtab_SYS, SYS_ENOMEM, "expected : %d, actual: %d", 2, values[1]->x);

  if(values[2]->x != 3)
    tfailf(perrtab_SYS, SYS_ENOMEM, "expected : %d, actual: %d", 3, values[2]->x);

  // by slot
  int x;
  for(x = 0; x < map_table_size(mapp); x++)
  {
    const char * key = map_table_key(mapp, x);
    itemp = map_table_value(mapp, x);

    if(!key ^ !itemp)
      tfail(perrtab_SYS, SYS_ENOMEM);

    if(key)
    {
      if(strcmp(key, "one") == 0)
      {
        if(itemp->x != 1)
          tfailf(perrtab_SYS, SYS_ENOMEM, "expected : %d, actual: %d", 1, itemp->x);
      }
      else if(strcmp(key, "two") == 0)
      {
        if(itemp->x != 2)
          tfailf(perrtab_SYS, SYS_ENOMEM, "expected : %d, actual: %d", 2, itemp->x);
      }
      else if(strcmp(key, "three") == 0)
      {
        if(itemp->x != 3)
          tfailf(perrtab_SYS, SYS_ENOMEM, "expected : %d, actual: %d", 3, itemp->x);
      }
      else
      {
        tfail(perrtab_SYS, SYS_ENOMEM);
      }
    }
  }

finally:
  free(keys);
  free(values);
coda;
}

xapi test_primary()
{
  enter;

  map * mapp = 0;
  fatal(map_create, &mapp, sizeof(item), 0, MAP_PRIMARY);

  item * itemp = 0;
  fatal(map_add, mapp, MMS("one"), &itemp);
  itemp->x = 1;

  fatal(map_add, mapp, MMS("two"), &itemp);
  itemp->x = 2;

  fatal(map_add, mapp, MMS("three"), &itemp);
  itemp->x = 3;

  fatal(validate, mapp);

finally:
  map_free(mapp);
coda;
}

xapi test_secondary()
{
  enter;

  map * mapp = 0;
  item * itemp = 0;

  fatal(map_create, &mapp, 0, destructor, MAP_SECONDARY);

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

finally:
  map_free(mapp);
  free(itemp);
coda;
}

xapi test_rehash()
{
  enter;

  map * mapp = 0;
  fatal(map_createx, &mapp, sizeof(item), 0, MAP_PRIMARY, 2);

  int x;
  for(x = 0; x <= 35; x++)
  {
    item * itemp = 0;
    fatal(map_add, mapp, MM(x), &itemp);
    itemp->x = x;

    if(x == 1 || x == 7 || x == 35)
    {
      if(x == 7)
      {
        int i;
        for(i = 0; i < 7; i++)
        {
          if(!map_delete(mapp, MM(i)))
            tfailf(perrtab_SYS, SYS_ENOMEM, "expected : (found), actual : %d", i);
        }
      }

      if(x >= 7)
      {
        int i;
        for(i = 0; i < 7; i++)
        {
          item * itemp = 0;
          if((itemp = map_get(mapp, MM(i))))
            tfailf(perrtab_SYS, SYS_ENOMEM, "expected : (null), actual : %d", itemp->x);
        }

        for(i = 7; i < x; i++)
        {
          item * itemp = 0;
          if(!(itemp = map_get(mapp, MM(i))))
            tfailf(perrtab_SYS, SYS_ENOMEM, "expected : (found), actual : %d", i);
        }
      }
    }
  }

finally:
  map_free(mapp);
coda;
}

xapi go()
{
  enter;

  fatal(test_primary);
  fatal(test_secondary);
  fatal(test_rehash);

finally:
  if(XAPI_UNWINDING)
  {
    xapi_backtrace();
  }
coda;
}

int main()
{
  go();
  xapi_teardown();
}
