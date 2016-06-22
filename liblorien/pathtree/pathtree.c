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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xapi.h"
#include "xlinux/xstdlib.h"
#include "xlinux/xstring.h"
#include "narrator.h"
#include "narrator/file.h"

#include "internal.h"
#include "pathtree.internal.h"

struct node;
#define LIST_ELEMENT_TYPE struct node
#include "valyria/list.h"

#include "grow.h"
#include "assure.h"
#include "strutil.h"
#include "macros.h"

#define restrict __restrict

//
// static
//

static xapi node_create(node ** const restrict n)
{
  enter;

  fatal(xmalloc, n, sizeof(**n));
  fatal(list_create, &(*n)->children, 0);

  finally : coda;
}

static void node_free(node * const restrict n)
{
  if(n)
  {
    free(n->pfx);
    list_free(n->children);
  }

  free(n);
}

static xapi node_say(node * n, narrator * const restrict N)
{
  enter;

  sayf("%s { keyl %zd, v ", n->pfx, n->keyl);
  if(n->val)
    sayf("%d", *(int*)n->val);
  else
    sayf("(none)");
  sayf(" }");

  finally : coda;
}

static xapi pathtree_say_level(node * n, int level, narrator * const restrict N)
{
  enter;

  sayf("%*s", level * 3, "-");
  fatal(node_say, n, N);
  sayf("\n");

  int x;
  for(x = 0; x < n->children->l; x++)
    fatal(pathtree_say_level, list_get(n->children, x), level + 1, N);

  finally : coda;
}

struct insert_context
{
  char *  pfx;    // current prefix being matched
  size_t  pfxl;   // these two fields are written to during traversal

  node *  parent; // (returns) parent of the matched node
  size_t  lx;     // (returns) index in the parent collection of the last node visited
  int     lc;     // (returns) result of comparison on the last visit
  size_t  len;    // (returns) characters matched on last visit
  size_t  matchl; // (returns) sum[len] for all nodes from the root to the match
};

/// compare_stem_single
//
// SUMMARY
//  compare the key to a node prefix, by finding the matching stem. they are considered a match if the
//  stem includes at least one slash delimited section.
//
static int compare_stem_single(void * ud, const node * n, size_t idx)
{
  struct insert_context * ctx = ud;

  int x;
  for(x = 1; x < ctx->pfxl && x < n->pfxl; x++)
  {
    if((ctx->lc = (ctx->pfx[x] - n->pfx[x])))
      break;

    if(ctx->pfx[x] == '/' && n->pfx[x] == '/')
      ctx->len = x;
  }

  if(ctx->lc == 0 && ctx->pfxl != n->pfxl)
  {
    if(ctx->pfxl > n->pfxl)
      ctx->lc = 1;
    else
      ctx->lc = -1;
  }

  if((x == ctx->pfxl || ctx->pfx[x] == '/') && (x == n->pfxl || n->pfx[x] == '/'))
    ctx->len = x;

  if(ctx->len)
    ctx->lc = 0;
  else if(ctx->lc == 0)
    ctx->lc = -1;

  ctx->lx = idx;
  return ctx->lc;
};

/// traverse_insert
//
// SUMMARY
//  traverse the tree to locate the node that matches the most segments of the key, and return
//  information about the partial key match among its children
//
// PARAMETERS
//  n   - node
//  ctx - see insert_context
//
// RETURNS
//  matching node, if any
//
static node __attribute__((nonnull)) * traverse_insert(node * restrict n, struct insert_context * const restrict ctx)
{
  ctx->len = 0;
  ctx->lc = 0;

  if(ctx->pfxl)
  {
    node * child;
    if((child = list_search(n->children, ctx, compare_stem_single)))
    {
      ctx->pfx += child->pfxl;
      ctx->pfxl -= child->pfxl;
      ctx->parent = n;
      ctx->matchl += ctx->len;
      if(ctx->len == child->pfxl)
        return traverse_insert(child, ctx);

      n = child;
    }
  }

  return n;
}

struct search_context
{
  char *  pfx;    // current prefix being matched
  size_t  pfxl;   // these two fields are written to during traversal

  node *  parent; // (returns) parent of the matched node
};

/// compare_stem_full
//
// SUMMARY
//  compare the key to a node prefix, by finding the matching stem. they are considered a match if the stem
//  includes the entire node prefix, and ends either on a slash boundary or the end of the key.
//
static int compare_stem_full(void * ud, const node * n, size_t idx)
{
  struct search_context * ctx = ud;

  int r = 0;
  int x;
  for(x = 1; x <= ctx->pfxl && x <= n->pfxl; x++)
  {
    // stem is the entire node prefix
    if(x == n->pfxl && x < ctx->pfxl && ctx->pfx[x] == '/')
      break;

    // end of either the node prefix or the key
    if(x == ctx->pfxl || x == n->pfxl)
    {
      if((x == ctx->pfxl) ^ (x == n->pfxl))
      {
        if(ctx->pfxl > n->pfxl)
          r = 1;
        else if(ctx->pfxl < n->pfxl)
          r = -1;
      }

      break;
    }

    if((r = (ctx->pfx[x] - n->pfx[x])))
      break;
  }

  return r;
}

