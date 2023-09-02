/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/* red-black tree */

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


/* initialize an rbtree */
void rbtree_init(rbtree * restrict rb)
  __attribute__((nonnull));

/*
 * destroy an rbtree
 *
 * [free_fn] - invoked on an element before releasing it
 */
void rbtree_destroy(rbtree * restrict rb, void (*free_fn)(rbnode *))
  __attribute__((nonnull(1)));

static inline __attribute__((nonnull)) bool rbtree_empty(const rbtree * restrict rb)
{
  return rb->root == &rbleaf;
}

void rbnode_init(rbnode * restrict rbn)
  __attribute__((nonnull));

static inline __attribute__((nonnull)) bool rbnode_attached(const rbnode * restrict rbn)
{
  return !!rbn->parent;
}

size_t rbtree_count(const rbtree * restrict rb)
  __attribute__((nonnull));

/*
 * add a node to the tree
 *
 * if the node already exists in the tree, returns the existing node, NULL otherwise
 */
rbnode * rbtree_put(
    rbtree * restrict rb
  , rbnode * restrict n
  , __attribute__((nonnull)) int (*cmp)(const rbnode * restrict, const rbnode * restrict)
)
  __attribute__((nonnull));

typedef struct rbtree_search_context {
  struct rbnode * last;
  int lx;
} rbtree_search_context;

void rbtree_insert(rbtree * restrict rb, rbtree_search_context * restrict ctx, rbnode * restrict n)
  __attribute__((nonnull));

/*
 * replace one node with another
 *
 * rb  - rbtree
 * old - node to be replaced
 * n   - replacement node
 */
void rbtree_replace(rbtree * restrict rb, rbnode * restrict old, rbnode * restrict n)
  __attribute__((nonnull));

void rbtree_delete(rbtree * restrict rb, rbnode * restrict n)
  __attribute__((nonnull));

rbnode * rbtree_search(
    const rbtree * restrict rb
  , rbtree_search_context *ctx
  , void * key
  , __attribute__((nonnull)) int (*cmp)(void * restrict key, const rbnode * restrict n)
)
  __attribute__((nonnull));

rbnode * rbtree_lookup(
    const rbtree * restrict rb
  , void * key
  , __attribute__((nonnull)) int (*cmp)(void * restrict key, const rbnode * restrict n)
)
  __attribute__((nonnull));

struct rbnode * rbtree_first_post(const rbtree * restrict rb)
  __attribute__((nonnull));

static inline __attribute__((nonnull)) struct rbnode * rbtree_last_post(rbtree * restrict rb)
{
  return rb->root;
}

struct rbnode * rbtree_next_post(const rbnode * restrict n)
  __attribute__((nonnull));

#define rbtree_first(rb, type, member) ({              \
  rbnode * n;                                          \
  n = rbtree_first_post(rb);                           \
  n == &rbleaf ? NULL : containerof(n, type, member);  \
})

#define rbtree_next(item, member) ({                   \
  if(item) {                                           \
    rbnode * n = rbtree_next_post(&(item)->member);        \
    item = (n == &rbleaf) ? NULL : containerof(n, typeof(*item), member);  \
  }                                                     \
  item;                                                 \
})

#define rbtree_foreach_node(rb, rbn)      \
  for(                                    \
    rbn = rbtree_first_post(rb);          \
    rbn != &rbleaf;                       \
    rbn = rbtree_next_post(rbn)      \
  )

#define rbtree_foreach(rb, item, member)                                                \
  for(                                                                                  \
    item = containerof(rbtree_first_post(rb), typeof(*item), member);                   \
    item != containerof(&rbleaf, typeof(*item), member);                                \
    item = containerof(rbtree_next_post(&(item)->member), typeof(*item), member)   \
  )
