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

struct item;
#define DICTIONARY_VALUE_TYPE struct item
#include "dictionary.h"

#include "macros.h"
#include "test_util.h"

struct item
{
  int a;
  int x;
};
typedef struct item item;

static xapi validate(dictionary * dp)
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
    ufail();

  itemp = dictionary_get(dp, MMS("two"));
  if(itemp->x != 2)
    ufail();

  itemp = dictionary_get(dp, MMS("three"));
  if(itemp->x != 3)
    ufail();

  // lists of keys
  fatal(dictionary_keys, dp, &keys, &keysl);

  if(keysl != 3)
    ufail();

  int stringsort(const void * A, const void * B) { return strcmp(*(char **)A, *(char**)B); };
  qsort(keys, keysl, sizeof(*keys), stringsort);

  if(strcmp(keys[0], "one"))
    ufailf("%s", "%s", "one", keys[0]);

  if(strcmp(keys[1], "three"))
    ufailf("%s", "%s", "three", keys[1]);

  if(strcmp(keys[2], "two"))
    ufailf("%s", "%s", "two", keys[2]);

  // list of values
  fatal(dictionary_values, dp, &values, &valuesl);

  if(valuesl != 3)
    ufail();

  int itemsort(const void * A, const void * B) { return (*(item **)A)->x - (*(item **)B)->x; };
  qsort(values, valuesl, sizeof(*values), itemsort);

  if(values[0]->x != 1)
    ufailf("%d", "%d", 1, values[0]->x);

  if(values[1]->x != 2)
    ufailf("%d", "%d", 2, values[1]->x);

  if(values[2]->x != 3)
    ufailf("%d", "%d", 3, values[2]->x);

  // by slot
  int x;
  for(x = 0; x < dictionary_table_size(dp); x++)
  {
    const char * key = dictionary_table_key(dp, x);
    itemp = dictionary_table_value(dp, x);

    if(!key ^ !itemp)
      ufail();

    if(key)
    {
      if(strcmp(key, "one") == 0)
      {
        if(itemp->x != 1)
          ufailf("%d", "%d", 1, itemp->x);
      }
      else if(strcmp(key, "two") == 0)
      {
        if(itemp->x != 2)
          ufailf("%d", "%d", 2, itemp->x);
      }
      else if(strcmp(key, "three") == 0)
      {
        if(itemp->x != 3)
          ufailf("%d", "%d", 3, itemp->x);
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

static xapi test()
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
  fatal(dictionary_xfree, dp);
coda;
}

static xapi test_rehash()
{
  enter;

  dictionary * dp = 0;
  fatal(dictionary_createx, &dp, sizeof(item), 0, 0, 2);

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
          fatal(dictionary_delete, dp, MM(i));
      }

      if(x >= 7)
      {
        int i;
        for(i = 0; i < 7; i++)
        {
          item * itemp = 0;
          if((itemp = dictionary_get(dp, MM(i))))
            ufailf("%s", "%d", "(null)", itemp->x);
        }

        for(i = 7; i < x; i++)
        {
          item * itemp = 0;
          if(!(itemp = dictionary_get(dp, MM(i))))
            ufailf("%s", "%d", "(found)", i);
        }
      }
    }
  }

finally:
  fatal(dictionary_xfree, dp);
coda;
}

int main()
{
  enter;

  xapi r;

  fatal(test);
  fatal(test_rehash);

  success;

finally:
  if(XAPI_UNWINDING)
  {
    xapi_backtrace();
  }
conclude(&r);

  xapi_teardown();
  return !!r;
}
