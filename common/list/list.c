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

#include "list.h"

#include "grow.h"
#include "ensure.h"

/*
 * Default capacity, the minimum number of entries which can be stored without reallocating
 */
#define DEFAULT_CAPACITY  10

struct list
{
  char *   v;     // storage
  size_t   l;     // number of elements
  size_t   a;     // allocated size in elements

  void (*destructor)(void *);
};

#define ELEMENT(li, x) *(char**)((li)->v + ((x) * sizeof(void*)))

#define restrict __restrict

//
// static
//

static xapi list_grow(list * const restrict li, size_t len)
{
  xproxy(grow, &li->v, sizeof(void*), len, li->l, &li->a);
}

static xapi list_ensure(list * const restrict li, size_t len)
{
  xproxy(ensure, &li->v, sizeof(void*), len, &li->a);
}

//
// public
//

xapi list_createx(list** const restrict li, void (*destructor)(LIST_ELEMENT_TYPE *), size_t capacity)
{
  enter;

  fatal(xmalloc, li, sizeof(**li));

  (*li)->destructor = destructor;

  fatal(list_grow, *li, capacity);

  finally : coda;
}

xapi list_create(list** const restrict li, void (*destructor)(LIST_ELEMENT_TYPE *))
{
  xproxy(list_createx, li, destructor, DEFAULT_CAPACITY);
}

void list_free(list * const restrict li)
{
  if(li)
  {
    list_clear(li);
    free(li->v);
  }
  free(li);
}

void list_xfree(list ** const restrict li)
{
  list_free(*li);
  *li = 0;
}

size_t list_size(const list * const restrict li)
{
  return li->l;
}

void list_clear(list * const restrict li)
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

LIST_ELEMENT_TYPE * list_get(const list * const restrict li, int x)
{
  return ELEMENT(li, x);
}

xapi list_shift(list * const restrict li, LIST_ELEMENT_TYPE ** const restrict el)
{
  enter;

  void * e = 0;
  if(li->l)
  {
    e = ELEMENT(li, 0);
    if(li->destructor)
      li->destructor(e);

    memmove(
        li->v
      , li->v + sizeof(void*)
      , (li->l - 1) * sizeof(void*)
    );

    li->l--;
  }
  if(el)
    *el = e;

  finally : coda;
}

xapi list_pop(list * const restrict li, LIST_ELEMENT_TYPE ** const restrict el)
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

xapi list_push(list * const restrict li, LIST_ELEMENT_TYPE * const el)
{
  xproxy(list_insert_range, li, li->l, &el, 1);
}

xapi list_push_range(list * const restrict li, LIST_ELEMENT_TYPE * const * const el, size_t len)
{
  xproxy(list_insert_range, li, li->l, el, len);
}

xapi list_unshift(list * const restrict li, LIST_ELEMENT_TYPE * const el)
{
  xproxy(list_insert_range, li, 0, &el, 1);
}

xapi list_unshift_range(list * const restrict li, LIST_ELEMENT_TYPE * const * const el, size_t len)
{
  xproxy(list_insert_range, li, 0, el, len);
}

xapi list_insert(list * const restrict li, size_t index, LIST_ELEMENT_TYPE * const el)
{
  xproxy(list_insert_range, li, index, &el, 1);
}

xapi list_insert_range(list * const restrict li, size_t index, LIST_ELEMENT_TYPE * const * const el, size_t len)
{
  enter;

  fatal(ensure, &li->v, sizeof(void*), li->l + len, &li->a);

  memmove(
      li->v + ((index + len) * sizeof(void*))
    , li->v + (index * sizeof(void*))
    , (li->l - index + len) * sizeof(void*)
  );

  memcpy(
      li->v + (index * sizeof(void*))
    , el
    , len * sizeof(void*)
  );
  li->l += len;

  finally : coda;
}

void list_sort(list * const restrict li, int (*compar)(const LIST_ELEMENT_TYPE *, const LIST_ELEMENT_TYPE *, void *), void * arg)
{
  int lcompar(const void * A, const void * B, void * arg)
  {
    return compar(*(const LIST_ELEMENT_TYPE **)A, *(const LIST_ELEMENT_TYPE **)B, arg);
  };

  qsort_r(li->v, li->l, sizeof(void*), lcompar, arg);
}

xapi list_sublist(list * const restrict li, size_t index, size_t len, list ** const restrict rv)
{
  enter;

  if(!*rv)
    fatal(list_create, rv, li->destructor);
  list_clear(*rv);

  fatal(list_ensure, *rv, len);
  memcpy(
      (*rv)->v
    , li->v + (index * sizeof(void*))
    , len * sizeof(void*)
  );
  (*rv)->l = len;

  finally : coda;
}
