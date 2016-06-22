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

#include <inttypes.h>

#include "xapi.h"
#include "xlinux/xstdlib.h"
#include "narrator.h"
#include "narrator/file.h"

struct pathtree_test;
#define TEST_TYPE struct pathtree_test
#include "xunit.h"
#include "xunit/assert.h"

#include "valyria/list.h"

#define PATHTREE_VALUE_TYPE char *
#include "pathtree.internal.h"
#include "pathtree_test.h"

#include "macros.h"
#include "permute.h"
#include "cksum.h"

/*

TEST
  test_order

SUMMARY
  verifies the property of order invariance, that is, for any set of keys, the order in which the
  keys are inserted does not matter

*/

static uint64_t hashtree(const node * const n)
{
  uint64_t hash = cksum64(n->pfx, n->pfxl);
  int x;
  for(x = 0; x < n->children->l; x++)
    hash += hashtree(list_get(n->children, x));

  return hash;
}

static xapi pathtree_order_test_entry(pathtree_test * test)
{
  enter;

  pathtree * pt = 0;

  uint8_t * indexes = 0;
  size_t indexesl = sentinel(test->entries);

  fatal(xmalloc, &indexes, sizeof(*indexes) * indexesl);
  int x;
  for(x = 0; x < indexesl; x++)
    indexes[x] = x;

  uint64_t hash[2] = { 0 };
  for(x = 0; permute(indexes, indexesl); x++)
  {
    pathtree_ifree(&pt);
    fatal(pathtree_create, &pt);

    // build the tree
    int y;
    for(y = 0; y < sentinel(test->entries); y++)
      fatal(pathtree_insert, pt, test->entries[y].key, &test->entries[y].key);

    fatal(pathtree_validate, pt->root, 0);

    // calculate a hash of the tree
    hash[0] = hash[1];
    hash[1] = hashtree(pt->root);

    if(hash[0] && hash[1])
      assertf(hash[0] == hash[1], "%"PRIu64, "%"PRIu64, hash[0], hash[1]);

  } while(permute(indexes, indexesl));

finally:
  pathtree_free(pt);
  free(indexes);
coda;
}

xunit_unit xunit = {
    tests : (xunit_test*[]) {
      (pathtree_test[]){{ entry : pathtree_order_test_entry
        , entries : (typeof(*((pathtree_test*)0)->entries)[]) {
            { key : "/ab" }
          , { key : "/abc" }
          , { key : "/a/b" }
          , { key : "/ab/c" }
          , { key : "/a/bc" }
          , { key : "/a/b/c" }
          , { key : "/a/bc/de/fg" }
          , { }
        }
      }}
    , 0
  }
};
