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
#define DICTIONARY_VALUE_TYPE struct item
#include "dictionary.h"

#include "macros.h"

struct item
{
  int a;
  int x;
};
typedef struct item item;

xapi validate(dictionary * dp)
{
  enter;

  const char ** keys = 0;
  size_t keysl = 0;
  item ** values = 0;
  size_t valuesl = 0;

  // retrieve by key
  item * itemp;
  itemp = dictionary_get(dp, MMS("one"));
  if(itemp->x != 1)
    tfail(perrtab_SYS, SYS_ENOMEM);

  itemp = dictionary_get(dp, MMS("two"));
  if(itemp->x != 2)
    tfail(perrtab_SYS, SYS_ENOMEM);

  itemp = dictionary_get(dp, MMS("three"));
  if(itemp->x != 3)
    tfail(perrtab_SYS, SYS_ENOMEM);

  // lists of keys
  fatal(dictionary_keys, dp, &keys, &keysl);

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
  fatal(dictionary_values, dp, &values, &valuesl);

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
  for(x = 0; x < dictionary_table_size(dp); x++)
  {
    const char * key = dictionary_table_key(dp, x);
    itemp = dictionary_table_value(dp, x);

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

xapi test()
{
  enter;

  dictionary * dp = 0;
  fatal(dictionary_create, &dp, sizeof(item));

  item * itemp = 0;
  fatal(dictionary_set, dp, MMS("one"), &itemp);
  itemp->x = 1;

  fatal(dictionary_set, dp, MMS("two"), &itemp);
  itemp->x = 2;

  fatal(dictionary_set, dp, MMS("three"), &itemp);
  itemp->x = 3;

  fatal(validate, dp);

finally:
  dictionary_free(dp);
coda;
}

xapi test_rehash()
{
  enter;

  dictionary * dp = 0;
  fatal(dictionary_createx, &dp, sizeof(item), 0, 2);

  int x;
  for(x = 0; x <= 35; x++)
  {
    item * itemp = 0;
    fatal(dictionary_set, dp, MM(x), &itemp);
    itemp->x = x;

    if(x == 1 || x == 7 || x == 35)
    {
      if(x == 7)
      {
        int i;
        for(i = 0; i < 7; i++)
        {
          if(!dictionary_delete(dp, MM(i)))
            tfailf(perrtab_SYS, SYS_ENOMEM, "expected : (found), actual : %d", i);
        }
      }

      if(x >= 7)
      {
        int i;
        for(i = 0; i < 7; i++)
        {
          item * itemp = 0;
          if((itemp = dictionary_get(dp, MM(i))))
            tfailf(perrtab_SYS, SYS_ENOMEM, "expected : (null), actual : %d", itemp->x);
        }

        for(i = 7; i < x; i++)
        {
          item * itemp = 0;
          if(!(itemp = dictionary_get(dp, MM(i))))
            tfailf(perrtab_SYS, SYS_ENOMEM, "expected : (found), actual : %d", i);
        }
      }
    }
  }

finally:
  dictionary_free(dp);
coda;
}

xapi go()
{
  enter;

  fatal(test);
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
