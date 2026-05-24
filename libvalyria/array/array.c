/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <string.h>

#include "xlinux/xstdlib.h"

#include "array.h"

#include "common/grow.h"
#include "common/assure.h"

/*
 * Default capacity, the minimum number of entries which can be stored without reallocating
 */
#define DEFAULT_CAPACITY  10

//
// static
//

static int compare_items(const struct array * ar, const void * a, const void * b, int (*cmp)(const void *, size_t, const void *, size_t))
{
  return cmp(a, ar->esz, b, ar->esz);
}

static void store_items(const struct array * ar, void * storage, void * _items, size_t len)
{
  void ** items = _items;
  size_t x;

  // return pointers to items
  for(x = 0; x < len; x++) {
    items[x] = storage + (x * ar->esz);
  }
}

static void array_grow(array * const restrict ar, size_t len)
{
  grow(&ar->v, ar->esz, len, ar->size, &ar->a);
}

static void array_assure(array * const restrict ar, size_t len)
{
  assure(&ar->v, ar->esz, len, &ar->a);
}

/*
 * copy elements from one array to another
 *
 * the arrays must be compatible, e.g. have the same element size and destroy_fn operations
 */
static void array_copy(array * const restrict dst, size_t dst_index, array * const restrict src, size_t src_index, size_t len)
{
  size_t elsz = dst->esz;

  memcpy(
      dst->v + (dst_index * elsz)
    , src->v + (src_index * elsz)
    , len * elsz
  );
}

/*
 * free a range of elements in a array
 *
 * ar    - pointer to array
 * start - index of the first element
 * len   - number of elements
 */
static void array_destroy_range(array * const restrict ar, size_t start, size_t len)
{
  size_t x;

  if(ar->destroy_fn)
  {
    for(x = 0; x < len; x++) {
      ar->destroy_fn(ar->v + ((start + x) * ar->esz));
    }
  }
}

//
// internal
//

void array_init(
    array * const restrict ar
  , size_t esz
  , size_t capacity
  , void (*destroy_fn)(void *item)
)
{
  *ar = (typeof(*ar)) {
    esz : esz,
    destroy_fn : destroy_fn
  };

  array_grow(ar, capacity ?: DEFAULT_CAPACITY);
}

void array_destroy(array * const restrict ar)
{
  array_recycle(ar);
  free(ar->v);
}

//
// api
//

void array_createx(
    array ** const restrict arp
  , size_t esz
  , size_t capacity
  , void (*destroy_fn)(void *)
)
{
  array * ar = 0;

  xmalloc(&ar, sizeof(*ar));
  array_init(ar, esz, capacity, destroy_fn);

  *arp = ar;
}

void array_create(array** const restrict ar, size_t esz)
{
  array_createx(ar, esz, 0, 0);
}

void array_xfree(array * const restrict ar)
{
  if(ar) {
    array_destroy(ar);
  }

  free(ar);
}

void array_ixfree(array ** const restrict ar)
{
  array_xfree(*ar);
  *ar = 0;
}

void array_recycle(array * const restrict ar)
{
  array_truncate(ar, 0);
}

void * array_get(const array * const restrict ar, int x)
{
  return ar->v + ((x) * ar->esz);
}

void array_push(array * const ar, void * const item)
{
  array_insert_range(ar, ar->size, item, 1);
}

void array_push_range(array * const ar, void * items, size_t len)
{
  array_insert_range(ar, ar->size, items, len);
}

void array_pop(array * const restrict ar)
{
  if(ar->size)
  {
    array_destroy_range(ar, ar->size - 1, 1);
    ar->size--;
  }
}

void array_unshift(array * const ar, void * item)
{
  array_insert_range(ar, 0, item, 1);
}

void array_unshift_range(array * const ar, void * items, size_t len)
{
  array_insert_range(ar, 0, items, len);
}

