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

#include "array.h"

#include "grow.h"
#include "ensure.h"

/*
 * Default capacity, the minimum number of entries which can be stored without reallocating
 */
#define DEFAULT_CAPACITY  10

struct array
{
  char *   v;     // storage
  size_t   l;     // number of elements
  size_t   a;     // allocated size in elements

  size_t   esz;   // element size
  void (*destructor)(void *);
};

#define ELEMENT(ar, x) (ar)->v + ((x) * (ar)->esz)

#define restrict __restrict

//
// static
//

static xapi array_grow(array * const restrict ar, size_t len)
{
  xproxy(grow, &ar->v, ar->esz, len, ar->l, &ar->a);
}

static xapi array_ensure(array * const restrict ar, size_t len)
{
  xproxy(ensure, &ar->v, ar->esz, len, &ar->a);
}

//
// public
//

xapi array_createx(array ** const restrict ar, size_t esz, void (*destructor)(ARRAY_ELEMENT_TYPE *), size_t capacity)
{
  enter;

  fatal(xmalloc, ar, sizeof(**ar));

  (*ar)->esz = esz;
  (*ar)->destructor = destructor;

  fatal(array_grow, *ar, capacity);

  finally : coda;
}

xapi array_create(array ** const restrict ar, size_t esz, void (*destructor)(ARRAY_ELEMENT_TYPE *))
{
  xproxy(array_createx, ar, esz, destructor, DEFAULT_CAPACITY);
}

void array_free(array * const restrict ar)
{
  if(ar)
  {
    array_clear(ar);
    free(ar->v);
  }
  free(ar);
}

void array_xfree(array ** const restrict ar)
{
  array_free(*ar);
  *ar = 0;
}

size_t array_size(const array * const restrict ar)
{
  return ar->l;
}

void array_clear(array * const restrict ar)
{
  if(ar->destructor)
  {
    int x;
    for(x = 0; x < ar->l; x++)
    {
      ar->destructor(ELEMENT(ar, x));
    }
  }

  ar->l = 0;
}

ARRAY_ELEMENT_TYPE * array_get(const array * const restrict ar, int x)
{
  return ELEMENT(ar, x);
}

xapi array_shift(array * const restrict ar, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  enter;

  ARRAY_ELEMENT_TYPE * e = 0;
  if(ar->l)
  {
    e = ELEMENT(ar, 0);
    if(ar->destructor)
      ar->destructor(e);

    memmove(
        ar->v
      , ar->v + ar->esz
      , (ar->l - 1) * ar->esz
    );

    ar->l--;
  }

  if(el)
    *el = e;

  finally : coda;
}

xapi array_pop(array * const restrict ar, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  enter;

  ARRAY_ELEMENT_TYPE * e = 0;
  if(ar->l)
  {
    e = ELEMENT(ar, ar->l - 1);
    if(ar->destructor)
      ar->destructor(e);
    ar->l--;
  }
  
  if(el)
    *el = e;

  finally : coda;
}

xapi array_push(array * const restrict ar, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  xproxy(array_insert_range, ar, ar->l, 1, el);
}

xapi array_push_range(array * const restrict ar, size_t len, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  xproxy(array_insert_range, ar, ar->l, len, el);
}

xapi array_unshift(array * const restrict ar, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  xproxy(array_insert_range, ar, 0, 1, el);
}

xapi array_unshift_range(array * const restrict ar, size_t len, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  xproxy(array_insert_range, ar, 0, len, el);
}

xapi array_insert(array * const restrict ar, size_t index, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  xproxy(array_insert_range, ar, index, 1, el);
}

xapi array_insert_range(array * const restrict ar, size_t index, size_t len, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  enter;

  fatal(array_ensure, ar, ar->l + len);

  memmove(
      ar->v + ((index + len) * ar->esz)
    , ar->v + (index * ar->esz)
    , (ar->l - index + len) * ar->esz
  );

  size_t x;
  for(x = 0; x < len; x++)
    el[x] = ELEMENT(ar, index + x);

  ar->l += len;

  finally : coda;
}

void array_sort(array * const restrict ar, int (*compar)(const ARRAY_ELEMENT_TYPE *, const ARRAY_ELEMENT_TYPE *, void *), void * arg)
{
  qsort_r(ar->v, ar->l, ar->esz, compar, arg);
}
