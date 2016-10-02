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
#define LIST_INTERNALS struct list_internals
#include "list.def.h"
#include "list.internal.h"

#include "grow.h"
#include "assure.h"

/*
 * Default capacity, the minimum number of entries which can be stored without reallocating
 */
#define DEFAULT_CAPACITY  10

#define ELEMENT_SIZE(li) ({       \
  size_t elz = sizeof(void*);     \
  if((li)->attr & LIST_PRIMARY)   \
  {                               \
    elz = (li)->esz;              \
  }                               \
  elz;                            \
})

#define ELEMENT(li, x) ({                               \
  void * el;                                            \
  if((li)->attr & LIST_PRIMARY)                         \
  {                                                     \
    el = (li)->v + ((x) * ELEMENT_SIZE(li));            \
  }                                                     \
  else                                                  \
  {                                                     \
    el = *(char**)((li)->v + ((x) * ELEMENT_SIZE(li))); \
  }                                                     \
  el;                                                   \
})

#define restrict __restrict

//
// static
//

static xapi list_grow(list * const restrict li, size_t len)
{
  xproxy(grow, &li->v, ELEMENT_SIZE(li), len, li->l, &li->a);
}

static xapi list_assure(list * const restrict li, size_t len)
{
  xproxy(assure, &li->v, ELEMENT_SIZE(li), len, &li->a);
}

/// list_update
//
// SUMMARY
//  overwrite elements in the list
//
static void list_update(list * const restrict li, size_t index, size_t len, void * el, void ** const restrict rv)
{
  // copy the elements into place
  if(el)
  {
    memcpy(
        li->v + (index * ELEMENT_SIZE(li))
      , el
      , len * ELEMENT_SIZE(li)
    );
  }

  // return pointers to the elements
  if(rv)
  {
    size_t x;
    for(x = 0; x < len; x++)
      rv[x] = ELEMENT(li, index + x);
  }
}

/// list_move
//
// SUMMARY
//  copy elements from one list to another
//
// REMARKS
//  the lists must be compatible, e.g. have the same element size and value_free callbacks
//
static void list_move(list * const restrict dst, size_t dst_index, list * const restrict src, size_t src_index, size_t len)
{
  size_t elsz = ELEMENT_SIZE(dst);

  memcpy(
      dst->v + (dst_index * elsz)
    , src->v + (src_index * elsz)
    , len * elsz
  );
}

/// list_free_range
//
// SUMMARY
//  free a range of elements in a list
//
// PARAMETERS
//  li    - pointer to list
//  start - index of the first element
//  len   - number of elements
//
static xapi list_free_range(list * const restrict li, size_t start, size_t len)
{
  enter;

  size_t x;

  if(li->free_element)
  {
    for(x = 0; x < len; x++)
      li->free_element(ELEMENT(li, start + x));
  }
  else if(li->xfree_element)
  {
    for(x = 0; x < len; x++)
      fatal(li->xfree_element, ELEMENT(li, start + x));
  }

  finally : coda;
}

//
// protected
//

xapi list_allocate(
    list ** const restrict rv
  , uint32_t attr
  , size_t esz
  , void * free_element
  , void * xfree_element
  , size_t capacity
)
{
  enter;

  list * li = 0;
  fatal(xmalloc, &li, sizeof(*li));

  li->free_element = free_element;
  li->xfree_element = xfree_element;
  li->esz = esz;
  li->attr = attr;

  fatal(list_grow, li, capacity ?: DEFAULT_CAPACITY);

  *rv = li;
  li = 0;

finally:
  fatal(list_xfree, li);
coda;
}

xapi list_put(list * const restrict li, size_t index, size_t len, void * el, void ** const restrict rv)
{
  enter;

  // destroy existing elements
  fatal(list_free_range, li, index, len);

  list_update(li, index, len, el, rv);

  finally : coda;
}

xapi list_add(list * const restrict li, size_t index, size_t len, void * el, void ** const restrict rv)
{
  enter;

  // allocate new space if necessary
  fatal(list_assure, li, li->l + len);

  // for an insertion, displace existing elements
  memmove(
      li->v + ((index + len) * ELEMENT_SIZE(li))
    , li->v + (index * ELEMENT_SIZE(li))
    , (li->l - index) * ELEMENT_SIZE(li)
  );

  list_update(li, index, len, el, rv);
  li->l += len;

  finally : coda;
}

//
// api
//

API xapi list_createx(
    list** const restrict li
  , void * free_element
  , void * xfree_element
  , size_t capacity
)
{
  xproxy(list_allocate, li, LIST_SECONDARY, 0, free_element, xfree_element, capacity);
}

API xapi list_create(list** const restrict li)
{
  xproxy(list_allocate, li, LIST_SECONDARY, 0, 0, 0, 0);
}

API xapi list_xfree(list * const restrict li)
{
  enter;

  if(li)
  {
    fatal(list_recycle, li);
    wfree(li->v);
  }
  wfree(li);

  finally : coda;
}

API xapi list_ixfree(list ** const restrict li)
{
  enter;

  fatal(list_xfree, *li);
  *li = 0;

  finally : coda;
}

API size_t list_size(const list * const restrict li)
{
  return li->l;
}

