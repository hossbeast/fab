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

#include "list.internal.h"

#include "macros.h"

//
// static
//

typedef struct container {
  void ** items;
  size_t * lens;
} container;

static int compare_items(const struct array_t * ar, const void * _a, const void * _b, int (*cmp_fn)(const void *, size_t, const void *, size_t))
{
  if(!cmp_fn)
    cmp_fn = ar->cmp_fn;

  const value * a = _a;
  const value * b = _b;

  return cmp_fn(a->p, a->l, b->p, b->l);
}

static xapi destroy_item(const struct array_t * ar, void * item)
{
  enter;

  list_t * li = containerof(ar, list_t, ar);
  value * val = item;

  if(li->free_fn)
    li->free_fn(val->p);
  else if(li->xfree_fn)
    fatal(li->xfree_fn, val->p);

  finally : coda;
}

static xapi store_items(const struct array_t * ar, void * _storage, void * _container, size_t len)
{
  enter;

  value * storage = _storage;
  container * c = _container;

  size_t x;
  for(x = 0; x < len; x++)
  {
    storage[x].p = c->items[x];
    if(c->lens)
      storage[x].l = c->lens[x];
  }

  finally : coda;
}

static ar_operations ar_ops = {
    compare_items : compare_items
  , destroy_item : destroy_item
  , store_items : store_items
};

//
// api
//

xapi API list_createx(
    list** const restrict rv
  , size_t capacity
  , int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz)
  , void * free_fn
  , void * xfree_fn
)
{
  enter;

  list_t * li = 0;

  fatal(xmalloc, &li, sizeof(*li));
  fatal(array_init
    , &li->ar
    , sizeof(value)
    , capacity
    , &ar_ops
    , cmp_fn
  );

  li->free_fn = free_fn;
  li->xfree_fn = xfree_fn;

  *rv = &li->lix;
  li = 0;

finally:
  if(li)
    fatal(list_xfree, &li->lix);
coda;
}

xapi API list_create(list** const restrict li)
{
  xproxy(list_createx, li, 0, 0, 0, 0);
}

xapi API list_xfree(list * const restrict lix)
{
  enter;

  list_t * li = containerof(lix, list_t, lix);

  if(li)
    fatal(array_xdestroy, &li->ar);

  wfree(li);

  finally : coda;
}

xapi API list_ixfree(list ** const restrict li)
{
  enter;

  fatal(list_xfree, *li);
  *li = 0;

  finally : coda;
}

xapi API list_recycle(list * const restrict lix)
{
  enter;

  list_t * li = containerof(lix, list_t, lix);
  fatal(array_recycle, &li->arx);

  finally : coda;
}

void * API list_get(const list * const restrict lix, int x)
{
  list_t * li = containerof(lix, list_t, lix);
  value * val = array_get(&li->arx, x);
  if(val)
    return val->p;

  return 0;
}

bool list_get_item(const list * const restrict lix, int x, void * restrict item, size_t * restrict item_len)
{
  list_t * li = containerof(lix, list_t, lix);
  value * val = array_get(&li->arx, x);
  if(val)
  {
    if(item)
      *(void**)item = val->p;
    if(item_len)
      *item_len = val->l;
    return true;
  }

  return false;
}

xapi API list_push(list * const lix, void * item, size_t item_len)
{
  xproxy(list_push_range, lix, &item, &item_len, 1);
}

xapi API list_push_range(list * const lix, void * items, size_t * restrict items_lens, size_t len)
{
  enter;

  list_t * li = containerof(lix, list_t, lix);

  container c = { items : items, lens : items_lens };
  fatal(array_push_range, &li->arx, &c, len);

  finally : coda;
}

xapi API list_pop(list * const restrict lix)
{
  enter;

  list_t * li = containerof(lix, list_t, lix);
  fatal(array_pop, &li->arx);

  finally : coda;
}

xapi API list_unshift(list * const lix, void * el, size_t item_len)
{
  xproxy(list_unshift_range, lix, &el, &item_len, 1);
}

xapi API list_unshift_range(list * const lix, void * items, size_t * restrict items_lens, size_t len)
{
  enter;

  list_t * li = containerof(lix, list_t, lix);

  container c = { items : items, lens : items_lens };
  fatal(array_unshift_range, &li->arx, &c, len);

  finally : coda;
}

xapi API list_shift(list * const restrict lix)
{
  enter;

  list_t * li = containerof(lix, list_t, lix);
  fatal(array_shift, &li->arx);

  finally : coda;
}

