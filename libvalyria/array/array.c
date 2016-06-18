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

#include "internal.h"
#include "list.def.h"
#include "list.internal.h"

#define ARRAY_INTERNALS struct list_internals
#include "array.h"

//
// api
//

API xapi array_create(array ** const restrict ar, size_t esz)
{
  xproxy(list_allocate, (void*)ar, LIST_PRIMARY, esz, 0, 0);
}

API xapi array_createx(array ** const restrict ar, size_t esz, void (*destructor)(ARRAY_ELEMENT_TYPE *), size_t capacity)
{
  xproxy(list_allocate, (void*)ar, LIST_PRIMARY, esz, destructor, capacity);
}

API void array_free(array * const restrict ar)
{
  return list_free((void*)ar);
}

API void array_ifree(array ** const restrict ar)
{
  list_ifree((void*)ar);
}

API size_t array_size(const array * const restrict ar)
{
  return list_size((void*)ar);
}

API void array_clear(array * const restrict ar)
{
  return list_clear((void*)ar);
}

API ARRAY_ELEMENT_TYPE * array_get(const array * const restrict ar, int x)
{
  return list_get((void*)ar, x);
}

API xapi array_shift(array * const restrict ar, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  xproxy(list_shift, (void*)ar, el);
}

API xapi array_pop(array * const restrict ar, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  xproxy(list_pop, (void*)ar, el);
}

API xapi array_push(array * const restrict ar, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  xproxy(list_add, (void*)ar, ar->l, 1, 0, el);
}

API xapi array_push_range(array * const restrict ar, size_t len, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  xproxy(list_add, (void*)ar, ar->l, len, 0, el);
}

API xapi array_unshift(array * const restrict ar, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  xproxy(list_add, (void*)ar, 0, 1, 0, el);
}

API xapi array_unshift_range(array * const restrict ar, size_t len, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  xproxy(list_add, (void*)ar, 0, len, 0, el);
}

API xapi array_insert(array * const restrict ar, size_t index, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  xproxy(list_add, (void*)ar, index, 1, 0, el);
}

API xapi array_insert_range(array * const restrict ar, size_t index, size_t len, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  xproxy(list_add, (void*)ar, index, len, 0, el);
}

void array_set(array * const restrict ar, size_t index, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  list_put((void*)ar, index, 1, 0, el);
}

void array_set_range(array * const restrict ar, size_t index, size_t len, ARRAY_ELEMENT_TYPE ** const restrict el)
{
  list_put((void*)ar, index, len, 0, el);
}

API void array_sort(array * const restrict ar, int (*compar)(const ARRAY_ELEMENT_TYPE *, const ARRAY_ELEMENT_TYPE *, void *), void * arg)
{
  list_sort((void*)ar, compar, arg);
}

API ARRAY_ELEMENT_TYPE * array_search(array * const restrict ar, void * ud, int (*compar)(void *, const ARRAY_ELEMENT_TYPE *, size_t))
{
  return list_search((void*)ar, ud, compar);
}
