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

#define restrict __restrict

struct list
{
  char *  v;      // storage
  size_t  l;      // number of elements
  size_t  a;      // allocated size in elements

  size_t  esz;    // element size
  uint32_t attr;  // options and modifiers

  void (*destructor)(void *);
};

#define ELEMENT(x) ({                             \
  void * el;                                      \
  if(list->attr & LIST_DEREF)                     \
    el = *(char**)(list->v + (list->esz * (x)));  \
  else                                            \
    el = list->v + (list->esz * (x));             \
  el;                                             \
})

//
// public
//

xapi list_create(list** const restrict list, size_t esz, void (*destructor)(void *), uint32_t attr)
{
  enter;

  fatal(xmalloc, list, sizeof(**list));

  (*list)->esz = esz;
  (*list)->destructor = destructor;
  (*list)->attr = attr;

  finally : coda;
}

void list_free(list * const restrict list)
{
  if(list)
  {
    list_clear(list);
    free(list->v);
  }
}

void list_xfree(list ** const restrict list)
{
  list_free(*list);
  *list = 0;
}

size_t list_size(list * const restrict list)
{
  return list->l;
}

void list_clear(list * const restrict list)
{
  if(list->destructor)
  {
    int x;
    for(x = 0; x < list->l; x++)
    {
      list->destructor(ELEMENT(x));
    }
  }

  list->l = 0;
}

void * list_get(list * const restrict list, int x)
{
  return ELEMENT(x);
}

xapi list_append(list * const restrict list, void * const restrict el)
{
  enter;

  fatal(grow, &list->v, list->esz, 1, list->l, &list->a);
  memcpy(list->v + (list->esz * list->l), el, list->esz);
  list->l++;

  finally : coda;
}

xapi list_append_range(list * const restrict list, void * const restrict el, size_t len)
{
  enter;

  fatal(grow, &list->v, list->esz, len, list->l, &list->a);
  memcpy(list->v + (list->esz * list->l), el, list->esz * len);
  list->l += len;

  finally : coda;
}

xapi list_grow(list * const restrict list, size_t len)
{
  xproxy(grow, &list->v, list->esz, len, list->l, &list->a);
}

xapi list_ensure(list * const restrict list, size_t len)
{
  xproxy(ensure, &list->v, list->esz, len, &list->a);
}

xapi list_insert(list * const restrict list, size_t index, void * const restrict el)
{
  enter;

  fatal(ensure, &list->v, list->esz, index + 1, &list->a);

  memmove(
      list->v + ((index + 1) * list->esz)
    , list->v + (index * list->esz)
    , (list->l - index) * list->esz
  );

  memcpy(list->v + (list->esz * index), el, list->esz);
  list->l++;

  finally : coda;
}

void list_sort(list * const restrict list, int (*compar)(const void *, const void *, void *), void * arg)
{
  qsort_r(list->v, list->l, list->esz, compar, arg);
}
