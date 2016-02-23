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
  char *   v;     // storage
  size_t   l;     // number of elements
  size_t   a;     // allocated size in elements

  size_t   esz;   // element size
  uint32_t attr;  // options and modifiers

  void (*destructor)(void *);
};

#define ELEMENT(listp, x) ({                            \
  void * el;                                            \
  if(listp->attr & LIST_PRIMARY)                        \
  {                                                     \
    el = listp->v + (x * SLOT_SIZE(listp));             \
  }                                                     \
  else                                                  \
  {                                                     \
    el = *(char**)(listp->v + (x * SLOT_SIZE(listp)));  \
  }                                                     \
  el;                                                   \
})

#define SLOT_SIZE(listp) ({         \
  size_t slotz = sizeof(void*);     \
  if(listp->attr == LIST_PRIMARY)   \
    slotz = listp->esz;             \
  slotz;                            \
})

//
// public
//

xapi list_create(list** const restrict listp, size_t esz, void (*destructor)(void *), uint32_t attr)
{
  enter;

  // apply defaults
  if((attr != LIST_PRIMARY) && (attr != LIST_SECONDARY))
  {
    attr = LIST_PRIMARY;
  }

  fatal(xmalloc, listp, sizeof(**listp));

  (*listp)->esz = esz;
  (*listp)->destructor = destructor;
  (*listp)->attr = attr;

  finally : coda;
}

void list_free(list * const restrict listp)
{
  if(listp)
  {
    list_clear(listp);
    free(listp->v);
  }
  free(listp);
}

void list_xfree(list ** const restrict listp)
{
  list_free(*listp);
  *listp = 0;
}

size_t list_size(list * const restrict listp)
{
  return listp->l;
}

void list_clear(list * const restrict listp)
{
  if(listp->destructor)
  {
    int x;
    for(x = 0; x < listp->l; x++)
    {
      listp->destructor(ELEMENT(listp, x));
    }
  }

  listp->l = 0;
}

void * list_get(list * const restrict listp, int x)
{
  return ELEMENT(listp, x);
}

void * list_shift(list * const restrict listp)
{
  void * e = 0;
  if(listp->l > 0)
  {
    e = ELEMENT(listp, listp->l - 1);
    if(listp->destructor)
      listp->destructor(e);
    listp->l--;
  }
  return e;
}

void * list_pop(list * const restrict listp)
{
  void * e = 0;
  if(listp->l > 0)
  {
    e = ELEMENT(listp, listp->l - 1);
    if(listp->destructor)
      listp->destructor(e);
    listp->l--;
  }
  return e;
}

xapi list_push(list * const restrict listp, void * const restrict el)
{
  xproxy(list_insert_range, listp, listp->l, el, 1);
}

xapi list_push_range(list * const restrict listp, void * const restrict el, size_t len)
{
  xproxy(list_insert_range, listp, listp->l, el, len);
}

xapi list_unshift(list * const restrict listp, void * const restrict el)
{
  xproxy(list_insert_range, listp, 0, el, 1);
}

xapi list_unshift_range(list * const restrict listp, void * const restrict el, size_t len)
{
  xproxy(list_insert_range, listp, 0, el, len);
}

xapi list_grow(list * const restrict listp, size_t len)
{
  xproxy(grow, &listp->v, SLOT_SIZE(listp), len, listp->l, &listp->a);
}

xapi list_ensure(list * const restrict listp, size_t len)
{
  xproxy(ensure, &listp->v, SLOT_SIZE(listp), len, &listp->a);
}

xapi list_insert(list * const restrict listp, size_t index, void * const restrict el)
{
  xproxy(list_insert_range, listp, index, el, 1);
}

xapi list_insert_range(list * const restrict listp, size_t index, void * const restrict el, size_t len)
{
  enter;

  fatal(ensure, &listp->v, SLOT_SIZE(listp), listp->l + len, &listp->a);

  memmove(
      listp->v + ((index + len) * SLOT_SIZE(listp))
    , listp->v + (index * SLOT_SIZE(listp))
    , (listp->l - index + len) * SLOT_SIZE(listp)
  );

  if(listp->attr == LIST_PRIMARY)
  {
    size_t x;
    for(x = 0; x < len; x++)
      ((void **)el)[x] = listp->v + (SLOT_SIZE(listp) * (index + x));
  }
  else
  {
    memcpy(
        listp->v + (index * SLOT_SIZE(listp))
      , el
      , len * SLOT_SIZE(listp)
    );
  }
  listp->l += len;

  finally : coda;
}

void list_sort(list * const restrict listp, int (*compar)(const void *, const void *, void *), void * arg)
{
  if(listp->attr & LIST_PRIMARY)
  {
    qsort_r(listp->v, listp->l, listp->esz, compar, arg);
  }
  else
  {
    int lcompar(const void * A, const void * B, void * arg)
    {
      return compar(*(const void **)A, *(const void**)B, arg);
    };

    qsort_r(listp->v, listp->l, SLOT_SIZE(listp), lcompar, arg);
  }
}

xapi list_sublist(list * const restrict listp, size_t index, size_t len, list ** const restrict rv)
{
  enter;

  if(!*rv)
    fatal(list_create, rv, listp->esz, listp->destructor, listp->attr);
  list_clear(*rv);

  fatal(list_ensure, *rv, len);
  memcpy((*rv)->v, listp->v + (index * SLOT_SIZE(listp)) , len * SLOT_SIZE(listp));
  (*rv)->l = len;

  finally : coda;
}
