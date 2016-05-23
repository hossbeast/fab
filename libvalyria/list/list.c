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

#include "xlinux.h"

#include "internal.h"
#define LIST_INTERNALS struct list_internals
#include "list.def.h"
#include "list.h"

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

//
// protected
//

xapi list_allocate(list ** const restrict rv, uint32_t attr, size_t esz, void (*destructor)(LIST_ELEMENT_TYPE *), size_t capacity)
{
  enter;

  list * li = 0;
  fatal(xmalloc, &li, sizeof(*li));

  li->destructor = destructor;
  li->esz = esz;
  li->attr = attr;

  fatal(list_grow, li, capacity ?: DEFAULT_CAPACITY);

  *rv = li;
  li = 0;

finally:
  list_free(li);
coda;
}

xapi list_add(list * const restrict li, size_t index, size_t len, LIST_ELEMENT_TYPE * const * const el, LIST_ELEMENT_TYPE ** const restrict rv)
{
  enter;

  // allocate new space if necessary
  fatal(assure, &li->v, ELEMENT_SIZE(li), li->l + len, &li->a);

  // for an insertion, displace existing elements
  memmove(
      li->v + ((index + len) * ELEMENT_SIZE(li))
    , li->v + (index * ELEMENT_SIZE(li))
    , (li->l - index) * ELEMENT_SIZE(li)
  );

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

  li->l += len;

  finally : coda;
}

//
// api
//

API xapi list_createx(list** const restrict li, void (*destructor)(LIST_ELEMENT_TYPE *), size_t capacity)
{
  xproxy(list_allocate, li, LIST_SECONDARY, 0, destructor, capacity);
}

API xapi list_create(list** const restrict li, void (*destructor)(LIST_ELEMENT_TYPE *))
{
  xproxy(list_allocate, li, LIST_SECONDARY, 0, destructor, 0);
}

API void list_free(list * const restrict li)
{
  if(li)
  {
    list_clear(li);
    free(li->v);
  }
  free(li);
}

API void list_ifree(list ** const restrict li)
{
  list_free(*li);
  *li = 0;
}

API size_t list_size(const list * const restrict li)
{
  return li->l;
}

API void list_clear(list * const restrict li)
{
  if(li->destructor)
  {
    int x;
    for(x = 0; x < li->l; x++)
    {
      li->destructor(ELEMENT(li, x));
    }
  }

  li->l = 0;
}

API LIST_ELEMENT_TYPE * list_get(const list * const restrict li, int x)
{
  return ELEMENT(li, x);
}

API xapi list_shift(list * const restrict li, LIST_ELEMENT_TYPE ** const restrict el)
{
  enter;

  LIST_ELEMENT_TYPE * e = 0;
  if(li->l)
  {
    e = ELEMENT(li, 0);
    if(li->destructor)
      li->destructor(e);

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

API xapi list_pop(list * const restrict li, LIST_ELEMENT_TYPE ** const restrict el)
{
  enter;

  void * e = 0;
  if(li->l)
  {
    e = ELEMENT(li, li->l - 1);
    if(li->destructor)
      li->destructor(e);
    li->l--;
  }
  if(el)
    *el = e;

  finally : coda;
}

API xapi list_push(list * const restrict li, LIST_ELEMENT_TYPE * const el)
{
  xproxy(list_add, li, li->l, 1, &el, 0);
}

API xapi list_push_range(list * const restrict li, LIST_ELEMENT_TYPE * const * const el, size_t len)
{
  xproxy(list_add, li, li->l, len, el, 0);
}

API xapi list_unshift(list * const restrict li, LIST_ELEMENT_TYPE * const el)
{
  xproxy(list_add, li, 0, 1, &el, 0);
}

API xapi list_unshift_range(list * const restrict li, LIST_ELEMENT_TYPE * const * const el, size_t len)
{
  xproxy(list_add, li, 0, len, el, 0);
}

API xapi list_insert(list * const restrict li, size_t index, LIST_ELEMENT_TYPE * const el)
{
  xproxy(list_add, li, index, 1, &el, 0);
}

API xapi list_insert_range(list * const restrict li, size_t index, LIST_ELEMENT_TYPE * const * const el, size_t len)
{
  xproxy(list_add, li, index, len, el, 0);
}

API void list_sort(list * const restrict li, int (*compar)(const LIST_ELEMENT_TYPE *, const LIST_ELEMENT_TYPE *, void *), void * arg)
{
  if(li->attr & LIST_PRIMARY)
  {
    qsort_r(li->v, li->l, ELEMENT_SIZE(li), compar, arg);
  }

  else if(li->attr & LIST_SECONDARY)
  {
    int lcompar(const void * A, const void * B, void * arg)
    {
      return compar(*(const LIST_ELEMENT_TYPE **)A, *(const LIST_ELEMENT_TYPE **)B, arg);
    };

    qsort_r(li->v, li->l, ELEMENT_SIZE(li), lcompar, arg);
  }
}

API xapi list_sublist(list * const restrict li, size_t index, size_t len, list ** const restrict rv)
{
  enter;

  if(!*rv)
    fatal(list_create, rv, li->destructor);
  list_clear(*rv);

  fatal(list_assure, *rv, len);
  memcpy(
      (*rv)->v
    , li->v + (index * ELEMENT_SIZE(li))
    , len * ELEMENT_SIZE(li)
  );
  (*rv)->l = len;

  finally : coda;
}

API LIST_ELEMENT_TYPE * list_searchx(list * const restrict li, const void * const restrict key, int (*compar)(const void *, const LIST_ELEMENT_TYPE *), size_t * restrict lx, int * restrict lc)
{
  LIST_ELEMENT_TYPE * elp = 0;

  if(li->attr & LIST_PRIMARY)
  {
    int pcompar(const void * key, const void * el)
    {
      int r = compar(key, el);
      if(lx)
        *lx = ((char*)el - li->v) / ELEMENT_SIZE(li);
      if(lc)
        *lc = r;
      return r;
    };

    elp = bsearch(key, li->v, li->l, ELEMENT_SIZE(li), pcompar);
  }

  else if(li->attr & LIST_SECONDARY)
  {
    int scompar(const void * key, const void * el)
    {
      int r = compar(key, *(const LIST_ELEMENT_TYPE **)el);
      if(lx)
        *lx = ((char*)el - li->v) / ELEMENT_SIZE(li);
      if(lc)
        *lc = r;
      return r;
    }

    void * res;
    if((res = bsearch(key, li->v, li->l, ELEMENT_SIZE(li), scompar)))
      elp = *(LIST_ELEMENT_TYPE **)res;
  }

  return elp;
}

API LIST_ELEMENT_TYPE * list_search(list * const restrict li, const void * const restrict key, int (*compar)(const void *, const LIST_ELEMENT_TYPE *))
{
  return list_searchx(li, key, compar, 0, 0);
}
