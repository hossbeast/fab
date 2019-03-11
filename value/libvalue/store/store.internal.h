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

struct array;   // valyria/array
struct list;    // valyria/list
struct value_parser;
struct value;

typedef struct value_store {
  struct list * values;
  struct list * strings;
  struct list * lists;
  struct list * sets;
} value_store;

/// store_create
//
// SUMMARY
//  create a value_store
//
xapi value_store_create(value_store ** const restrict store)
  __attribute__((nonnull));

/// store_xfree
//
// SUMMARY
//  free a value_store
//
xapi value_store_xfree(value_store * const restrict store);

/// store_ixfree
//
// SUMMARY
//  free a value_store, zero its reference
//
xapi value_store_ixfree(value_store ** const store)
  __attribute__((nonnull));

/// store_recyle
//
//  
//
xapi value_store_recycle(value_store * const restrict store)
  __attribute__((nonnull));

/// store_string
//
//
//
xapi store_string(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

/// store_float
//
//
//
xapi store_float(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

/// store_boolean
//
//
//
xapi store_boolean(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

/// store_posint
//
//
//
xapi store_posint(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

/// store_negint
//
//
//
xapi store_negint(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

/// store_variable
//
//
//
xapi store_variable(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

/// store_mapping
//
//
//
xapi store_mapping(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

/// store_list
//
//
//
xapi store_list(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

/// store_set
//
//
//
xapi store_set(struct value_parser * const restrict parser, struct value ** rv)
  __attribute__((nonnull));

#endif
