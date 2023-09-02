/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdlib.h>
#include <string.h>

#include "xlinux/xstdlib.h"

#include "rbtree.h"
#include "llist.h"

#include "macros.h"
#include "zcat.h"

rbnode rbleaf = { color : RB_BLACK, parent : &rbleaf };

//
// static
//

#if CONFIG_DEVEL
static int black_height(rbnode * restrict n)
{
  int left;
  int right;

  if(n == &rbleaf) {
    return 1;
  }

  left = black_height(n->left);
  right = black_height(n->right);
  RUNTIME_ASSERT(left == right);

  if(n->color == RB_BLACK)
    left++;

  return left;
}

static size_t rbnode_say_self(char * buf, size_t sz, rbnode * n, int level)
{
  size_t z = 0;

  if(n == &rbleaf || n == 0)
  {
    return zcatf(buf + z, sz - z, "%*s%-16p [%s] LEAF", level * 2, "", n, "B");
  }

  z += zcatf(buf + z, sz - z, "%*s%-16p [%s]", level * 2, "", n, n->color == RB_RED ? "R" : n->color == RB_BLACK ? "B" : "U");
  z += zcatf(buf + z, sz - z, " parent %-16p", n->parent);
  if(n->left == &rbleaf)
    z += zcatf(buf + z, sz - z, " left %-16s", " --- ");
  else
    z += zcatf(buf + z, sz - z, " left %-16p", n->left);
  if(n->right == &rbleaf)
    z += zcatf(buf + z, sz - z, " right %-16s", " --- ");
  else
    z += zcatf(buf + z, sz - z, " right %-16p", n->right);
  z += zcatf(buf + z, sz - z, " <<-- %s", *(char**)(((char*)n) + 48));

  z += zcatf(buf + z, sz - z, " (%d)", black_height(n));

  return z;
}

static size_t rbnode_say(char * buf, size_t sz, rbnode * n, int level);
static size_t rbnode_say(char * buf, size_t sz, rbnode * n, int level)
{
  size_t z = 0;

  z += rbnode_say_self(buf + z, sz - z, n, level);
  z += zcatf(buf + z, sz - z, "\n");
  if(n == &rbleaf || n == 0)
    return z;

  z += rbnode_say(buf + z, sz - z, n->left, level + 1);
  z += rbnode_say(buf + z, sz - z, n->right, level + 1);

  return z;
}

void rbtree_print(rbtree * rb);
void rbtree_print(rbtree * restrict rb)
{
  char buf[4096 << 1];
  size_t z = 0;

  z += rbnode_say(buf, sizeof(buf), rb->root, 0);
  printf("%.*s\n", (int)z, buf);
}

void rbnode_print(rbnode * n);
void rbnode_print(rbnode * restrict n)
{
  char buf[4096 << 1];
  size_t z = 0;

  z += rbnode_say_self(buf, sizeof(buf), n, 0);
  printf("%.*s\n", (int)z, buf);
}
#endif

static void node_search(
    const rbtree * restrict rb
  , rbnode * restrict base
  , rbtree_search_context * restrict ctx
  , const rbnode * restrict key
  , __attribute__((nonnull)) int (*cmp)(const rbnode * restrict, const rbnode * restrict)
)
{
  while(base != &rbleaf)
  {
    ctx->last = base;
    ctx->lx = cmp(key, ctx->last);

    if(ctx->lx == 0)
    {
      break;
    }
    else if(ctx->lx > 0)
    {
      base = base->right;
    }
    else
    {
      base = base->left;
    }
  }
}

static void key_search(
    const rbtree * restrict rb
  , rbnode * restrict base
  , rbtree_search_context * restrict ctx
  , void * key
  , __attribute__((nonnull)) int (*cmp)(void * restrict key, const rbnode * restrict n)
)
{
  while(base != &rbleaf)
  {
    ctx->last = base;
    ctx->lx = cmp(key, ctx->last);

    if(ctx->lx == 0)
    {
      break;
    }
    else if(ctx->lx > 0)
    {
      base = base->right;
    }
    else
    {
      base = base->left;
    }
  }
}

static rbnode * leftmost(const rbnode * restrict n)
{
  while(1)
  {
    if(n->left != &rbleaf)
      n = n->left;
    else if(n->right != &rbleaf)
      n = n->right;
    else
      return (void*)n;
  }
}

static __attribute__((nonnull)) void destroy(rbtree * restrict tb, rbnode * restrict n, void (*rbn_free)(rbnode * restrict n));
static __attribute__((nonnull)) void destroy(rbtree * restrict rb, rbnode * restrict n, void (*rbn_free)(rbnode * restrict n))
{
  if(n == &rbleaf) {
    return;
  }

  destroy(rb, n->left, rbn_free);
  destroy(rb, n->right, rbn_free);
  rbn_free(n);
}

