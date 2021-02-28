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
struct value_parser;
struct value;

typedef struct value_store {
  struct list * values;
  struct list * strings;
  struct list * lists;
  struct list * sets;
} value_store;

/* create a value_store */
xapi value_store_create(value_store ** const restrict store)
  __attribute__((nonnull));

/* free a value_store */
xapi value_store_xfree(value_store * const restrict store);

/* free a value_store, zero its reference */
xapi value_store_ixfree(value_store ** const store)
  __attribute__((nonnull));

xapi value_store_recycle(value_store * const restrict store)
  __attribute__((nonnull));

xapi store_string(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

xapi store_float(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

xapi store_boolean(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

xapi store_posint(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

xapi store_negint(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

xapi store_variable(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

xapi store_mapping(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

xapi store_list(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

xapi store_set(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

#endif