API xapi list_recycle(list * const restrict li)
{
  enter;

  fatal(list_free_range, li, 0, li->l);
  li->l = 0;

  finally : coda;
}

API void * list_get(const list * const restrict li, int x)
{
  return ELEMENT(li, x);
}

API xapi list_shift(list * const restrict li, void ** const restrict el)
{
  enter;

  void * e = 0;
  if(li->l)
  {
    e = ELEMENT(li, 0);
    if(li->free_element)
      li->free_element(e);
    else if(li->xfree_element)
      fatal(li->xfree_element, e);

    memmove(
        li->v
      , li->v + ELEMENT_SIZE(li)
      , (li->l - 1) * ELEMENT_SIZE(li)
    );

    li->l--;
  }
  if(el)
    *el = e;

  finally : coda;
}

API xapi list_pop(list * const restrict li, void ** const restrict el)
{
  enter;

  void * e = 0;
  if(li->l)
  {
    e = ELEMENT(li, li->l - 1);
    if(li->free_element)
      li->free_element(e);
    else if(li->xfree_element)
      fatal(li->xfree_element, e);
    li->l--;
  }
  if(el)
    *el = e;

  finally : coda;
}

API xapi list_push(list * const restrict li, void * el)
{
  xproxy(list_add, li, li->l, 1, &el, 0);
}

API xapi list_push_range(list * const restrict li, void * el, size_t len)
{
  xproxy(list_add, li, li->l, len, el, 0);
}

API xapi list_unshift(list * const restrict li, void * el)
{
  xproxy(list_add, li, 0, 1, &el, 0);
}

API xapi list_unshift_range(list * const restrict li, void * el, size_t len)
{
  xproxy(list_add, li, 0, len, el, 0);
}

API xapi list_insert(list * const restrict li, size_t index, void * el)
{
  xproxy(list_add, li, index, 1, &el, 0);
}

API xapi list_insert_range(list * const restrict li, size_t index, void * el, size_t len)
{
  xproxy(list_add, li, index, len, el, 0);
}

API xapi list_set(list * const restrict li, size_t index, void * el)
{
  xproxy(list_put, li, index, 1, &el, 0);  
}

API xapi list_set_range(list * const restrict li, size_t index, void * el, size_t len)
{
  xproxy(list_put, li, index, len, el, 0);  
}

API void list_sort(list * const restrict li, int (*compar)(const void *, const void *, void *), void * arg)
{
  if(li->attr & LIST_PRIMARY)
  {
    qsort_r(li->v, li->l, ELEMENT_SIZE(li), compar, arg);
  }

  else if(li->attr & LIST_SECONDARY)
  {
    int lcompar(const void * A, const void * B, void * arg)
    {
      return compar(*(const void **)A, *(const void **)B, arg);
    };

    qsort_r(li->v, li->l, ELEMENT_SIZE(li), lcompar, arg);
  }
}

API xapi list_sublist(list * const restrict li, size_t index, size_t len, list ** const restrict rv)
{
  enter;

  if(!*rv)
    fatal(list_createx, rv, li->free_element, li->xfree_element, len);
  fatal(list_recycle, *rv);

  fatal(list_assure, *rv, len);
  memcpy(
      (*rv)->v
    , li->v + (index * ELEMENT_SIZE(li))
    , len * ELEMENT_SIZE(li)
  );
  (*rv)->l = len;

  finally : coda;
}

struct context
{
  int (*compar)(void *, const void *, size_t);
  const char * v;
  size_t esz;
  void * ud;
};

static int pcompar(struct context * ctx, const void * el)
{
  return ctx->compar(ctx->ud, el, ((char*)el - ctx->v) / ctx->esz);
};

static int scompar(struct context * ctx, const void * el)
{
  return ctx->compar(ctx->ud, *(const void **)el, ((char*)el - ctx->v) / ctx->esz);
}

API void * list_search(list * const restrict li, void * ud, int (*compar)(void *, const void *, size_t))
{
  void * elp = 0;

  struct context ctx = { v : li->v, ud : ud, esz : ELEMENT_SIZE(li), compar : compar };

  if(li->attr & LIST_PRIMARY)
  {
    elp = bsearch(&ctx, li->v, li->l, ctx.esz, (void*)pcompar);
  }
  else // li->attr & LIST_SECONDARY
  {

    void * res;
    if((res = bsearch(&ctx, li->v, li->l, ctx.esz, (void*)scompar)))
      elp = *(void **)res;
  }

  return elp;
}

API xapi list_splice(list * const restrict dst, size_t dst_index, list * const restrict src, size_t src_index, size_t len)
{
  enter;

  fatal(list_free_range, dst, dst_index, len);
  list_move(dst, dst_index, src, src_index, len);

  finally : coda;
}

API xapi list_replicate(list * const restrict dst, size_t dst_index, list * const restrict src, size_t src_index, size_t len)
{
  enter;

  // allocate new space if necessary
  fatal(list_assure, dst, dst->l + len);

  // displace existing elements
  memmove(
      dst->v + ((dst_index + len) * ELEMENT_SIZE(dst))
    , dst->v + (dst_index * ELEMENT_SIZE(dst))
    , (dst->l - dst_index) * ELEMENT_SIZE(dst)
  );
  
  list_move(dst, dst_index, src, src_index, len);
  dst->l += len;

  finally : coda;
}
