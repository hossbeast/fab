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

#ifndef _VALUE_STORE_INTERNAL_H
#define _VALUE_STORE_INTERNAL_H

#include "store.h"

struct list;    // valyria/list

typedef struct value_store {
  struct list * values;
  struct list * lists;
  struct list * strings;
} value_store;

/// store_string
//
//
//
xapi store_string(value_store * const restrict stor, value ** rv)
  __attribute__((nonnull));

/// store_float
//
//
//
xapi store_float(value_store * const restrict stor, value ** rv)
  __attribute__((nonnull));

/// store_boolean
//
//
//
xapi store_boolean(value_store * const restrict stor, value ** rv)
  __attribute__((nonnull));

/// store_integer
//
//
//
xapi store_integer(value_store * const restrict stor, value ** rv)
  __attribute__((nonnull));

/// store_map
//
//
//
xapi store_map(value_store * const restrict stor, value ** rv)
  __attribute__((nonnull));

/// store_list
//
//
//
xapi store_list(value_store * const restrict stor, value ** rv)
  __attribute__((nonnull));

#endif
