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

#include "array.internal.h"

#include "common/grow.h"
#include "common/assure.h"
#include "macros.h"

/*
 * Default capacity, the minimum number of entries which can be stored without reallocating
 */
#define DEFAULT_CAPACITY  10

//
// static
//

static int compare_items(const struct array_t * ar, const void * a, const void * b, int (*cmp_fn)(const void *, size_t, const void *, size_t))
{
  if(!cmp_fn)
    cmp_fn = ar->cmp_fn;
  return cmp_fn(a, ar->esz, b, ar->esz);
}

static void destroy_item(const struct array_t * ar, void * item)
{
  if(ar->destroy_fn)
    ar->destroy_fn(item);
}

static void store_items(const struct array_t * ar, void * storage, void * _items, size_t len)
{
  void ** items = _items;
  size_t x;

  if(ar->init_fn){
    for(x = 0; x < len; x++)
      ar->init_fn(storage + (x * ar->esz));
  }

  // return pointers to items
  for(x = 0; x < len; x++)
  {
    items[x] = storage + (x * ar->esz);
    if(ar->init_fn)
      ar->init_fn(items[x]);
  }
}

static ar_operations ar_ops = {
    compare_items : compare_items
  , destroy_item : destroy_item
  , store_items : store_items
};

static void array_grow(array_t * const restrict ar, size_t len)
{
  grow(&ar->v, ar->esz, len, ar->size, &ar->a);
}

static void array_assure(array_t * const restrict ar, size_t len)
{
  assure(&ar->v, ar->esz, len, &ar->a);
}

/// array_copy
//
// SUMMARY
//  copy elements from one array to another
//
// REMARKS
//  the arrays must be compatible, e.g. have the same element size and value_free operations
//
static void array_copy(array_t * const restrict dst, size_t dst_index, array_t * const restrict src, size_t src_index, size_t len)
{
  size_t elsz = dst->esz;

  memcpy(
      dst->v + (dst_index * elsz)
    , src->v + (src_index * elsz)
    , len * elsz
  );
}

/// array_destroy_range
//
// SUMMARY
//  free a range of elements in a array
//
// PARAMETERS
//  ar    - pointer to array
//  start - index of the first element
//  len   - number of elements
//
void array_destroy_range(array_t * const restrict ar, size_t start, size_t len)
{
  size_t x;

  if(ar->ops->destroy_item)
  {
    for(x = 0; x < len; x++) {
      ar->ops->destroy_item(ar, ar->v + ((start + x) * ar->esz));
    }
  }
}

//
// internal
//

void array_init(
    array_t * const restrict ar
  , size_t esz
  , size_t capacity
  , ar_operations * restrict ops
  , int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz)
)
{
  ar->esz = esz;
  ar->ops = ops;
  ar->cmp_fn = cmp_fn ?: memncmp;

  array_grow(ar, capacity ?: DEFAULT_CAPACITY);
}

void array_xdestroy(array_t * const restrict ar)
{
  array_recycle(&ar->arx);
  wfree(ar->v);
}

//
// api
//

void API array_createx(
    array ** const restrict arp
  , size_t esz
  , size_t capacity
  , int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz)
  , void * init_fn
  , void * destroy_fn
)
{
  array_t * ar = 0;
  xmalloc(&ar, sizeof(*ar));
  array_init(ar, esz, capacity, &ar_ops, cmp_fn);

  ar->init_fn = init_fn;
  ar->destroy_fn = destroy_fn;

  *arp = &ar->arx;
  ar = 0;

  if(ar) {
    array_xfree(&ar->arx);
  }
}

void API array_create(array** const restrict ar, size_t esz)
{
  array_createx(ar, esz, 0, 0, 0, 0);
}

void API array_xfree(array * const restrict arx)
{
  array_t * ar = containerof(arx, array_t, arx);

  if(ar)
    array_xdestroy(ar);

  wfree(ar);
}

