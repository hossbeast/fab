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

#ifndef _LIST_INTERNAL_H
#define _LIST_INTERNAL_H

/*

MODULE
 list.internal

*/

#include "list.h"
#include "array.internal.h"

#include "macros.h"

typedef struct value {
  void * p;
  size_t l;
} value;

typedef struct list_t {
  union {
    array_t ar;
    array arx;
    list lix;
    struct {
      size_t size;
    };
  };

  int (*cmp_fn)(const void * A, size_t Asz, const void * B, size_t Bsz);
  void (*free_fn)(void * item);
  xapi (*xfree_fn)(void * item);
} list_t;

STATIC_ASSERT(offsetof(list, size) == offsetof(list_t, size));
STATIC_ASSERT(offsetof(list_t, size) == offsetof(array_t, size));

#endif
