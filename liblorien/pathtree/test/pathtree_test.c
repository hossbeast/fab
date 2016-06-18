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
#include "narrator.h"
#include "narrator/file.h"

#include "xlinux.h"
#include "xlinux/SYS.errtab.h"

struct pathtree_test;
#define TEST_TYPE struct pathtree_test
#include "xunit.h"
#include "xunit/logging.h"
#include "xunit/assert.h"

#include "logger.h"

struct node;
#define LIST_ELEMENT_TYPE struct node
#include "valyria/list.h"

#define PATHTREE_VALUE_TYPE char *
#include "pathtree.internal.h"
#include "pathtree_test.h"

#include "strutil.h"
#include "macros.h"

//
// public
//

xapi pathtree_validate(node * const restrict n, node * const restrict parent)
{
  enter;

  // prefix length
  assertf(n->pfxl == strlen(n->pfx ?: ""), "%zu", "%zu", strlen(n->pfx ?: ""), n->pfxl);

  // prefix always begins with a slash
  assertf(n->pfxl == 0 || n->pfx[0] == '/', "%c", "%c", '/', n->pfx[0]);

  // keyl must be accurate
  size_t parent_keyl = parent ? parent->keyl : 0;
  assertf(n->keyl == (parent_keyl + n->pfxl), "%zu", "%zu", parent_keyl, n->keyl);

  // children must be unique and sorted asc
  int x;
  for(x = 1; x < n->children->l; x++)
  {
    node * A = list_get(n->children, x - 1);
    node * B = list_get(n->children, x);
    int r = estrcmp(A->pfx, A->pfxl, B->pfx, B->pfxl, 0);
    assertf(r < 0, "%.*s < %.*s", "%.*s %s %.*s"
      , A->pfxl, A->pfx, B->pfxl, B->pfx
      , A->pfxl, A->pfx, r ? ">" : "=", B->pfxl, B->pfx
    );
  }

  for(x = 0; x < n->children->l; x++)
    fatal(pathtree_validate, list_get(n->children, x), n);

  finally : coda;
}

xapi pathtree_test_entry(pathtree_test * test)
{
  enter;

  pathtree * pt = 0;
  fatal(pathtree_create, &pt);

  // build the tree
  int y;
  for(y = 0; y < sentinel(test->entries); y++)
    fatal(pathtree_insert, pt, test->entries[y].key, &test->entries[y].key);

  // assert that the tree satisfies certain properties
  fatal(pathtree_validate, pt->root, 0);

  // search for each key
  for(y = 0; y < sentinel(test->entries); y++)
  {
    char ** val = pathtree_search(pt, test->entries[y].key);
    char * expected = test->entries[y].key;
    char * actual = val ? *val : "(none)";

    assertf(strcmp(expected, actual) == 0, "%s", "%s", expected, actual);
  }

  // additional queries
  if(test->queries)
  {
    for(y = 0; y < sentinel(test->queries); y++)
    {
      char ** val = pathtree_search(pt, test->queries[y].key);
      char * expected = test->queries[y].val ?: "(none)";
      char * actual = val ? *val : "(none)";

      assertf(strcmp(expected, actual) == 0, "%s", "%s", expected, actual);
    }
  }

finally:
  pathtree_free(pt);
coda;
}

xapi pathtree_unit_setup(xunit_unit * restrict unit)
{
  enter;

  int x;
  for(x = 0; x < sentinel(unit->tests); x++)
    unit->tests[x]->entry = pathtree_test_entry;

  finally : coda;
}