void array_shift(array * const restrict ar)
{
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

void array_insert(array * const ar, size_t index, void * item)
{
  array_insert_range(ar, index, item, 1);
}

void array_insert_range(array * const ar, size_t index, void * items, size_t len)
{
  // allocate new space if necessary
  array_assure(ar, ar->size + len);

  // for an insertion, displace existing elements
  memmove(
      ar->v + ((index + len) * ar->esz)
    , ar->v + (index * ar->esz)
    , (ar->size - index) * ar->esz
  );

  store_items(ar, ar->v + (index * ar->esz), items, len);
  ar->size += len;
}

void array_update(array * const ar, size_t index, void * item)
{
  array_update_range(ar, index, item, 1);
}

void array_update_range(array * const ar, size_t index, void * items, size_t len)
{
  array_destroy_range(ar, index, len);
  store_items(ar, ar->v + (index * ar->esz), items, len);
}

void array_delete(array * const restrict ar, size_t index)
{
  array_destroy_range(ar, index, 1);

  // overwrite
  memmove(
      ar->v + (index * ar->esz)
    , ar->v + ((index + 1) * ar->esz)
    , (ar->size - index - 1) * ar->esz
  );

  ar->size--;
}

void array_delete_range(array * const restrict ar, size_t index, size_t len)
{
  array_destroy_range(ar, index, len);

  // overwrite
  memmove(
      ar->v + (index * ar->esz)
    , ar->v + ((index + len) * ar->esz)
    , (ar->size - index - len) * ar->esz
  );

  ar->size -= len;
}

void array_splice(array * const restrict dst, size_t dst_index, array * const restrict src, size_t src_index, size_t len)
{
  if(len == -1) {
    len = src->size;
  }

  array_replicate(dst, dst_index, src, src_index, len);

  // overwrite
  memmove(
      src->v + (src_index * src->esz)
    , src->v + ((src_index + len) * src->esz)
    , (src->size - src_index - len) * src->esz
  );
  src->size -= len;
}

void array_replicate(array * const restrict dst, size_t dst_index, array * const restrict src, size_t src_index, size_t len)
{
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

void array_truncate(array * const restrict ar, size_t len)
{
  if(len > ar->size) {
    return;
  }

  array_destroy_range(ar, len, ar->size - len);
  ar->size = len;
}

bool array_equal(array * const restrict A, array * const restrict B, int (*cmp)(const void * A, size_t Asz, const void * B, size_t Bsz))
{
  int x;
  void *a;
  void *b;

  if(A->size != B->size) {
    return false;
  }

  for(x = 0; x < A->size; x++)
  {
    a = A->v + (x * A->esz);
    b = B->v + (x * A->esz);
    if(cmp(a, A->esz, b, B->esz)) {
      return false;
    }
  }

  return true;
}

struct sort_ctx {
  const array * ar;
  int (*cmp)(const void * A, size_t Asz, const void * B, size_t Bsz);
};

static int sort_cmp(const void * A, const void * B, void * arg)
{
  struct sort_ctx * ctx = arg;
  return compare_items(ctx->ar, A, B, ctx->cmp);
}

void array_sort(const array * const restrict ar, int (*cmp)(const void * A, size_t Asz, const void * B, size_t Bsz))
{
  struct sort_ctx ctx = { ar : ar, cmp : cmp };
  qsort_r(ar->v, ar->size, ar->esz, sort_cmp, &ctx);
}

struct context {
  const array * ar;
  const void * key;
  int (*cmp)(const void * A, size_t Asz, const void * B, size_t Bsz);
  int lc;
  const void * last_item;
};

static int compare(const void * _ctx, const void * item)
{
  struct context * ctx = (typeof(ctx))_ctx;

  ctx->last_item = item;
  return ctx->lc = compare_items(ctx->ar, ctx->key, item, ctx->cmp);
};

bool array_search(
    const array * const restrict ar
  , void * key
  , int (*cmp)(const void * A, size_t Asz, const void * B, size_t Bsz)
  , void * item
  , size_t * restrict lx
  , int * restrict lc
)
{
  return array_search_range(ar, 0, ar->size, key, cmp, item, lx, lc);
}

bool array_search_range(
    const array * const restrict ar
  , size_t index
  , size_t len
  , void * key
  , int (*cmp)(const void * A, size_t Asz, const void * B, size_t Bsz)
  , void * item
  , size_t * restrict lx
  , int * restrict lc
)
{
  struct context ctx = { ar : ar, cmp : cmp, key : key };
  void * result = bsearch(&ctx, ar->v + (index * ar->esz), len, ar->esz, compare);

  if(item) {
    *(void**)item = result;
  }
  if(lx)
  {
    if(ctx.last_item)
      *lx = ((char*)ctx.last_item - ar->v) / ar->esz;
    else
      *lx = 0;
  }
  if(lc) {
    *lc = ctx.lc;
  }

  return !!result;
}
