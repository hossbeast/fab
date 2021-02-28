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

#ifndef _VALYRIA_RBTREE_H
#define _VALYRIA_RBTREE_H

/*

*/

#include "types.h"
#include "llist.h"

#define RBTREE_HASH  0
#define RBTREE_SIZE  0
#define RBTREE_TRACK 0

struct rbnode;

enum rbcolor {
  RB_RED = 1,
  RB_BLACK,
};

typedef struct rbnode {
  enum rbcolor color;
  struct rbnode * parent;
  struct rbnode * left;
  struct rbnode * right;
  union {
    uint64_t u64;
    void *p;
  } ud;
  llist lln;      // adhoc

#if RBTREE_HASH
  struct rbtree *rbt;
#endif
#if RBTREE_TRACK
  int deleted;
#endif
} rbnode;

extern rbnode rbleaf;

typedef struct rbtree {
  struct rbnode * root;

#if RBTREE_SIZE
  size_t size;            // number of active nodes
#endif
#if RBTREE_HASH
  uint32_t hash;
#endif
} rbtree;

typedef int (*rbtree_node_cmp)(const rbnode * restrict a, const rbnode * restrict b)
  __attribute__((nonnull));

static inline int rbtree_node_cmp_node(const rbnode * restrict a, const rbnode * restrict b)
{
  return a > b ? 1 : a < b ? -1 : 0;
}

typedef int (*rbtree_key_cmp)(void * restrict key, const rbnode * restrict n)
  __attribute__((nonnull));

typedef void (*rbtree_free)(rbnode * restrict n);

/// rbtree_create
//
// SUMMARY
//  initialize an rbtree
//
// PARAMETERS
//  list         - created list goes here
//  [capacity]   - initial capacity
//  [free_item]  - invoked on an element before releasing its storage
//  [xfree_item] - invoked on an element before releasing its storage
//
// REMARKS
//  free_item follows the free idiom
//
void rbtree_init(rbtree * restrict rb)
  __attribute__((nonnull));

void rbtree_destroy(rbtree * restrict rb, rbtree_free free_fn)
  __attribute__((nonnull));

bool rbtree_empty(rbtree * restrict rb)
  __attribute__((nonnull));

void rbnode_init(rbnode * restrict rbn)
  __attribute__((nonnull));

bool rbnode_attached(rbnode * restrict rbn)
  __attribute__((nonnull));

size_t rbtree_count(const rbtree * restrict rb)
  __attribute__((nonnull));

/*
 * add a node to the tree
 *
 * if the node already exists in the tree, returns the existing node, NULL otherwise
 */
rbnode * rbtree_put_node(rbtree * restrict rb, rbnode * restrict n, rbtree_node_cmp cmp)
  __attribute__((nonnull));

#define rbtree_put(rb, item, member, cmp) ({  \
  rbtree_put_node(rb, &(item)->member, cmp);  \
})

typedef struct rbtree_search_context {
  struct rbnode * last;
  int lx;
} rbtree_search_context;

void rbtree_insert_node(rbtree * restrict rb, rbtree_search_context * restrict ctx, rbnode * restrict n)
  __attribute__((nonnull));

/*
 * replace one node with another
 *
 * @n - node to be replaced
 * @new - replacement node
 */
void rbtree_replace_node(rbnode * restrict n, rbnode * restrict new)
  __attribute__((nonnull));

#define rbtree_replace(n, new, member) ({             \
  rbtree_replace_node(&(n)->member, &(new)->member);  \
})

void rbtree_delete_node(rbtree * restrict rb, rbnode * restrict n)
  __attribute__((nonnull));

#if DEBUG || DEVEL
#define rbtree_delete(rb, item, member) ({    \
  rbtree_delete_node(rb, &(item)->member);    \
  (item)->member.color = 0;                   \
  (item)->member.parent = 0;                  \
  (item)->member.left = 0;                    \
  (item)->member.right = 0;                   \
})
#else
#define rbtree_delete(rb, item, member) ({    \
  rbtree_delete_node(rb, &(item)->member);    \
})
#endif

void rbtree_print(rbtree * rb);
void rbnode_print(rbnode * n);

rbnode * rbtree_search(const rbtree * restrict rb, rbtree_search_context *ctx, void *key, rbtree_key_cmp cmp)
  __attribute__((nonnull));

rbnode * rbtree_lookup_node(const rbtree * restrict rb, void * key, rbtree_key_cmp cmp)
  __attribute__((nonnull));

/*
 * lookup an item in the tree
 */
#define rbtree_lookup(rb, key, member, cmp)                     \
  ({                                                            \
    rbnode * __n = rbtree_lookup_node(rb, &(key)->member, cmp); \
    typeof(*key) *__i = 0;                                      \
    if (__n) {                                                  \
      __i = containerof(__n, typeof(*key), member);             \
    }                                                           \
    __i;                                                        \
  })

struct rbnode * rbtree_first_post_node(rbtree * restrict rb)
  __attribute__((nonnull));
struct rbnode * rbtree_last_post_node(rbtree * restrict rb)
  __attribute__((nonnull));
struct rbnode * rbtree_next_post_node(rbnode * restrict n);

#define rbtree_first(rb, type, member) ({              \
  rbnode * n;                                          \
  n = rbtree_first_post_node(rb);              \
  n == &rbleaf ? NULL : containerof(n, type, member);  \
})

#define rbtree_next(item, member) ({                   \
  if(item) {                                           \
    rbnode * n = rbtree_next_post_node(&(item)->member);        \
    item = (n == &rbleaf) ? NULL : containerof(n, typeof(*item), member);  \
  }                                                     \
  item;                                                 \
})

#define rbtree_is_last_node(rb, rbn) ({  \
  rbn == rbtree_last_post_node(rb);      \
})

#define rbtree_is_last(rb, item, member) ({                                 \
  item == containerof(rbtree_last_post_node(rb), typeof(*item), member);    \
})

#define rbtree_foreach_node(rb, rbn)      \
  for(                                    \
    rbn = rbtree_first_post_node(rb);     \
    rbn != &rbleaf;                       \
    rbn = rbtree_next_post_node(rbn)      \
  )

#define rbtree_foreach(rb, item, member)                                                \
  for(                                                                                  \
    item = containerof(rbtree_first_post_node(rb), typeof(*item), member);              \
    item != containerof(&rbleaf, typeof(*item), member);                                \
    item = containerof(rbtree_next_post_node(&(item)->member), typeof(*item), member)   \
  )

#endif
