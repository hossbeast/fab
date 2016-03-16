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

#include "array.h"

#include "list.h"
#include "list.def.h"

struct array
{
  struct list;
};

//
// public
//

xapi array_create(array ** const restrict ar, size_t esz)
{
  xproxy(list_allocate, (void*)ar, LIST_PRIMARY, esz, 0, 0);
}

xapi array_createx(array ** const restrict ar, size_t esz, void (*destructor)(ARRAY_ELEMENT_TYPE *), size_t capacity)
{
  xproxy(list_allocate, (void*)ar, LIST_PRIMARY, esz, destructor, capacity);
}

void array_free(array * const restrict ar)
{
  return list_free(ar);
}

void array_xfree(array ** const restrict ar)
{
  list_xfree((void*)ar);
}

size_t array_size(const array * const restrict ar)
{
  return list_size(ar);
}

void array_clear(array * const restrict ar)
{
  return list_clear(ar);
}

ARRAY_ELEMENT_TYPE * array_get(const array * const restrict ar, int x)
{
  return list_get(ar, x);
}

xapi array_shift(array * const restrict ar, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  xproxy(list_shift, ar, el);
}

xapi array_pop(array * const restrict ar, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  xproxy(list_pop, ar, el);
}

xapi array_push(array * const restrict ar, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  xproxy(list_add, ar, ar->l, 1, 0, el);
}

xapi array_push_range(array * const restrict ar, size_t len, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  xproxy(list_add, ar, ar->l, len, 0, el);
}

xapi array_unshift(array * const restrict ar, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  xproxy(list_add, ar, 0, 1, 0, el);
}

xapi array_unshift_range(array * const restrict ar, size_t len, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  xproxy(list_add, ar, 0, len, 0, el);
}

xapi array_insert(array * const restrict ar, size_t index, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  xproxy(list_add, ar, index, 1, 0, el);
}

xapi array_insert_range(array * const restrict ar, size_t index, size_t len, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  xproxy(list_add, ar, index, len, 0, el);
}

void array_sort(array * const restrict ar, int (*compar)(const ARRAY_ELEMENT_TYPE *, const ARRAY_ELEMENT_TYPE *, void *), void * arg)
{
  list_sort(ar, compar, arg);
}
