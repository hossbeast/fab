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
#include <string.h>

#include "xlinux/xstdlib.h"

#include "internal.h"
#include "rbtree.internal.h"
#include "llist.h"

#include "macros.h"
#include "zbuffer.h"

APIDATA rbnode rbleaf = { color : RB_BLACK, parent : &rbleaf };

//
// static
//

static int black_height(rbnode * restrict n)
{
  int left;
#if DEBUG || DEVEL
  int right;
#endif

  if(n == &rbleaf)
    return 1;

  left = black_height(n->left);
#if DEBUG || DEVEL
  right = black_height(n->right);
  RUNTIME_ASSERT(left == right);
#endif

  if(n->color == RB_BLACK)
    left++;

  return left;
}

static size_t rbnode_say_self(char * buf, size_t sz, rbnode * n, int level)
{
  size_t z = 0;

  if(n == &rbleaf || n == 0)
  {
    return znloadf(buf + z, sz - z, "%*s%-16p [%s] LEAF", level * 2, "", n, "B");
  }

  z += znloadf(buf + z, sz - z, "%*s%-16p [%s]", level * 2, "", n, n->color == RB_RED ? "R" : n->color == RB_BLACK ? "B" : "U");
  z += znloadf(buf + z, sz - z, " parent %-16p", n->parent);
  if(n->left == &rbleaf)
    z += znloadf(buf + z, sz - z, " left %-16s", " --- ");
  else
    z += znloadf(buf + z, sz - z, " left %-16p", n->left);
  if(n->right == &rbleaf)
    z += znloadf(buf + z, sz - z, " right %-16s", " --- ");
  else
    z += znloadf(buf + z, sz - z, " right %-16p", n->right);
  z += znloadf(buf + z, sz - z, " <<-- %s", *(char**)(((char*)n) + 48));

  z += znloadf(buf + z, sz - z, " (%d)", black_height(n));

  return z;
}

static size_t rbnode_say(char * buf, size_t sz, rbnode * n, int level);
static size_t rbnode_say(char * buf, size_t sz, rbnode * n, int level)
{
  size_t z = 0;

  z += rbnode_say_self(buf + z, sz - z, n, level);
  z += znloadf(buf + z, sz - z, "\n");
  if(n == &rbleaf || n == 0)
    return z;

  z += rbnode_say(buf + z, sz - z, n->left, level + 1);
  z += rbnode_say(buf + z, sz - z, n->right, level + 1);

  return z;
}

static void node_search(const rbtree * restrict rb, rbnode * restrict base, rbtree_search_context * restrict ctx, const rbnode * restrict key, rbtree_node_cmp cmp)
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

static void key_search(const rbtree * restrict rb, rbnode * restrict base, rbtree_search_context * restrict ctx, void * key, rbtree_key_cmp cmp)
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

static rbnode * leftmost(rbnode * restrict n)
{
  while(1)
  {
    if(n->left != &rbleaf)
      n = n->left;
    else if(n->right != &rbleaf)
      n = n->right;
    else
      return n;
  }
}

