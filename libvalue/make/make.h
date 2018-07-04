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

#ifndef _VALUE_MAKE_H
#define _VALUE_MAKE_H

#include <sys/types.h>

#include "xapi.h"

struct list;
struct narrator;
struct pstring;
struct value;
struct value_store;
struct yyu_location;

/// value_integer_mk
//
// SUMMARY
//  create a VALUE_TYPE_FLOAT value
//
// PARAMETERS
//  stor  - storage for the value
//  [loc] - location for the value
//  rv    - (returns) value containing the float
//  i     - integer value
//
xapi value_integer_mk(
    struct value_store * const restrict stor
  , const struct yyu_location * const restrict loc
  , struct value ** rv
  , int64_t i
)
  __attribute__((nonnull(1, 3)));

/// value_float_mk
//
// SUMMARY
//  create a VALUE_TYPE_FLOAT value
//
// PARAMETERS
//  stor  - storage for the value
//  [loc] - location for the value
//  rv    - (returns) value containing the float
//  fp    - float value
//
xapi value_float_mk(
    struct value_store * const restrict stor
  , const struct yyu_location * const restrict loc
  , struct value ** rv
  , const float fp
)
  __attribute__((nonnull(1, 3)));

/// value_boolean_mk
//
// SUMMARY
//  create a VALUE_TYPE_BOOLEAN value
//
// PARAMETERS
//  stor  - storage for the value
//  [loc] - location for the value
//  rv    - (returns) value containing the boolean
//  b     - boolean value
//
xapi value_boolean_mk(
    struct value_store * const restrict stor
  , const struct yyu_location * const restrict loc
  , struct value ** rv
  , const int b
)
  __attribute__((nonnull(1, 3)));

/// value_string_mk
//
// SUMMARY
//  create or update a VALUE_TYPE_STRING value
//
// PARAMETERS
//  stor   - value storage
//  [loc]  - value location
//  [e]    - existing value to append to, if any
//  rv     - (returns) value containing string
//
// VARIANT
//  s - string
//  w - buffer
//  v - value
//  c - character
//
xapi value_string_mks(
    struct value_store * const restrict stor
  , const struct yyu_location * const restrict loc
  , struct value * e
  , struct value ** rv
  , const char * const restrict s
)
  __attribute__((nonnull(1, 4, 5)));

xapi value_string_mkw(
    struct value_store * const restrict stor
  , const struct yyu_location * const restrict loc
  , struct value * e
  , struct value ** rv
  , const char * const restrict b
  , size_t l
)
  __attribute__((nonnull(1, 4, 5)));

xapi value_string_mkc(
    struct value_store * const restrict stor
  , const struct yyu_location * const restrict loc
  , struct value * e
  , struct value ** rv
  , const uint8_t c
)
  __attribute__((nonnull(1, 4)));

xapi value_string_mkv(
    struct value_store * const restrict stor
  , const struct yyu_location * const restrict loc
  , struct value * e
  , struct value ** rv
  , struct value * v
)
  __attribute__((nonnull(1, 4, 5)));

/// value_map_mk
//
// SUMMARY
//  create or update a VALUE_TYPE_MAP value
//
// PARAMETERS
//  stor    - value storage
//  [loc]   - value location
//  [e]     - existing value to append to, if any
//  rv      - (returns) value containing the map
//  key     - key to insert (VALUE_TYPE_STRING)
//  val     - value to insert
//  [attr]  - attributes to apply to the mapping
//
// REMARKS
//  attr & VALUE_MERGE_* only makes sense when val->type & VALUE_TYPE_AGGREGATE
//
// VARIANTS
//  v - key from value
//  s - key from string
//
xapi value_map_mk(
    struct value_store * const restrict stor
  , const struct yyu_location * const restrict loc
  , struct value * e
  , struct value ** rv
  , uint16_t attr
)
  __attribute__((nonnull(1, 4)));

xapi value_map_mkv(
    struct value_store * const restrict stor
  , const struct yyu_location * const restrict loc
  , struct value * e
  , struct value ** rv
  , struct value * key
  , struct value * val
  , uint16_t attr
)
  __attribute__((nonnull(1, 4, 5, 6)));

xapi value_map_mks(
    struct value_store * const restrict stor
  , const struct yyu_location * const restrict loc
  , struct value * e
  , struct value ** rv
  , const char * const key
  , struct value * val
  , uint16_t attr
)
  __attribute__((nonnull(1, 4, 5, 6)));

/// value_list_mk
//
// SUMMARY
//  create or update a VALUE_TYPE_LIST value
//
// PARAMETERS
//  stor  - value storage
//  [loc] - value location
//  [e]   - existing value to append to, if any
//  rv    - (returns) list value
//  [el]  - value to insert
//
// REMARKS
//  list size is unchanged if el is null
//
xapi value_list_mkv(
    struct value_store * const restrict stor
  , const struct yyu_location * const restrict loc
  , struct value * e
  , struct value ** rv
  , struct value * el
)
  __attribute__((nonnull(1, 4)));

/// value_set_mk
//
// SUMMARY
//  create or update a VALUE_TYPE_SET value
//
// PARAMETERS
//  stor  - value storage
//  [loc] - value location
//  [e]   - existing value to append to, if any
//  rv    - (returns) set value
//  [el]  - value to insert
//
// REMARKS
//  set size is unchanged if el is null
//
xapi value_set_mkv(
    struct value_store * const restrict stor
  , const struct yyu_location * const restrict loc
  , struct value * e
  , struct value ** rv
  , struct value * el
)
  __attribute__((nonnull(1, 4)));

#endif