/// traverse_search
//
// SUMMARY
//  traverse the tree to locate the node that matches the most segments of the key
//
// PARAMETERS
//  n   - node
//  ctx - contains the key, returns the parent of the matching node, if any
//
// RETURNS
//  matching node, if any
//
static node __attribute__((nonnull)) * traverse_search(node * restrict n, struct search_context * const restrict ctx)
{
  if(ctx->pfxl)
  {
    node * child;
    if((child = list_search(n->children, ctx, compare_stem_full)))
    {
      ctx->pfx += child->pfxl;
      ctx->pfxl -= child->pfxl;
      ctx->parent = n;
      return traverse_search(child, ctx);
    }
  }

  return n;
}

//
// public
//

xapi pathtree_say(pathtree * pt, narrator * const restrict N)
{
  xproxy(pathtree_say_level, pt->root, 0, N);
}

node * pathtree_search_node(pathtree * const restrict pt, char * const restrict key)
{
  size_t keyl = strlen(key);
  struct search_context ctx = {
      pfx : key
    , pfxl : keyl
  };

  node * n = traverse_search(pt->root, &ctx);

  // partial match, not on a file boundary, fallback to the parent
  if(keyl > n->keyl && key[n->keyl] != '/')
    n = ctx.parent;

  return n;
}

//
// api
//

xapi pathtree_create(pathtree ** restrict pt)
{
  enter;

  node * root = 0;

  fatal(xmalloc, pt, sizeof(**pt));
  fatal(list_create, &(*pt)->nodes, node_free);
  fatal(node_create, &root);
  fatal(list_push, (*pt)->nodes, root);

  (*pt)->root = root;
  root = 0;

finally:
  node_free(root);
coda;
}

void pathtree_free(pathtree * const restrict pt)
{
  if(pt)
  {
    list_free(pt->nodes);
  }

  free(pt);
}

void pathtree_ifree(pathtree ** const restrict pt)
{
  pathtree_free(*pt);
  *pt = 0;
}

xapi pathtree_insert(pathtree * const restrict pt, char * const restrict key, PATHTREE_VALUE_TYPE * val)
{
  enter;

  node * parent = 0;
  node * child = 0;

  size_t keyl = strlen(key);

  struct insert_context ctx = {
      pfx : key
    , pfxl : keyl
  };
  node * match = traverse_insert(pt->root, &ctx);

  // exact match
  if(keyl == match->keyl)
  {
    match->val = val;
  }
  else
  {
    // matched entire node, append child
    if(ctx.matchl == match->keyl)
    {
      fatal(node_create, &child);
      child->val = val;
      fatal(ixstrndup, &child->pfx, key + ctx.matchl, keyl - ctx.matchl);
      child->pfxl = keyl - ctx.matchl;
      child->keyl = keyl;

      if(match->children->l == 0)
        ctx.lx = 0;
      else if(ctx.lc > 0)
        ctx.lx++;

      fatal(list_insert, match->children, ctx.lx, child);
    }

    // node is longer, displace node
    else
    {
      // install new parent
      fatal(node_create, &parent);
      parent->pfxl = ctx.matchl - ctx.parent->keyl;
      fatal(ixstrndup, &parent->pfx, key + ctx.parent->keyl, parent->pfxl);
      parent->keyl = ctx.parent->keyl + parent->pfxl;
      list_set(ctx.parent->children, ctx.lx, parent);

      // updated match node
      char * old = match->pfx;
      size_t oldl = match->pfxl;

      match->pfxl = oldl - parent->pfxl;
      fatal(xmalloc, &match->pfx, match->pfxl + 1);   // <--
      memcpy(match->pfx, old + (oldl - match->pfxl), match->pfxl);
      free(old);

      // new child
      if(keyl > ctx.matchl)
      {
        fatal(node_create, &child);
        child->val = val;
        child->pfxl = keyl - parent->keyl;
        fatal(ixstrndup, &child->pfx, key + parent->keyl, child->pfxl);
        child->keyl = parent->keyl + child->pfxl;
      }
      else
      {
        parent->val = val;
      }

      int r = 0;
      if(child)
        r = estrcmp(child->pfx, child->pfxl, match->pfx, match->pfxl, 0);

      if(r < 0)
        fatal(list_push, parent->children, child);

      fatal(list_push, parent->children, match);

      if(r > 0)
        fatal(list_push, parent->children, child);
    }
  }

  if(child)
    fatal(list_push, pt->nodes, child);
  child = 0;

  if(parent)
    fatal(list_push, pt->nodes, parent);
  parent = 0;

finally:
  node_free(child);
  node_free(parent);
coda;
}

PATHTREE_VALUE_TYPE * pathtree_search(pathtree * const restrict pt, char * const restrict key)
{
  node * n = pathtree_search_node(pt, key);

  if(n)
    return n->val;

  return 0;
}
