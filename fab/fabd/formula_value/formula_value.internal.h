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

#ifndef FORMULA_VALUE_INTERNAL_H
#define FORMULA_VALUE_INTERNAL_H

#include "types.h"

#include "formula_value.h"

struct yyu_location;
struct selector;
struct rbtree;

/* create formula_value from the parser */

void formula_value_posint_mk(
    const struct yyu_location * restrict loc
  , struct formula_value ** rv
  , uint64_t u
)
  __attribute__((nonnull(1, 2)));

void formula_value_posint_mk(
    const struct yyu_location * restrict loc
  , struct formula_value ** rv
  , uint64_t u
)
  __attribute__((nonnull(1, 2)));

void formula_value_negint_mk(
    const struct yyu_location * restrict loc
  , struct formula_value ** rv
  , int64_t i
)
  __attribute__((nonnull(1, 2)));

void formula_value_float_mk(
    const struct yyu_location * restrict loc
  , struct formula_value ** rv
  , float fp
)
  __attribute__((nonnull(1, 2)));

void formula_value_boolean_mk(
    const struct yyu_location * restrict loc
  , struct formula_value ** rv
  , bool b
)
  __attribute__((nonnull(1, 2)));

void formula_value_string_mk(
    const struct yyu_location * restrict loc
  , struct formula_value ** rv
  , char * restrict s
)
  __attribute__((nonnull));

void formula_value_variable_mk(
    const struct yyu_location * restrict loc
  , struct formula_value ** rv
  , const char * restrict name
  , uint8_t len
)
  __attribute__((nonnull));

void formula_value_sysvar_mk(
    const struct yyu_location * restrict loc
  , struct formula_value ** rv
  , uint16_t token
)
  __attribute__((nonnull));

void formula_value_mapping_mk(
    const struct yyu_location * restrict loc
  , struct formula_value ** rv
  , const char * name
  , uint8_t name_len
  , struct formula_value * restrict value
)
  __attribute__((nonnull));

void formula_value_list_mk(
    const struct yyu_location * restrict loc
  , struct formula_value ** restrict rv
  , struct formula_value * restrict list_head
)
  __attribute__((nonnull));

void formula_value_set_mk(
    const struct yyu_location * restrict loc
  , struct formula_value ** restrict rv
  , struct rbtree * restrict rbt
)
  __attribute__((nonnull));

void formula_value_select_mk(
    const struct yyu_location * restrict loc
  , struct formula_value ** rv
  , struct selector * restrict select
)
  __attribute__((nonnull));

void formula_value_property_mk(
    const struct yyu_location * restrict loc
  , struct formula_value ** rv
  , fsent_property property
)
  __attribute__((nonnull));

void formula_value_prepend_mk(
    const struct yyu_location * restrict loc
  , struct formula_value ** rv
  , struct formula_value * v
)
  __attribute__((nonnull));

void formula_value_path_search_mk(
    const struct yyu_location * restrict loc
  , struct formula_value ** rv
  , struct formula_value * v
)
  __attribute__((nonnull));

void formula_value_sequence_mk(
    const struct yyu_location * restrict loc
  , struct formula_value ** rv
  , struct formula_value * list_head
)
  __attribute__((nonnull));

int fmlval_rbn_cmp(const rbnode * restrict a, const rbnode * restrict b)
  __attribute__((nonnull));

#endif