static void rotate_left(rbtree * restrict rb, rbnode * restrict n)
{
  rbnode *nn, *p;

  nn = n->right;
  p = n->parent;

  n->right = nn->left;
  nn->left = n;
  n->parent = nn;

  if(n->right != &rbleaf) {
    n->right->parent = n;
  }

  if(p != &rbleaf && n == p->left) {
    p->left = nn;
  } else if(p != &rbleaf && n == p->right) {
    p->right = nn;
  }

  if((nn->parent = p) == &rbleaf) {
    rb->root = nn;
  }
}

static void rotate_right(rbtree * restrict rb, rbnode * restrict n)
{
  rbnode *nn, *p;

  /* nn becomes the new parent */
  nn = n->left;
  p = n->parent;

  n->left = nn->right;
  nn->right = n;
  n->parent = nn;

  if(n->left != &rbleaf) {
    n->left->parent = n;
  }

  if(p != &rbleaf && n == p->left) {
    p->left = nn;
  } else if(p != &rbleaf && n == p->right) {
    p->right = nn;
  }

  if((nn->parent = p) == &rbleaf) {
    rb->root = nn;
  }
}

static void put_repair(rbtree * restrict rb, rbnode * restrict n);
static void put_repair(rbtree * restrict rb, rbnode * restrict n)
{
  rbnode *p, *g, *u = 0;

  // case 1
  if(n == rb->root)
  {
    n->color = RB_BLACK;
    return;
  }

  // case 2
  p = n->parent;
  if(p->color == RB_BLACK) {
    return;
  }

  if((g = p->parent))
  {
    if((u = g->left) == p) {
      u = g->right;
    }
  }

  // case 3
  if(u && u->color == RB_RED)
  {
    g->color = RB_RED;
    p->color = RB_BLACK;
    u->color = RB_BLACK;

    return put_repair(rb, g);
  }

  // case 4
  if(n == p->right && p == g->left)
  {
    rotate_left(rb, p);
    n = n->left;
  }
  else if(n == p->left && p == g->right)
  {
    rotate_right(rb, p);
    n = n->right;
  }

  p = n->parent;
  g = p->parent;

  if(n == p->left)
  {
    rotate_right(rb, g);
  }
  else
  {
    rotate_left(rb, g);
  }

  p->color = RB_BLACK;
  g->color = RB_RED;
}

static rbnode * minimum(rbnode * restrict n)
{
  while(n->left != &rbleaf) {
    n = n->left;
  }

  return n;
}

static void transplant(rbtree * restrict rb, rbnode * restrict a, rbnode * restrict b)
{
  if(a->parent == &rbleaf) {
    rb->root = b;
  } else if(a == a->parent->left) {
    a->parent->left = b;
  } else {
    a->parent->right = b;
  }

  b->parent = a->parent;
}

static void delete_repair(rbtree * restrict rb, rbnode * restrict n)
{
  rbnode *m;

  while(n != rb->root && n->color == RB_BLACK)
  {
    if(n == n->parent->left)
    {
      m = n->parent->right;
      if(m->color == RB_RED)
      {
        m->color = RB_BLACK;
        n->parent->color = RB_RED;
        rotate_left(rb, n->parent);
        m = n->parent->right;
      }

      if(m->left->color == RB_BLACK && m->right->color == RB_BLACK)
      {
        m->color = RB_RED;
        n = n->parent;
      }
      else
      {
        if(m->right->color == RB_BLACK)
        {
          m->left->color = RB_BLACK;
          m->color = RB_RED;
          rotate_right(rb, m);
          m = n->parent->right;
        }

        m->color = n->parent->color;
        n->parent->color = RB_BLACK;
        m->right->color = RB_BLACK;
        rotate_left(rb, n->parent);
        n = rb->root;
      }
    }
    else
    {
      m = n->parent->left;
      if(m->color == 1)
      {
        m->color = RB_BLACK;
        n->parent->color = RB_RED;
        rotate_right(rb, n->parent);
        m = n->parent->left;
      }

      if(m->left->color == RB_BLACK && m->right->color == RB_BLACK)
      {
        m->color = RB_RED;
        n = n->parent;
      }
      else
      {
        if(m->left->color == RB_BLACK)
        {
          m->right->color = RB_BLACK;
          m->color = RB_RED;
          rotate_left(rb, m);
          m = n->parent->left;
        }

        m->color = n->parent->color;
        n->parent->color = RB_BLACK;
        m->left->color = RB_BLACK;
        rotate_right(rb, n->parent);
        n = rb->root;
      }
    }
  }

  n->color = RB_BLACK;
}

//
// api
//

void rbtree_init(rbtree * restrict rb)
{
  memset(rb, 0, sizeof(*rb));
  rb->root = &rbleaf;
}

size_t rbtree_count(const rbtree * restrict rb)
{
  size_t c = 0;
  rbnode *rbn;

  rbtree_foreach_node((rbtree*)rb, rbn) {
    c++;
  }

  return c;
}

void rbtree_destroy(rbtree * restrict rb, void (*rbn_free)(rbnode * restrict n))
{
  if(rbn_free) {
    destroy(rb, rb->root, rbn_free);
  }
}

void rbnode_init(rbnode * restrict rbn)
{
  memset(rbn, 0, sizeof(*rbn));
}

