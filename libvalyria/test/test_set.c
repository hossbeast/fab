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

#include "set.h"
#include "test_util.h"

#include "macros.h"

static xapi validate_elements(set * restrict s, char ** restrict elements)
{
  enter;

  int n = sentinel(elements);
  int x;
  int i;
  const char *element;

  assert_eq_d(n, s->count);

  // elements in the set are in elements
  for(x = 0; x < s->table_size; x++)
  {
    if(set_table_element(s, x, &element, 0))
    {
      for(i = 0; i < n; i++)
      {
        if(strcmp(element, elements[i]) == 0)
          break;
      }

      assert_le_d(n, i);
    }
  }

  // elements in elements are in the set
  for(x = 0; x < n; x++)
  {
    for(i = 0; i < s->table_size; i++)
    {
      if(set_table_element(s, i, &element, 0) && strcmp(element, elements[x]) == 0)
          break;
    }

    assert_le_d(s->table_size, i);

    // and is lookupable
    assert_eq_b(true, set_contains(s, MMS(elements[x])));
  }

  finally : coda;
}

static xapi test_basic()
{
  enter;

  set * s = 0;

  fatal(set_create, &s);

  char * elementset[] = {
      "1"
    , "200"
    , "3"
    , 0
  };

  fatal(set_put, s, MMS("1"));
  fatal(set_put, s, MMS("200"));
  fatal(set_put, s, MMS("3"));

  // assert
  fatal(validate_elements, s, elementset);

finally:
  fatal(set_xfree, s);
coda;
}

static xapi test_recycle()
{
  enter;

  char space[64];
  set * s = 0;
  int x;
  int y;
  int i;

  fatal(set_create, &s);

  for(i = 0; i < 2; i++)
  {
    y = i * 1000;
    x = y;
    do {
      snprintf(space, sizeof(space), "%d", x);
      fatal(set_put, s, MMS(space));
      x = (x + 1) % 5000;
    } while(x != y);

    assert_eq_d(5000, s->count);

    // entries by lookup
    for(x = 0; x < 5000; x++)
    {
      snprintf(space, sizeof(space), "%d", x);
      assert_eq_b(true, set_contains(s, MMS(space)));
    }

    // entries by enumeration
    fatal(set_recycle, s);
  }

finally:
  fatal(set_xfree, s);
coda;
}

static xapi test_delete()
{
  enter;

  set * s = 0;

  fatal(set_create, &s);

  char * elements[] = {
      "1"
    , "200"
    , "3"
    , "37"
    , "9000000"
    , 0
  };

  fatal(set_put, s, MMS("1"));
  fatal(set_put, s, MMS("200"));
  fatal(set_put, s, MMS("3"));
  fatal(set_put, s, MMS("37"));
  fatal(set_put, s, MMS("9000000"));

  // successive deletion
  fatal(set_delete, s, MMS(elements[4]));
  assert_eq_b(false, set_contains(s, MMS(elements[4])));
  elements[4] = 0;
  fatal(validate_elements, s, elements);

  fatal(set_delete, s, MMS(elements[3]));
  assert_eq_b(false, set_contains(s, MMS(elements[3])));
  elements[3] = 0;
  fatal(validate_elements, s, elements);

  fatal(set_delete, s, MMS(elements[2]));
  assert_eq_b(false, set_contains(s, MMS(elements[2])));
  elements[2] = 0;
  fatal(validate_elements, s, elements);

  fatal(set_delete, s, MMS(elements[1]));
  assert_eq_b(false, set_contains(s, MMS(elements[1])));
  elements[1] = 0;
  fatal(validate_elements, s, elements);

  fatal(set_delete, s, MMS(elements[0]));
  assert_eq_b(false, set_contains(s, MMS(elements[0])));
  elements[0] = 0;
  fatal(validate_elements, s, elements);

finally:
  fatal(set_xfree, s);
coda;
}

static xapi run_tests()
{
  enter;

  fatal(test_basic);
  fatal(test_recycle);
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