void API array_ixfree(array ** const restrict arx)
{
  array_xfree(*arx);
  *arx = 0;
}

void API array_recycle(array * const restrict arx)
{
  array_t * ar = containerof(arx, array_t, arx);

  array_truncate(&ar->arx, ar->size);
  ar->size = 0;
}

void * API array_get(const array * const restrict arx, int x)
{
  array_t * ar = containerof(arx, array_t, arx);
  return ar->v + ((x) * ar->esz);
}

void API array_push(array * const arx, void * const item)
{
  array_insert_range(arx, arx->size, item, 1);
}

void API array_push_range(array * const arx, void * items, size_t len)
{
  array_insert_range(arx, arx->size, items, len);
}

void API array_pop(array * const restrict arx)
{
  array_t * ar = containerof(arx, array_t, arx);

  if(ar->size)
  {
    array_destroy_range(ar, ar->size - 1, 1);
    ar->size--;
  }
}

void API array_unshift(array * const arx, void * item)
{
  array_insert_range(arx, 0, item, 1);
}

void API array_unshift_range(array * const arx, void * items, size_t len)
{
  array_insert_range(arx, 0, items, len);
}

void API array_shift(array * const restrict arx)
{
  array_t * ar = containerof(arx, array_t, arx);

  if(ar->size)
  {
    array_destroy_range(ar, 0, 1);

    memmove(
        ar->v
      , ar->v + ar->esz
      , (ar->size - 1) * ar->esz
    );

    ar->size--;
  }
}

void API array_insert(array * const arx, size_t index, void * item)
{
  array_insert_range(arx, index, item, 1);
}

void API array_insert_range(array * const arx, size_t index, void * items, size_t len)
{
  array_t * ar = containerof(arx, array_t, arx);

  // allocate new space if necessary
  array_assure(ar, ar->size + len);

  // for an insertion, displace existing elements
  memmove(
      ar->v + ((index + len) * ar->esz)
    , ar->v + (index * ar->esz)
    , (ar->size - index) * ar->esz
  );

  ar->ops->store_items(ar, ar->v + (index * ar->esz), items, len);
  ar->size += len;
}

void API array_update(array * const arx, size_t index, void * item)
{
  array_update_range(arx, index, item, 1);
}

void API array_update_range(array * const arx, size_t index, void * items, size_t len)
{
  array_t * ar = containerof(arx, array_t, arx);

  array_destroy_range(ar, index, len);
  ar->ops->store_items(ar, ar->v + (index * ar->esz), items, len);
}

void API array_delete(array * const restrict arx, size_t index)
{
  array_t * ar = containerof(arx, array_t, arx);

  array_destroy_range(ar, index, 1);

  // overwrite
  memmove(
      ar->v + (index * ar->esz)
    , ar->v + ((index + 1) * ar->esz)
    , (ar->size - index - 1) * ar->esz
  );

  ar->size--;
}

void API array_delete_range(array * const restrict arx, size_t index, size_t len)
{
  array_t * ar = containerof(arx, array_t, arx);

  array_destroy_range(ar, index, len);

  // overwrite
  memmove(
      ar->v + (index * ar->esz)
    , ar->v + ((index + len) * ar->esz)
    , (ar->size - index - len) * ar->esz
  );

  ar->size -= len;
}

void API array_subarray(array * const restrict arx, size_t index, size_t len, array ** const restrict rv)
{
  array_t * ar = containerof(arx, array_t, arx);
  array_t * dst;

  if(!*rv)
    array_createx(rv, ar->esz, len, ar->cmp_fn, ar->init_fn, ar->destroy_fn);
  dst = containerof(*rv, array_t, arx);

  array_recycle(&dst->arx);
  array_replicate(&dst->arx, 0, &ar->arx, index, len);
}

