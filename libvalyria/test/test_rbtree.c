/* Copyright (c) 2012-2019 Todd Freed <todd.freed@gmail.com>

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

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "xapi.h"
#include "xapi/trace.h"
#include "xlinux/xstdlib.h"
#include "llist.h"

#include "rbtree.h"
#include "test_util.h"

struct item {
  llist lln;    // freelist
  char * label;

  /* not at the start, some sanitizers will hit this */
  rbnode rbn;   // rbtree
};

/*
invariants
  for each node, left < self, right > self
  max height is log(size of tree)
  max node height / min node height <= 2
 */
static xapi depth(rbnode * restrict n, int * restrict d)
{
  enter;

  int left;
  int right;

  if(n == &rbleaf)
  {
    *d = 1;
    goto XAPI_FINALIZE;
  }

  fatal(depth, n->left, &left);
  fatal(depth, n->right, &right);

  assert_eq_d(left, right);

  if(n->color == RB_BLACK)
    left++;
  *d = left;

  finally : coda;
}

static xapi validate(rbtree * rb)
{
  enter;

  int d;
  fatal(depth, rb->root, &d);

  if(rb->root == &rbleaf)
    assert_eq_d(1, d);
  else
    assert_eq_p(&rbleaf, rb->root->parent);

  finally : coda;
}

static int item_rbn_key_cmp(void * _a, const rbnode * _b)
{
  struct item * a = containerof(_a, struct item, rbn);
  struct item * b = containerof(_b, struct item, rbn);

  return strcmp(a->label, b->label);
}

static int item_rbn_node_cmp(const rbnode * _a, const rbnode * _b)
{
  struct item * a = containerof(_a, struct item, rbn);
  struct item * b = containerof(_b, struct item, rbn);

  return strcmp(a->label, b->label);
}

static void item_rbn_free(rbnode * _a)
{
  struct item * a = containerof(_a, struct item, rbn);

  free(a);
}

static xapi test_put()
{
  enter;

  rbtree rb;
  llist freelist;
  llist *T;
  struct item *i, *j;
  struct item key;
  int x;
  int c;
  bool found;

  llist_init_node(&freelist);
  rbtree_init(&rb);

  char * labels[] = {
    "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
  };
  for(x = 0; x < sizeof(labels) / sizeof(*labels); x++)
  {
    fatal(xmalloc, &i, sizeof(struct item));
    i->label = labels[x];
    found = rbtree_put(&rb, i, rbn, item_rbn_node_cmp);
    assert_eq_b(false, found);

    // validate redblack properties
    fatal(validate, &rb);

    // for each item thus far inserted
    int y;
    for(y = 0; y <= x; y++)
    {
      // verify reachability by lookup
      key.label = labels[y];
      i = rbtree_lookup(&rb, &key, rbn, item_rbn_key_cmp);

      assert_notnull(i);
      assert_eq_p(labels[y], i->label);

      // verify reachability by traversal
      c = 0;
      found = false;
      rbtree_foreach(&rb, j, rbn) {
        if(j == i) {
          found = true;
        }
        c++;
      }

      assert_eq_b(true, found);
      assert_eq_d(x + 1, c);
    }
  }

finally:
  rbtree_destroy(&rb, item_rbn_free);
  llist_foreach_safe(&freelist, i, lln, T) {
    wfree(i);
  }
coda;
}

static xapi test_delete()
{
  enter;

  rbtree rb;
  llist freelist;
  llist *T;
  struct item *i, *j;
  struct item key;
  int x;
  bool found;
  int c;

  llist_init_node(&freelist);
  rbtree_init(&rb);

  // traverse empty tree
  x = 0;
  rbtree_foreach(&rb, i, rbn) {
    x++;
  }
  assert_eq_d(0, x);

  char * labels[] = {
    "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
  };
  for(x = 0; x < sizeof(labels) / sizeof(*labels); x++)
  {
    fatal(xmalloc, &i, sizeof(struct item));
    i->label = labels[x];
    found = rbtree_put(&rb, i, rbn, item_rbn_node_cmp);
    assert_eq_b(false, found);
  }

  // delete in forward order
  for(x = 0; x < sizeof(labels) / sizeof(*labels); x++)
  {
    key.label = labels[x];
    i = rbtree_lookup(&rb, &key, rbn, item_rbn_key_cmp);

    rbtree_delete(&rb, i, rbn);
    llist_append(&freelist, i, lln);

    // validate redblack properties
    fatal(validate, &rb);

    // for each item remaining
    int y;
    for(y = x + 1; y < sizeof(labels) / sizeof(*labels); y++)
    {
      // verify reachability by lookup
      key.label = labels[y];
      i = rbtree_lookup(&rb, &key, rbn, item_rbn_key_cmp);

      assert_notnull(i);
      assert_eq_p(labels[y], i->label);

      // verify reachability by traversal
      c = 0;
      found = false;
      rbtree_foreach(&rb, j, rbn) {
        if(j == i) {
          found = true;
        }
        c++;
      }

      assert_eq_b(true, found);
      assert_eq_d((sizeof(labels) / sizeof(*labels)) - x - 1, c);
    }
  }

  // traverse empty tree
  x = 0;
  rbtree_foreach(&rb, i, rbn) {
    x++;
  }
  assert_eq_d(0, x);

  for(x = 0; x < sizeof(labels) / sizeof(*labels); x++)
  {
    fatal(xmalloc, &i, sizeof(struct item));
    i->label = labels[x];
    found = rbtree_put(&rb, i, rbn, item_rbn_node_cmp);
    assert_eq_b(false, found);
  }

  // delete in reverse order
  for(x = (sizeof(labels) / sizeof(*labels)) - 1; x >= 0; x--)
  {
    key.label = labels[x];
    i = rbtree_lookup(&rb, &key, rbn, item_rbn_key_cmp);

    rbtree_delete(&rb, i, rbn);
    llist_append(&freelist, i, lln);

    // validate redblack properties
    fatal(validate, &rb);

    // for each item remaining
    int y;
    for(y = 0; y < x; y++)
    {
      // verify reachability by lookup
      key.label = labels[y];
      i = rbtree_lookup(&rb, &key, rbn, item_rbn_key_cmp);

      assert_notnull(i);
      assert_eq_p(labels[y], i->label);

      // verify reachability by traversal
      c = 0;
      found = false;
      rbtree_foreach(&rb, j, rbn) {
        if(j == i) {
          found = true;
        }
        c++;
      }

      assert_eq_b(true, found);
      assert_eq_d(x, c);
    }
  }

  // traverse empty tree
  x = 0;
  rbtree_foreach(&rb, i, rbn) {
    x++;
  }
  assert_eq_d(0, x);

finally:
  rbtree_destroy(&rb, item_rbn_free);
  llist_foreach_safe(&freelist, i, lln, T) {
    wfree(i);
  }
coda;
}

static xapi run_tests()
{
  enter;

  fatal(test_put);
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
