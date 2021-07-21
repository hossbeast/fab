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

#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"

#include "set.h"
#include "test_util.h"

#include "macros.h"
#include "common/assure.h"
#include "common/hash.h"

static void validate_elements(set * restrict s, char ** restrict elements)
{
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
}

static void test_basic()
{
  set * s = 0;

  set_create(&s);

  char * elementset[] = {
      "1"
    , "200"
    , "3"
    , ""
    , 0
  };

  set_put(s, MMS(elementset[0]));
  set_put(s, MMS(elementset[1]));
  set_put(s, MMS(elementset[2]));
  set_put(s, MMS(elementset[3]));

  // assert
  validate_elements(s, elementset);

  set_xfree(s);
}

static void test_recycle()
{
  set * s = 0;
  char * str = 0;

  char space[64];
  int x;
  int y;
  int i;

  set_createx(&s, 0, hash32, memncmp, wfree);

  for(i = 0; i < 2; i++)
  {
    y = i * 1000;
    x = y;
    do {
      snprintf(space, sizeof(space), "%d", x);
      ixstrdup(&str, space);
      set_put(s, MMS(str));
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

    set_recycle(s);
  }

  set_xfree(s);
  wfree(str);
}

static void test_delete()
{
  set * s = 0;

  set_create(&s);

  char * elements[] = {
      "1"
    , "200"
    , "3"
    , "37"
    , "9000000"
    , ""
    , 0
  };

  set_put(s, MMS("1"));
  set_put(s, MMS("200"));
  set_put(s, MMS("3"));
  set_put(s, MMS("37"));
  set_put(s, MMS("9000000"));
  set_put(s, MMS(""));

  // successive deletion
  set_delete(s, MMS(elements[5]));
  assert_eq_b(false, set_contains(s, MMS(elements[5])));
  elements[5] = 0;
  validate_elements(s, elements);

  set_delete(s, MMS(elements[4]));
  assert_eq_b(false, set_contains(s, MMS(elements[4])));
  elements[4] = 0;
  validate_elements(s, elements);

  set_delete(s, MMS(elements[3]));
  assert_eq_b(false, set_contains(s, MMS(elements[3])));
  elements[3] = 0;
  validate_elements(s, elements);

  set_delete(s, MMS(elements[2]));
  assert_eq_b(false, set_contains(s, MMS(elements[2])));
  elements[2] = 0;
  validate_elements(s, elements);

  set_delete(s, MMS(elements[1]));
  assert_eq_b(false, set_contains(s, MMS(elements[1])));
  elements[1] = 0;
  validate_elements(s, elements);

  set_delete(s, MMS(elements[0]));
  assert_eq_b(false, set_contains(s, MMS(elements[0])));
  elements[0] = 0;
  validate_elements(s, elements);

  set_xfree(s);
}

static void test_splice()
{
  set * A = 0;
  set * B = 0;
  char * str = 0;
  char space[64];
  int x;

  set_createx(&A, 0, hash32, memncmp, wfree);
  set_createx(&B, 0, hash32, memncmp, wfree);

  for(x = 0; x < 5000; x++)
  {
    snprintf(space, sizeof(space), "%d", x);
    ixstrdup(&str, space);
    set_put(A, MMS(str));
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

  set_delete(A, MMS("1234"));
  set_delete(A, MMS("1235"));
  set_delete(A, MMS("1236"));

  // replicate A -> B
  set_splice(B, A);

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

  set_xfree(A);
  set_xfree(B);
  wfree(str);
}

static void test_replicate()
{
  set * A = 0;
  set * B = 0;
  char * str = 0;
  char space[64];
  int x;

  set_createx(&A, 0, hash32, memncmp, wfree);
  set_createx(&B, 0, hash32, memncmp, 0);

  for(x = 0; x < 5000; x++)
  {
    snprintf(space, sizeof(space), "%d", x);
    ixstrdup(&str, space);
    set_put(A, MMS(str));
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

  set_delete(A, MMS("1234"));
  set_delete(A, MMS("1235"));
  set_delete(A, MMS("1236"));

  // replicate A -> B
  set_replicate(B, A);

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

  set_xfree(A);
  set_xfree(B);
  wfree(str);
}

int main()
{
  test_basic();
  test_recycle();
  test_delete();
  test_replicate();
  test_splice();

  summarize;

  return 0;
}
