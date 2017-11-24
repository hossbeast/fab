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

#include "map.h"
#include "test_util.h"

#include "macros.h"

typedef struct item
{
  int a;
  int x;
} item;

static void destructor(void * value)
{
  wfree(value);
}

static inline xapi insert(map * restrict m, const char * restrict key, int val)
{
  enter;

  item * itemp;
  fatal(xmalloc, &itemp, sizeof(*itemp));
  itemp->x = val;
  fatal(map_set, m, MMS(key), itemp);

  finally : coda;
}

static inline xapi contains(map * restrict m, const char * restrict key, int val)
{
  enter;

  item * i = map_get(m, MMS(key));
  assert_notnull(i);
  assert_eq_d(val, i->x);

  finally : coda;
}

static inline xapi absent(map * restrict m, const char * restrict key)
{
  enter;

  item * i = map_get(m, MMS(key));
  assert_null(i);

  finally : coda;
}

static xapi validate_enumeration(map * restrict m)
{
  enter;

  int x;
  for(x = 0; x < map_table_size(m); x++)
  {
    const char * key = map_table_key(m, x);
    const item * itemp = map_table_value(m, x);

    if(!key ^ !itemp)
      ufail();

    if(key)
    {
      int val;
      sscanf(key, "%d", &val);
      assert_eq_d(val, itemp->x);
    }
  }

  finally : coda;
}

static xapi validate_keyset(map * restrict m, char ** restrict keyset)
{
  enter;

  int n = sentinel(keyset);
  int x;
  int i;

  // keys in the map are in the keyset
  for(x = 0; x < map_table_size(m); x++)
  {
    const char * key = map_table_key(m, x);
    if(key)
    {
      for(i = 0; i < n; i++)
      {
        if(strcmp(key, keyset[i]) == 0)
          break;
      }

      if(i == n)
        ufailf("key", "in keyset", "not found : %s", key);
    }
  }

  // keys in the keyset are in the map
  for(x = 0; x < n; x++)
  {
    for(i = 0; i < map_table_size(m); i++)
    {
      const char * key = map_table_key(m, i);
      if(key && strcmp(key, keyset[x]) == 0)
          break;
    }

    if(i == map_table_size(m))
      ufailf("key", "in map", "not found : %s", keyset[x]);
  }

  finally : coda;
}

static xapi test_basic()
{
  enter;

  map * m = 0;
  fatal(map_createx, &m, destructor, 0, 0);

  char * keyset[] = {
      "1"
    , "200"
    , "3"
    , 0
  };

  fatal(insert, m, "1", 1);
  fatal(insert, m, "200", 200);
  fatal(insert, m, "3", 3);

  assert_eq_d(3, map_size(m));

  // entries by lookup
  fatal(contains, m, "1", 1);
  fatal(contains, m, "3", 3);
  fatal(contains, m, "200", 200);

  // entries by enumeration
  fatal(validate_enumeration, m);
  fatal(validate_keyset, m, keyset);

finally:
  fatal(map_xfree, m);
coda;
}

static xapi test_load()
{
  enter;

  char space[64];
  map * m = 0;

  fatal(map_createx, &m, destructor, 0, 0);

  int x;
  for(x = 0; x < 5000; x++)
  {
    snprintf(space, sizeof(space), "%d", x);
    fatal(insert, m, space, x);
  }

  assert_eq_d(5000, map_size(m));

  // entries by lookup
  for(x = 0; x < 5000; x++)
  {
    snprintf(space, sizeof(space), "%d", x);
    fatal(contains, m, space, x);
  }

  // entries by enumeration
  fatal(validate_enumeration, m);

finally:
  fatal(map_xfree, m);
coda;
}

static xapi test_delete()
{
  enter;

  map * m = 0;
  fatal(map_createx, &m, destructor, 0, 0);

  char * keyset[] = {
      "1"
    , "200"
    , "3"
    , "37"
    , "9000000"
  };

  fatal(insert, m, "1", 1);
  fatal(insert, m, "200", 200);
  fatal(insert, m, "3", 3);
  fatal(insert, m, "37", 37);
  fatal(insert, m, "9000000", 9000000);

  // successive deletion
  fatal(map_delete, m, MMS(keyset[4]));
  assert_eq_d(4, map_size(m));
  fatal(absent, m, keyset[4]);
  fatal(validate_enumeration, m);
  keyset[4] = 0;
  fatal(validate_keyset, m, keyset);

  fatal(map_delete, m, MMS(keyset[3]));
  assert_eq_d(3, map_size(m));
  fatal(absent, m, keyset[3]);
  fatal(validate_enumeration, m);
  keyset[3] = 0;
  fatal(validate_keyset, m, keyset);

  fatal(map_delete, m, MMS(keyset[2]));
  assert_eq_d(2, map_size(m));
  fatal(absent, m, keyset[2]);
  fatal(validate_enumeration, m);
  keyset[2] = 0;
  fatal(validate_keyset, m, keyset);

  fatal(map_delete, m, MMS(keyset[1]));
  assert_eq_d(1, map_size(m));
  fatal(absent, m, keyset[1]);
  fatal(validate_enumeration, m);
  keyset[1] = 0;
  fatal(validate_keyset, m, keyset);

  fatal(map_delete, m, MMS(keyset[0]));
  assert_eq_d(0, map_size(m));
  fatal(absent, m, keyset[0]);
  fatal(validate_enumeration, m);
  keyset[0] = 0;
  fatal(validate_keyset, m, keyset);

finally:
  fatal(map_xfree, m);
coda;
}

static xapi run_tests()
{
  enter;

  fatal(test_basic);
  fatal(test_load);
  fatal(test_delete);

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
