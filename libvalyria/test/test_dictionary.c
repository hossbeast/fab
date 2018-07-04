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

#include <stdio.h>
#include <stdlib.h>

#include "xapi.h"
#include "xapi/trace.h"
#include "xlinux/xstdlib.h"

#include "dictionary.internal.h"
#include "test_util.h"

#include "macros.h"

typedef struct item
{
  int a;
  int x;
} item;

static xapi validate_int_table_values(dictionary * restrict m)
{
  enter;

  int x;
  for(x = 0; x < m->table_size; x++)
  {
    const char * key = dictionary_table_key(m, x);
    const int * val = dictionary_table_value(m, x);

    assert_eq_b(!key, !val);

    if(key)
    {
      int keyval;
      sscanf(key, "%d", &keyval);
      assert_eq_d(keyval, *val);
    }
  }

  finally : coda;
}

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
  assert_eq_d(1, itemp->x);

  itemp = dictionary_get(dp, MMS("two"));
  assert_eq_d(2, itemp->x);

  itemp = dictionary_get(dp, MMS("three"));
  assert_eq_d(3, itemp->x);

  // lists of keys
  fatal(dictionary_keys, dp, &keys, &keysl);

  assert_eq_zu(3, keysl);

  int stringsort(const void * A, const void * B) { return strcmp(*(char **)A, *(char**)B); };
  qsort(keys, keysl, sizeof(*keys), stringsort);

  assert_eq_s("one", keys[0]);
  assert_eq_s("three", keys[1]);
  assert_eq_s("two", keys[2]);

  // list of values
  fatal(dictionary_values, dp, &values, &valuesl);

  assert_eq_zu(3, valuesl);

  int itemsort(const void * A, const void * B) { return (*(item **)A)->x - (*(item **)B)->x; };
  qsort(values, valuesl, sizeof(*values), itemsort);

  assert_eq_d(1, values[0]->x);
  assert_eq_d(2, values[1]->x);
  assert_eq_d(3, values[2]->x);

  // by slot
  int x;
  for(x = 0; x < dp->table_size; x++)
  {
    const char * key = dictionary_table_key(dp, x);
    itemp = dictionary_table_value(dp, x);

    if(!key ^ !itemp)
      ufail();

    if(key)
    {
      if(strcmp(key, "one") == 0)
      {
        assert_eq_d(1, itemp->x);
      }
      else if(strcmp(key, "two") == 0)
      {
        assert_eq_d(2, itemp->x);
      }
      else if(strcmp(key, "three") == 0)
      {
        assert_eq_d(3, itemp->x);
      }
      else
      {
        ufail();
      }
    }
  }

finally:
  wfree(keys);
  wfree(values);
coda;
}

static xapi test_basic()
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

static xapi test_load()
{
  enter;

  char space[64];
  dictionary * m = 0;
  int * val;
  int x;
  int y;
  int i;

  fatal(dictionary_create, &m, sizeof(int));

  for(i = 0; i < 2; i++)
  {
    y = i * 1000;
    x = y;
    do {
      snprintf(space, sizeof(space), "%d", x);
      fatal(dictionary_set, m, space, strlen(space), &val);
      *val = x;

      x = (x + 1) % 5000;
    } while(x != y);

    assert_eq_d(5000, m->count);

    // entries by lookup
    for(x = 0; x < 5000; x++)
    {
      snprintf(space, sizeof(space), "%d", x);
      val = dictionary_get(m, space, strlen(space));
      assert_eq_d(x, *val);
    }

    // entries by enumeration
    fatal(validate_int_table_values, m);
    fatal(dictionary_recycle, m);
  }

finally:
  fatal(dictionary_xfree, m);
coda;
}

static xapi test_rehash()
{
  enter;

  dictionary * dp = 0;
  fatal(dictionary_createx, &dp, sizeof(item), 2, 0, 0);

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
          item * itemp = dictionary_get(dp, MM(i));
          assert_null(itemp);
        }

        for(i = 7; i < x; i++)
        {
          item * itemp = dictionary_get(dp, MM(i));
          assert_notnull(itemp);
        }
      }
    }
  }

finally:
  fatal(dictionary_xfree, dp);
coda;
}

static xapi run_tests()
{
  enter;

  fatal(test_basic);
  fatal(test_load);
  fatal(test_rehash);

  summarize;

  finally : coda;
}

int main()
{
  enter;

  xapi R = 0;
  fatal(run_tests);

finally:
  if(XAPI_UNWINDING)
    xapi_backtrace();
conclude(&R);
  xapi_teardown();

  return !!R;
}