xapi API list_insert(list * const lix, size_t index, void * item, size_t item_len)
{
  xproxy(list_insert_range, lix, index, &item, &item_len, 1);
}

xapi API list_insert_range(list * const lix, size_t index, void * items, size_t * restrict items_lens, size_t len)
{
  enter;

  list_t * li = containerof(lix, list_t, lix);

  container c = { items : items, lens : items_lens };
  fatal(array_insert_range, &li->arx, index, &c, len);

  finally : coda;
}

xapi API list_update(list * const lix, size_t index, void * item, size_t item_len)
{
  xproxy(list_update_range, lix, index, &item, &item_len, 1);
}

xapi API list_update_range(list * const lix, size_t index, void * items, size_t * restrict items_lens, size_t len)
{
  enter;

  list_t * li = containerof(lix, list_t, lix);

  container c = { items : items, lens : items_lens };
  fatal(array_update_range, &li->arx, index, &c, len);

  finally : coda;
}

xapi API list_delete(list * const restrict lix, size_t index)
{
  xproxy(list_delete_range, lix, index, 1);
}

xapi API list_delete_range(list * const restrict lix, size_t index, size_t len)
{
  enter;

  list_t * li = containerof(lix, list_t, lix);
  fatal(array_delete_range, &li->arx, index, len);

  finally : coda;
}

xapi API list_sublist(list * const restrict lix, size_t index, size_t len, list ** const restrict rv)
{
  enter;

  list_t * li = containerof(lix, list_t, lix);
  list_t * dst = 0;

  if(!*rv)
    fatal(list_createx, rv, li->size, li->cmp_fn, li->free_fn, li->xfree_fn);
  dst = containerof(*rv, list_t, lix);

  fatal(list_recycle, &dst->lix);
  fatal(list_replicate, &dst->lix, 0, &li->lix, index, len);

  finally : coda;
}

xapi API list_splice(list * const restrict dstx, size_t dst_index, list * const restrict srcx, size_t src_index, int len)
{
  enter;

  list_t * dst = containerof(dstx, list_t, lix);
  list_t * src = containerof(srcx, list_t, lix);

  fatal(array_splice, &dst->arx, dst_index, &src->arx, src_index, len);

  finally : coda;
}

xapi API list_replicate(list * const restrict dstx, size_t dst_index, list * const restrict srcx, size_t src_index, int len)
{
  enter;

  list_t * dst = containerof(dstx, list_t, lix);
  list_t * src = containerof(srcx, list_t, lix);

  fatal(array_replicate, &dst->arx, dst_index, &src->arx, src_index, len);

  finally : coda;
}

xapi API list_truncate(list * const restrict lix, size_t len)
{
  enter;

  list_t * li = containerof(lix, list_t, lix);
  fatal(array_truncate, &li->arx, len);

  finally : coda;
}

bool API list_equal(list * const restrict Ax, list * const restrict Bx, int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz))
{
  list_t * A = containerof(Ax, list_t, lix);
  list_t * B = containerof(Bx, list_t, lix);
  return array_equal(&A->arx, &B->arx, cmp_fn);
}

void API list_sort(list * const restrict lix, int (*cmp_fn)(const void *A, size_t Asz, const void *B, size_t Bsz))
{
  list_t * li = containerof(lix, list_t, lix);
  array_sort(&li->arx, cmp_fn);
}

bool API list_search(
    const list * const restrict lix
  , void * key_data
  , size_t key_len
  , int (*cmp_fn)(const void *A, size_t Asz, const void *B, size_t Bsz)
  , void * item
  , size_t * restrict item_len
  , size_t * restrict lx
  , int * restrict lc
)
{
  return list_search_range(lix, 0, lix->size, key_data, key_len, cmp_fn, item, item_len, lx, lc);
}

bool API list_search_range(
    const list * const restrict lix
  , size_t index
  , size_t len
  , void * key_data
  , size_t key_len
  , int (*cmp_fn)(const void *A, size_t Asz, const void *B, size_t Bsz)
  , void * item
  , size_t * restrict item_len
  , size_t * restrict lx
  , int * restrict lc
)
{
  list_t * li = containerof(lix, list_t, lix);

  value key = { p : key_data, l : key_len };

  value * result;
  array_search_range(&li->arx, index, len, &key, cmp_fn, &result, lx, lc);

  if(item)
    *(void**)item = result ? result->p : 0;
  if(item_len)
    *item_len = result ? result->l : 0;

  return !!result;
}