static void destroy(rbtree * restrict tb, rbnode * restrict n, void (*rbn_free)(rbnode * restrict n));
static void destroy(rbtree * restrict rb, rbnode * restrict n, void (*rbn_free)(rbnode * restrict n))
{
  if(n == &rbleaf)
    return;

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

  if(n->right != &rbleaf)
    n->right->parent = n;

  if(p != &rbleaf && n == p->left)
    p->left = nn;
  else if(p != &rbleaf && n == p->right)
    p->right = nn;

  if((nn->parent = p) == &rbleaf)
    rb->root = nn;
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

  if(n->left != &rbleaf)
    n->left->parent = n;

  if(p != &rbleaf && n == p->left)
    p->left = nn;
  else if(p != &rbleaf && n == p->right)
    p->right = nn;

  if((nn->parent = p) == &rbleaf)
    rb->root = nn;
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
  if(p->color == RB_BLACK)
  {
    return;
  }

  if((g = p->parent))
  {
    if((u = g->left) == p)
      u = g->right;
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
  while(n->left != &rbleaf)
    n = n->left;

  return n;
}

static void transplant(rbtree * restrict rb, rbnode * restrict a, rbnode * restrict b)
{
  if(a->parent == &rbleaf)
    rb->root = b;
  else if(a == a->parent->left)
    a->parent->left = b;
  else
    a->parent->right = b;

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
// internal
//

void rbtree_print(rbtree * restrict rb)
{
  char buf[4096 << 1];
  size_t z = 0;

  z += rbnode_say(buf, sizeof(buf), rb->root, 0);
  printf("%.*s\n", (int)z, buf);
}

void rbnode_print(rbnode * restrict n)
{
  char buf[4096 << 1];
  size_t z = 0;

  z += rbnode_say_self(buf, sizeof(buf), n, 0);
  printf("%.*s\n", (int)z, buf);
}

//
// api
//

API void rbtree_init(rbtree * restrict rb)
{
  memset(rb, 0, sizeof(*rb));
  rb->root = &rbleaf;
}

API bool rbtree_empty(rbtree * restrict rb)
{
  return rb->root == &rbleaf;
}

API size_t rbtree_count(rbtree * restrict rb)
{
  size_t c = 0;
  rbnode *rbn;

  rbtree_foreach_node(rb, rbn) {
    c++;
  }

  return c;
}

API void rbtree_destroy(rbtree * restrict rb, void (*rbn_free)(rbnode * restrict n))
{
  destroy(rb, rb->root, rbn_free);
}

API void rbnode_init(rbnode * restrict rbn)
{
  memset(rbn, 0, sizeof(*rbn));
}

API bool rbnode_attached(rbnode * restrict rbn)
{
  return rbn->parent != NULL;
}

API void rbtree_insert_node(rbtree * restrict rb, rbtree_search_context * restrict ctx, rbnode * restrict n)
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
}

API rbnode * rbtree_put_node(rbtree * restrict rb, rbnode * restrict n, rbtree_node_cmp cmp)
{
  rbtree_search_context ctx;

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
    return 0;
  }

  memset(&ctx, 0, sizeof(ctx));
  node_search(rb, rb->root, &ctx, n, cmp);

  if(ctx.lx == 0)
  {
    return ctx.last;
  }

  rbtree_insert_node(rb, &ctx, n);
  return 0;
}

API rbnode * rbtree_search(const rbtree * restrict rb, rbtree_search_context *ctx, void *key, rbtree_key_cmp cmp)
{
  if(rb->root == &rbleaf)
    return 0;

  memset(ctx, 0, sizeof(*ctx));
  key_search(rb, rb->root, ctx, key, cmp);

  if(ctx->lx == 0)
    return ctx->last;

  return 0;
}


API rbnode * rbtree_lookup_node(const rbtree * restrict rb, void * key, rbtree_key_cmp cmp)
{
  rbtree_search_context ctx;
  return rbtree_search(rb, &ctx, key, cmp);
}

API void rbtree_replace_node(rbnode * restrict n, rbnode * restrict new)
{
  rbnode *p;

  p = n->parent;

  *new = *n;

  if(n == p->left)
    p->left = new;
  else
    p->right = new;

#if RBTREE_HASH
n->rbt->hash -= (uint32_t)(uintptr_t)n;
n->rbt->hash += (uint32_t)(uintptr_t)new;
#endif
}

API void rbtree_delete_node(rbtree * restrict rb, rbnode * restrict n)
{
  rbnode *m;
  rbnode *o;
  enum rbcolor color;

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

#if RBTREE_HASH
rb->hash -= (uint32_t)(uintptr_t)n;
#endif
#if RBTREE_SIZE
  rb->size--;
#endif
}

API rbnode * rbtree_first_post_node(rbtree * restrict rb)
{
  if(rb->root == &rbleaf)
    return &rbleaf;

  return leftmost(rb->root);
}

API rbnode * rbtree_last_post_node(rbtree * restrict rb)
{
  return rb->root;
}

API rbnode * rbtree_next_post_node(rbnode * restrict n)
{
  if(n->parent == &rbleaf)
    return &rbleaf;

  if(n == n->parent->left && n->parent->right != &rbleaf)
    return leftmost(n->parent->right);

  return n->parent;
}