void API array_splice(array * const restrict dstx, size_t dst_index, array * const restrict srcx, size_t src_index, size_t len)
{
  array_t * src = containerof(srcx, array_t, arx);
  array_t * dst = containerof(dstx, array_t, arx);

  if(len == -1)
    len = src->size;

  array_replicate(&dst->arx, dst_index, &src->arx, src_index, len);

  // overwrite
  memmove(
      src->v + (src_index * src->esz)
    , src->v + ((src_index + len) * src->esz)
    , (src->size - src_index - len) * src->esz
  );
  src->size -= len;
}

void API array_replicate(array * const restrict dstx, size_t dst_index, array * const restrict srcx, size_t src_index, size_t len)
{
  array_t * src = containerof(srcx, array_t, arx);
  array_t * dst = containerof(dstx, array_t, arx);

  if(len == -1)
    len = src->size;

  // allocate new space if necessary
  array_assure(dst, dst->size + len);

  // displace existing elements
  memmove(
      dst->v + ((dst_index + len) * dst->esz)
    , dst->v + (dst_index * dst->esz)
    , (dst->size - dst_index) * dst->esz
  );

  array_copy(dst, dst_index, src, src_index, len);
  dst->size += len;
}

void API array_truncate(array * const restrict arx, size_t len)
{
  array_t * ar = containerof(arx, array_t, arx);

  array_destroy_range(ar, 0, len);
  ar->size = len;
}

bool API array_equal(array * const restrict Ax, array * const restrict Bx, int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz))
{
  array_t * A = containerof(Ax, array_t, arx);
  array_t * B = containerof(Bx, array_t, arx);

  if(A->size != B->size)
    return false;

  int x;
  for(x = 0; x < A->size; x++)
  {
    void * a = A->v + (x * A->esz);
    void * b = B->v + (x * A->esz);
    if(A->ops->compare_items(A, a, b, cmp_fn))
      return false;
  }

  return true;
}

struct sort_ctx {
  const array_t * ar;
  int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz);
};

static int sort_cmp(const void * A, const void * B, void * arg)
{
  struct sort_ctx * ctx = arg;
  return ctx->ar->ops->compare_items(ctx->ar, A, B, ctx->cmp_fn);
}

void API array_sort(const array * const restrict arx, int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz))
{
  array_t * ar = containerof(arx, array_t, arx);

  struct sort_ctx ctx = { ar : ar, cmp_fn : cmp_fn ?: ar->cmp_fn };
  qsort_r(ar->v, ar->size, ar->esz, sort_cmp, &ctx);
}

struct context {
  const array_t * ar;
  const void * key;
  int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz);
  int lc;
  const void * last_item;
};

static int compare(const void * _ctx, const void * item)
{
  struct context * ctx = (typeof(ctx))_ctx;

  ctx->last_item = item;
  return ctx->lc = ctx->ar->ops->compare_items(ctx->ar, ctx->key, item, ctx->cmp_fn);
};

bool API array_search(
    const array * const restrict arx
  , void * key
  , int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz)
  , void * item
  , size_t * restrict lx
  , int * restrict lc
)
{
  array_t * ar = containerof(arx, array_t, arx);
  return array_search_range(&ar->arx, 0, ar->size, key, cmp_fn, item, lx, lc);
}

bool API array_search_range(
    const array * const restrict arx
  , size_t index
  , size_t len
  , void * key
  , int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz)
  , void * item
  , size_t * restrict lx
  , int * restrict lc
)
{
  array_t * ar = containerof(arx, array_t, arx);

  struct context ctx = { ar : ar, cmp_fn : cmp_fn, key : key };
  void * result = bsearch(&ctx, ar->v + (index * ar->esz), len, ar->esz, compare);

  if(item)
    *(void**)item = result;
  if(lx)
  {
    if(ctx.last_item)
      *lx = ((char*)ctx.last_item - ar->v) / ar->esz;
    else
      *lx = 0;
  }
  if(lc)
    *lc = ctx.lc;

  return !!result;
}
