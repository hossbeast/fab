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
#include "xlinux/xstring.h"

#include "set.h"
#include "test_util.h"

#include "macros.h"
#include "assure.h"
#include "hash.h"

static xapi validate_elements(set * restrict s, char ** restrict elements)
{
  enter;

  int n = sentinel(elements);
  int x;
  int i;
  const char *el;
  size_t elen;

  assert_eq_d(n, s->size);

  // elements in the set are in elements
  for(x = 0; x < s->table_size; x++)
  {
    if(set_table_element(s, x, &el, &elen))
    {
      for(i = 0; i < n; i++)
      {
        if(memncmp(el, elen, MMS(elements[i])) == 0)
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
      if(set_table_element(s, i, &el, &elen) && memncmp(el, elen, MMS(elements[x])) == 0)
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
    , ""
    , 0
  };

  fatal(set_put, s, MMS(elementset[0]));
  fatal(set_put, s, MMS(elementset[1]));
  fatal(set_put, s, MMS(elementset[2]));
  fatal(set_put, s, MMS(elementset[3]));

  // assert
  fatal(validate_elements, s, elementset);

finally:
  fatal(set_xfree, s);
coda;
}

static xapi test_recycle()
{
  enter;

  set * s = 0;
  char * str = 0;

  char space[64];
  int x;
  int y;
  int i;

  fatal(set_createx, &s, 0, hash32, memncmp, wfree, 0);

  for(i = 0; i < 2; i++)
  {
    y = i * 1000;
    x = y;
    do {
      snprintf(space, sizeof(space), "%d", x);
      fatal(ixstrdup, &str, space);
      fatal(set_put, s, MMS(str));
      str = 0;
      x = (x + 1) % 5000;
    } while(x != y);

    assert_eq_d(5000, s->size);

    // lookup entries
    for(x = 0; x < 5000; x++)
    {
      snprintf(space, sizeof(space), "%d", x);
      assert_eq_b(true, set_contains(s, MMS(space)));
    }

    assert_eq_b(false, set_contains(s, MMS("foobar")));
    assert_eq_b(false, set_contains(s, MMS("09123")));

    fatal(set_recycle, s);
  }

finally:
  fatal(set_xfree, s);
  wfree(str);
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
    , ""
    , 0
  };

  fatal(set_put, s, MMS("1"));
  fatal(set_put, s, MMS("200"));
  fatal(set_put, s, MMS("3"));
  fatal(set_put, s, MMS("37"));
  fatal(set_put, s, MMS("9000000"));
  fatal(set_put, s, MMS(""));

  // successive deletion
  fatal(set_delete, s, MMS(elements[5]));
  assert_eq_b(false, set_contains(s, MMS(elements[5])));
  elements[5] = 0;
  fatal(validate_elements, s, elements);

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

static xapi test_splice()
{
  enter;

  set * A = 0;
  set * B = 0;
  char * str = 0;
  char space[64];
  int x;

  fatal(set_createx, &A, 0, hash32, memncmp, wfree, 0);
  fatal(set_createx, &B, 0, hash32, memncmp, wfree, 0);

  for(x = 0; x < 5000; x++)
  {
    snprintf(space, sizeof(space), "%d", x);
    fatal(ixstrdup, &str, space);
    fatal(set_put, A, MMS(str));
    str = 0;
  }

  // lookup in A
  assert_eq_d(5000, A->size);
  for(x = 0; x < 5000; x++)
  {
    snprintf(space, sizeof(space), "%d", x);
    assert_eq_b(true, set_contains(A, MMS(space)));
  }

  assert_eq_b(false, set_contains(A, MMS("foobar")));
  assert_eq_b(false, set_contains(A, MMS("09123")));
  assert_eq_b(false, set_contains(B, MMS("foobar")));
  assert_eq_b(false, set_contains(B, MMS("09123")));

  fatal(set_delete, A, MMS("1234"));
  fatal(set_delete, A, MMS("1235"));
  fatal(set_delete, A, MMS("1236"));

  // replicate A -> B
  fatal(set_splice, B, A);

  assert_eq_d(0, A->size);
  assert_eq_d(4997, B->size);

  // lookup in A and B
  for(x = 0; x < 5000; x++)
  {
    if(x == 1234 || x == 1235 || x == 1236)
      continue;

    snprintf(space, sizeof(space), "%d", x);
    assert_eq_b(false, set_contains(A, MMS(space)));
    assert_eq_b(true, set_contains(B, MMS(space)));
  }

  assert_eq_b(false, set_contains(A, MMS("foobar")));
  assert_eq_b(false, set_contains(A, MMS("09123")));
  assert_eq_b(false, set_contains(B, MMS("foobar")));
  assert_eq_b(false, set_contains(B, MMS("09123")));

finally:
  fatal(set_xfree, A);
  fatal(set_xfree, B);
  wfree(str);
coda;
}

static xapi test_replicate()
{
  enter;

  set * A = 0;
  set * B = 0;
  char * str = 0;
  char space[64];
  int x;

  fatal(set_createx, &A, 0, hash32, memncmp, wfree, 0);
  fatal(set_createx, &B, 0, hash32, memncmp, 0, 0);

  for(x = 0; x < 5000; x++)
  {
    snprintf(space, sizeof(space), "%d", x);
    fatal(ixstrdup, &str, space);
    fatal(set_put, A, MMS(str));
    str = 0;
  }

  // lookup in A
  assert_eq_d(5000, A->size);
  for(x = 0; x < 5000; x++)
  {
    snprintf(space, sizeof(space), "%d", x);
    assert_eq_b(true, set_contains(A, MMS(space)));
  }

  assert_eq_b(false, set_contains(A, MMS("foobar")));
  assert_eq_b(false, set_contains(A, MMS("09123")));
  assert_eq_b(false, set_contains(B, MMS("foobar")));
  assert_eq_b(false, set_contains(B, MMS("09123")));

  fatal(set_delete, A, MMS("1234"));
  fatal(set_delete, A, MMS("1235"));
  fatal(set_delete, A, MMS("1236"));

  // replicate A -> B
  fatal(set_replicate, B, A);

  assert_eq_d(4997, A->size);
  assert_eq_d(4997, B->size);

  // lookup in A and B
  for(x = 0; x < 5000; x++)
  {
    if(x == 1234 || x == 1235 || x == 1236)
      continue;

    snprintf(space, sizeof(space), "%d", x);
    assert_eq_b(true, set_contains(A, MMS(space)));
    assert_eq_b(true, set_contains(B, MMS(space)));
  }

  assert_eq_b(false, set_contains(A, MMS("foobar")));
  assert_eq_b(false, set_contains(A, MMS("09123")));
  assert_eq_b(false, set_contains(B, MMS("foobar")));
  assert_eq_b(false, set_contains(B, MMS("09123")));

finally:
  fatal(set_xfree, A);
  fatal(set_xfree, B);
  wfree(str);
coda;
}

static xapi run_tests()
{
  enter;

  fatal(test_basic);
  fatal(test_recycle);
  fatal(test_delete);
  fatal(test_replicate);
  fatal(test_splice);

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
