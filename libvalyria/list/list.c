/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <stdlib.h>
#include <string.h>

#include "xlinux/xstdlib.h"
#include "common/assure.h"

#include "list.h"

#include "macros.h"

/*
 * Default capacity, the minimum number of entries which can be stored without reallocating
 */
#define DEFAULT_CAPACITY  10

//
// static
//

static void store(void ** storage, void ** items, size_t len)
{
  int x;

  for(x = 0; x < len; x++)
  {
    storage[x] = items[x];
  }
}

/*
 * free a range of elements in a list
 *
 * li    - pointer to list
 * start - index of the first element
 * len   - number of elements
 */
static void list_free_range(list * const restrict li, size_t start, size_t len)
{
  size_t x;

  if(!li->free_fn) {
    return;
  }

  for(x = 0; x < len; x++) {
    li->free_fn(li->v[start + x]);
  }
}

//
// api
//

void API list_initx(list * restrict li, size_t capacity, void (*free_fn)(void *))
{
  *li = (typeof(*li)) {
    free_fn : free_fn
  };

  assure(&li->v, sizeof(*li->v), capacity ?: DEFAULT_CAPACITY, &li->a);
}

void API list_init(list * restrict li)
{
  list_initx(li, 0, 0);
}

void API list_destroy(list * restrict li)
{
  list_truncate(li, 0);
  free(li->v);
}

void API list_createx(
    list ** restrict rv
  , size_t capacity
  , void (*free_fn)(void *)
)
{
  list * li = 0;

  xmalloc(&li, sizeof(*li));
  list_initx(li, capacity, free_fn);

  *rv = li;
}

void API list_create(list ** restrict li)
{
  list_createx(li, 0, 0);
}

void API list_free(list * restrict li)
{
  if(!li) {
    return;
  }

  list_destroy(li);
  free(li);
}

void API list_ifree(list ** restrict li)
{
  list_free(*li);
  *li = 0;
}

void API list_recycle(list * restrict li)
{
  list_truncate(li, 0);
}

void * API list_get(const list * restrict li, int x)
{
  return li->v[x];
}

void API list_push(list * restrict li, void * item)
{
  list_push_range(li, &item, 1);
}

void API list_push_range(list * restrict li, void * items, size_t len)
{
  list_insert_range(li, li->size, items, len);
}

void API list_pop(list * restrict li)
{
  if(li->size == 0) {
    return;
  }

  list_free_range(li, li->size - 1, 1);
  li->size--;
}

void API list_unshift(list * restrict li, void * el)
{
  list_unshift_range(li, &el, 1);
}

void API list_unshift_range(list * restrict li, void * items, size_t len)
{
  list_insert_range(li, 0, items, len);
}

void API list_shift(list * restrict li)
{
  if(li->size == 0) {
    return;
  }

  list_free_range(li, 0, 1);

  memmove(
      &li->v[0]
    , &li->v[1]
    , (li->size - 1) * sizeof(*li->v)
  );

  li->size--;
}

void API list_insert(list * restrict li, size_t index, void * item)
{
  list_insert_range(li, index, &item, 1);
}

void API list_insert_range(list * restrict li, size_t index, void * items, size_t len)
{
  // allocate new space if necessary
  assure(&li->v, sizeof(*li->v), li->size + len, &li->a);

  // for an insertion, displace existing elements
  memmove(
      &li->v[index + len]
    , &li->v[index]
    , (li->size - index) * sizeof(*li->v)
  );

  store(&li->v[index], items, len);
  li->size += len;
}

void API list_put(list * restrict li, size_t index, void * item)
{
  list_put_range(li, index, &item, 1);
}

void API list_put_range(list * restrict li, size_t index, void * items, size_t len)
{
  list_free_range(li, index, len);
  store(&li->v[index], items, len);
}

void API list_delete(list * restrict li, size_t index)
{
  list_delete_range(li, index, 1);
}

void API list_delete_range(list * restrict li, size_t index, size_t len)
{
  list_free_range(li, index, len);

  // overwrite
  memmove(
      &li->v[index]
    , &li->v[index + len]
    , (li->size - index - len) * sizeof(*li->v)
  );

  li->size -= len;
}

void API list_splice(list * restrict dst, size_t dst_index, list * restrict src, size_t src_index, int len)
{
  if(len == -1) {
    len = src->size;
  }

  list_replicate(dst, dst_index, src, src_index, len);

  // overwrite
  memmove(
      &src->v[src_index]
    , &src->v[src_index + len]
    , (src->size - src_index - len) * sizeof(*src->v)
  );
  src->size -= len;
}

void API list_replicate(list * restrict dst, size_t dst_index, list * restrict src, size_t src_index, int len)
{
  if(len == -1) {
    len = src->size;
  }

  // allocate new space if necessary
  assure(&dst->v, sizeof(*dst->v), dst->size + len, &dst->a);

  // displace existing elements
  memmove(
      &dst->v[dst_index + len]
    , &dst->v[dst_index]
    , (dst->size - dst_index) * sizeof(*dst->v)
  );

  // copy new elements in
  memcpy(
      &dst->v[dst_index]
    , &src->v[src_index]
    , len * sizeof(*src->v)
  );

  dst->size += len;
}

void API list_truncate(list * restrict li, size_t len)
{
  if(len > li->size) {
    return;
  }

  list_free_range(li, len, li->size - len);
  li->size = len;
}

bool API list_equal(list * restrict A, list * restrict B, int (*cmp_fn)(const void * A, const void * B, void *))
{
  int x;
  void *a;
  void *b;

  if(A->size != B->size) {
    return false;
  }

  for(x = 0; x < A->size; x++)
  {
    a = list_get(A, x);
    b = list_get(B, x);

    if(cmp_fn(a, b, 0)) {
      return false;
    }
  }

  return true;
}

void API list_sort(list * restrict li, int (*cmp_fn)(const void *A, const void *B, void *))
{
  qsort_r(li->v, li->size, sizeof(*li->v), cmp_fn, li);
}