void rbtree_insert(rbtree * restrict rb, rbtree_search_context * restrict ctx, rbnode * restrict n)
{
  if(rb->root == &rbleaf)
  {
    n->color = RB_BLACK;
    n->left = &rbleaf;
    n->right = &rbleaf;
    n->parent = &rbleaf;
    rb->root = n;
#if RBTREE_HASH
    n->rbt = rb;
    rb->hash += (uint32_t)(uintptr_t)n;
#endif
#if RBTREE_SIZE
    rb->size++;
#endif
    return;
  }

  n->color = RB_RED;
  n->left = &rbleaf;
  n->right = &rbleaf;

  if(ctx->lx < 0)
  {
    ctx->last->left = n;
    n->parent = ctx->last;
  }
  else
  {
    ctx->last->right = n;
    n->parent = ctx->last;
  }

  put_repair(rb, n);

#if RBTREE_HASH
  n->rbt = rb;
  rb->hash += (uint32_t)(uintptr_t)n;
#endif
#if RBTREE_SIZE
  rb->size++;
#endif
#if RBTREE_TRACK
  n->deleted = 0;
#endif
}

rbnode * rbtree_put(
    rbtree * restrict rb
  , rbnode * restrict n
  , __attribute__((nonnull)) int (*cmp)(const rbnode * restrict, const rbnode * restrict)
)
{
  rbtree_search_context ctx;

  if(rb->root == &rbleaf) {
    goto insert;
  }

  memset(&ctx, 0, sizeof(ctx));
  node_search(rb, rb->root, &ctx, n, cmp);

  if(ctx.lx == 0) {
    return ctx.last;
  }

insert:
  rbtree_insert(rb, &ctx, n);
  return 0;
}

rbnode * rbtree_search(
    const rbtree * restrict rb
  , rbtree_search_context *ctx
  , void *key
  , __attribute__((nonnull)) int (*cmp)(void * restrict key, const rbnode * restrict n)
)
{
  if(rb->root == &rbleaf) {
    return 0;
  }

  memset(ctx, 0, sizeof(*ctx));
  key_search(rb, rb->root, ctx, key, cmp);

  if(ctx->lx == 0)
    return ctx->last;

  return 0;
}

rbnode * rbtree_lookup(
    const rbtree * restrict rb
  , void * key
  , __attribute__((nonnull)) int (*cmp)(void * restrict key, const rbnode * restrict n)
)
{
  rbtree_search_context ctx;
  return rbtree_search(rb, &ctx, key, cmp);
}

void rbtree_replace(rbtree * restrict rb, rbnode * restrict old, rbnode * restrict n)
{
  rbnode *p;

  if(old == rb->root) {
    rb->root = n;
  }

  p = old->parent;
  if(p == &rbleaf) {
  } else if(old == p->left) {
    p->left = n;
  } else {
    p->right = n;
  }
  if(old->left != &rbleaf) {
    old->left->parent = n;
  }
  if(old->right != &rbleaf) {
    old->right->parent = n;
  }

  n->color = old->color;
  n->parent = old->parent;
  n->left = old->left;
  n->right = old->right;

#if RBTREE_HASH
old->rbt->hash -= (uint32_t)(uintptr_t)old;
old->rbt->hash += (uint32_t)(uintptr_t)n;
#endif
}

void rbtree_delete(rbtree * restrict rb, rbnode * restrict n)
{
  rbnode *m;
  rbnode *o;
  enum rbcolor color;

#if RBTREE_TRACK
  RUNTIME_ASSERT(!n->deleted);
#endif

  if(n->left == &rbleaf)
  {
    color = n->color;
    o = n->right;
    transplant(rb, n, n->right);
  }
  else if(n->right == &rbleaf)
  {
    color = n->color;
    o = n->left;
    transplant(rb, n, n->left);
  }
  else
  {
    m = minimum(n->right);
    color = m->color;
    o = m->right;
    if(m->parent == n)
    {
      o->parent = m;
    }
    else
    {
      transplant(rb, m, m->right);
      m->right = n->right;
      m->right->parent = m;
    }

    transplant(rb, n, m);
    m->left = n->left;
    m->left->parent = m;
    m->color = n->color;
  }

  if(color == RB_BLACK)
  {
    delete_repair(rb, o);
  }

#if CONFIG_DEBUG || CONFIG_DEVEL
  n->color = 0;
  n->left = 0;
  n->right = 0;
#endif
  n->parent = 0;

#if RBTREE_HASH
  rb->hash -= (uint32_t)(uintptr_t)n;
#endif
#if RBTREE_SIZE
  rb->size--;
#endif
#if RBTREE_TRACK
  n->deleted = 1;
#endif
}

rbnode * rbtree_first_post(const rbtree * restrict rb)
{
  if(rb->root == &rbleaf) {
    return &rbleaf;
  }

  return leftmost(rb->root);
}

rbnode * rbtree_next_post(const rbnode * restrict n)
{
  if(n->parent == &rbleaf) {
    return &rbleaf;
  }

  if(n == n->parent->left && n->parent->right != &rbleaf) {
    return leftmost(n->parent->right);
  }

  return n->parent;
}
