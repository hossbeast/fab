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

#ifndef _VALUE_H
#define _VALUE_H

#include "xapi.h"
#include "types.h"

struct list;          // libvalyria/list
struct set;           // libvalyria/set
struct yyu_location;
struct value_store;
struct narrator;
struct attrs32;
struct pstring;

#define VALUE_TYPE_SCALAR     0xF0
#define VALUE_TYPE_AGGREGATE  0x0F

#define VALUE_TYPE_OPT 0xFF
#define VALUE_TYPE_TABLE                          \
  /* aggregates */                                \
  /* unordered list of unique values */           \
  VALUE_TYPE_DEF(SET      , VALUE_TYPE_OPT, 0x01) \
  /* ordered list of distinct values */           \
  VALUE_TYPE_DEF(LIST     , VALUE_TYPE_OPT, 0x02) \
  /* association between two values */            \
  VALUE_TYPE_DEF(MAPPING  , VALUE_TYPE_OPT, 0x04) \
  /* scalars */                                   \
  VALUE_TYPE_DEF(STRING   , VALUE_TYPE_OPT, 0x10) \
  VALUE_TYPE_DEF(FLOAT    , VALUE_TYPE_OPT, 0x20) \
  VALUE_TYPE_DEF(BOOLEAN  , VALUE_TYPE_OPT, 0x30) \
  VALUE_TYPE_DEF(POSINT   , VALUE_TYPE_OPT, 0x40) \
  VALUE_TYPE_DEF(NEGINT   , VALUE_TYPE_OPT, 0x50) \
  VALUE_TYPE_DEF(VARIABLE , VALUE_TYPE_OPT, 0x60) \

typedef enum value_type {
#define VALUE_TYPE_DEF(x, r, y) VALUE_TYPE_ ## x = UINT16_C(y),
VALUE_TYPE_TABLE
#undef VALUE_TYPE_DEF

  /* this enum occupies uint16 */
  VALUE_TYPE_MAX = (UINT16_MAX - 1)
} value_type;

extern struct attrs32 * value_type_attrs;

/*
 * merge options
 */
#define VALUE_MERGE_TABLE                                         \
  VALUE_MERGE_DEF(ADD, 0x01, 0x01)  /* (default) add elements */  \
  VALUE_MERGE_DEF(SET, 0x02, 0x01)  /* set elements */            \

typedef enum value_merge_type {
#define VALUE_MERGE_DEF(x, y, r) VALUE_MERGE_ ## x = UINT16_C(y),
VALUE_MERGE_TABLE
#undef VALUE_MERGE_DEF
} value_merge_type;

typedef struct value_location {
  const char * fname;

  int f_lin;
  int f_col;
  int l_lin;
  int l_col;

  char * s;
  char * e;
  size_t l;
} value_location;

typedef struct value {
  value_type type;
  value_location loc;
  uint16_t attr;           // bitwise combo of VALUE_*
  uint32_t hash;

  union {
    struct list * items;    // list
    struct set * els;       // set
    struct {                // mapping
      struct value * key;
      struct value * val;
    };

    struct pstring *s;  // string
    double f;           // float
    bool b;             // bool
    uint64_t u;         // positive integer
    int64_t i;          // negative integer
    struct {            // variable
      char name[64];
      uint16_t len;
    } v;
  };
} value;

/// value_load
//
// SUMMARY
//  initialize the library
//
xapi value_load(void);

/// value_unload
//
// SUMMARY
//  release the library
//
xapi value_unload(void);

/// value_znload
//
//
//
size_t value_znload(void * restrict dst, size_t sz, const value * const restrict val)
  __attribute__((nonnull));

/// value_say
//
// SUMMARY
//  write a (multiline) representation of a value to a narrator, in bacon-conforming format
//
// PARAMETERS
//  val - value
//  N   - narrator to write to
//
xapi value_say(const value * const restrict val, struct narrator * const restrict N)
  __attribute__((nonnull(2)));

/*
 * write a (single-line) representation of a value to a narrator
 */
xapi value_render(const value * const restrict val, struct narrator * const restrict N)
  __attribute__((nonnull));

/// value_cmp
//
// SUMMARY
//  compare two values
//
int value_cmp(value * const restrict A, value * const restrict B);

/// value_equal
//
// SUMMARY
//  determine whether two values are equal
//
bool value_equal(value * const restrict A, value * const restrict B);

/// value_hash
//
// SUMMARY
//  compute a hash over the value
//
uint32_t value_hash(uint32_t h, value * const restrict val)
  __attribute__((nonnull));

/*
 * lookup a value from among the mappings in a set
 */
value * value_lookupw(const value * restrict set, const char * restrict key, uint16_t key_len)
  __attribute__((nonnull));

#endif
