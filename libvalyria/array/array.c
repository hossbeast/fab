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
  xproxy(list_allocate, (void*)ar, LIST_PRIMARY, esz, 0, 0, 0);
}

API xapi array_createx(
    array ** const restrict ar
  , size_t esz
  , void * free_element
  , void * xfree_element
  , size_t capacity
)
{
  xproxy(list_allocate, (void*)ar, LIST_PRIMARY, esz, free_element, xfree_element, capacity);
}

API xapi array_xfree(array * const restrict ar)
{
  xproxy(list_xfree, (void*)ar);
}

API xapi array_ixfree(array ** const restrict ar)
{
  xproxy(list_ixfree, (void*)ar);
}

API size_t array_size(const array * const restrict ar)
{
  return list_size((void*)ar);
}

API xapi array_recycle(array * const restrict ar)
{
  xproxy(list_recycle, (void*)ar);
}

API void * array_get(const array * const restrict ar, int x)
{
  return list_get((void*)ar, x);
}

API xapi array_shift(array * const restrict ar, void ** const restrict el)
{
  xproxy(list_shift, (void*)ar, el);
}

API xapi array_pop(array * const restrict ar, void ** const restrict el)
{
  xproxy(list_pop, (void*)ar, el);
}

API xapi array_push(array * const restrict ar, void * el)
{
  xproxy(list_add, (void*)ar, ar->l, 1, 0, el);
}

API xapi array_push_range(array * const restrict ar, size_t len, void * el)
{
  xproxy(list_add, (void*)ar, ar->l, len, 0, el);
}

API xapi array_unshift(array * const restrict ar, void * el)
{
  xproxy(list_add, (void*)ar, 0, 1, 0, el);
}

API xapi array_unshift_range(array * const restrict ar, size_t len, void * el)
{
  xproxy(list_add, (void*)ar, 0, len, 0, el);
}

API xapi array_insert(array * const restrict ar, size_t index, void * el)
{
  xproxy(list_add, (void*)ar, index, 1, 0, el);
}

API xapi array_insert_range(array * const restrict ar, size_t index, size_t len, void * el)
{
  xproxy(list_add, (void*)ar, index, len, 0, el);
}

xapi array_set(array * const restrict ar, size_t index, void * el)
{
  xproxy(list_put, (void*)ar, index, 1, 0, el);
}

xapi array_set_range(array * const restrict ar, size_t index, size_t len, void * el)
{
  xproxy(list_put, (void*)ar, index, len, 0, el);
}

API void array_sort(array * const restrict ar, int (*compar)(const void *, const void *, void *), void * arg)
{
  list_sort((void*)ar, compar, arg);
}

API void * array_search(array * const restrict ar, void * ud, int (*compar)(void *, const void *, size_t))
{
  return list_search((void*)ar, ud, compar);
}

API xapi array_truncate(array * const restrict ar, size_t len)
{
  xproxy(list_truncate, (void*)ar, len);
}

API xapi array_delete(array * const restrict ar, size_t index)
{
  xproxy(list_delete, (void*)ar, index);
}

API xapi array_delete_range(array * const restrict ar, size_t index, size_t len)
{
  xproxy(list_delete_range, (void*)ar, index, len);
}
