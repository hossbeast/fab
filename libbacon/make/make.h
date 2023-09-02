/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "expr.h"

struct bacon_value;
struct bacon_parser;
struct yyu_location;
struct bacon_expr;

/* create a BACON_TYPE_INTEGER value
 *
 * parser - parser
 * [loc]  - location for the value
 * [rv]   - (returns) the value
 * i      - integer
 */
struct bacon_value *bacon_integer_mk(
    struct bacon_parser * const restrict parser
  , const struct yyu_location * const restrict loc
  , struct bacon_value ** rv
  , int128_t i
)
  __attribute__((nonnull(1)));

/* create a BACON_TYPE_FLOAT value
 *
 * parser - parser
 * [loc]  - location for the value
 * [rv]   - (returns) value containing the float
 * fp     - float value
 */
struct bacon_value * bacon_float_mk(
    struct bacon_parser * const restrict parser
  , const struct yyu_location * const restrict loc
  , struct bacon_value ** rv
  , double fp
)
  __attribute__((nonnull(1)));

/* create a BACON_TYPE_BOOLEAN value
 *
 * parser - parser
 * [loc] - location for the value
 * rv    - (returns) value containing the boolean
 * b     - boolean value
 */
struct bacon_value *bacon_boolean_mk(
    struct bacon_parser * const restrict parser
  , const struct yyu_location * const restrict loc
  , struct bacon_value ** rv
  , int b
)
  __attribute__((nonnull(1)));

/// value_string_mk
//
// SUMMARY
//  create or update a BACON_TYPE_STRING value
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
struct bacon_value * bacon_string_mk(
    struct bacon_parser * const restrict parser
  , const struct yyu_location * const restrict loc
  , struct bacon_value * e
  , struct bacon_value ** rv
)
  __attribute__((nonnull(1)));

struct bacon_value * bacon_string_mks(
    struct bacon_parser * const restrict parser
  , const struct yyu_location * const restrict loc
  , struct bacon_value * e
  , struct bacon_value ** rv
  , const char * const restrict s
)
  __attribute__((nonnull(1, 5)));

struct bacon_value * bacon_string_mkw(
    struct bacon_parser * const restrict parser
  , const struct yyu_location * const restrict loc
  , struct bacon_value * e
  , struct bacon_value ** rv
  , const char * const restrict b
  , size_t l
)
  __attribute__((nonnull(1, 5)));

struct bacon_value * bacon_string_mkc(
    struct bacon_parser * const restrict parser
  , const struct yyu_location * const restrict loc
  , struct bacon_value * e
  , struct bacon_value ** rv
  , uint8_t c
)
  __attribute__((nonnull(1)));

struct bacon_value * bacon_string_mkv(
    struct bacon_parser * const restrict parser
  , const struct yyu_location * const restrict loc
  , struct bacon_value * e
  , struct bacon_value ** rv
  , struct bacon_value * v      // freed by this function
)
  __attribute__((nonnull(1, 5)));

struct bacon_value * bacon_string_mkf(
    struct bacon_parser * const restrict parser
  , const struct yyu_location * const restrict loc
  , struct bacon_value * e
  , struct bacon_value ** rv
  , const char * restrict fmt
  , ...
)
  __attribute__((nonnull(1, 5)))
  __attribute__((format(printf, 5, 6)));

/// value_variable_mkw
//
//
//
struct bacon_value * bacon_variable_mkw(
    struct bacon_parser * const restrict parser
  , const struct yyu_location * const restrict loc
  , struct bacon_value ** rv
  , const char * const restrict name
  , uint16_t len
)
  __attribute__((nonnull(1, 4)));

/// value_mapping_mk
//
// SUMMARY
//  create or update a BACON_TYPE_MAPPING value
//
// PARAMETERS
//  stor    - value storage
//  [loc]   - value location
//  rv      - (returns) value containing the mapping
//  key     - key
//  val     - value
//
struct bacon_value * bacon_mapping_mk(
    struct bacon_parser * const restrict parser
  , const struct yyu_location * const restrict loc
  , struct bacon_value ** rv
  , struct bacon_value * k
  , struct bacon_value * v
)
  __attribute__((nonnull(1, 4, 5)));

//  create or update a BACON_TYPE_LIST value
//
// PARAMETERS
//  parser - parser
//  [loc] - value location
//  [e]   - existing value to append to, if any
//  rv    - (returns) list value
//  [el]  - value to insert
//
// REMARKS
//  list size is unchanged if el is null
//
struct bacon_value * bacon_list_mkv(
    struct bacon_parser * const restrict parser
  , const struct yyu_location * const restrict loc
  , struct bacon_value * e
  , struct bacon_value ** rv
  , struct bacon_value * el
)
  __attribute__((nonnull(1)));

/// value_set_mk
//
// SUMMARY
//  create or update a BACON_TYPE_SET value
//
// PARAMETERS
//  parser - parser
//  [loc] - value location
//  [e]   - existing value to append to, if any
//  rv    - (returns) set value
//  [el]  - value to insert
//
// REMARKS
//  set size is unchanged if el is null
//
struct bacon_value * bacon_set_mkv(
    struct bacon_parser * const restrict parser
  , const struct yyu_location * const restrict loc
  , struct bacon_value * e
  , struct bacon_value ** rv
  , struct bacon_value * el
)
  __attribute__((nonnull(1)));

struct bacon_expr *bacon_expr_mk(
    struct bacon_parser * const restrict parser
  , const struct yyu_location * const restrict loc
  , struct bacon_expr **rv
  , bacon_operation op
  , struct bacon_expr *lop
  , struct bacon_expr *rop
)
  __attribute__((nonnull(1)));

struct bacon_expr *bacon_expr_wrap(
    struct bacon_parser * const restrict parser
  , struct bacon_value *v
  , struct bacon_expr **rv
)
  __attribute__((nonnull(1)));
