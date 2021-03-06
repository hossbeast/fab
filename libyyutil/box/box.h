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

#ifndef FABD_BOX_H
#define FABD_BOX_H

/*

A box is a heap-allocated scalar

*/

#include "xapi.h"
#include "types.h"
#include "yyutil/grammar.h"

struct attrs16;

#define BOX_TYPE_OPT 0xf
#define BOX_TYPE_TABLE                    \
  BOX_ATTR_DEF(BOX_INT    , BOX_OPT, 0x1) \
  BOX_ATTR_DEF(BOX_INT8   , BOX_OPT, 0x2) \
  BOX_ATTR_DEF(BOX_INT16  , BOX_OPT, 0x3) \
  BOX_ATTR_DEF(BOX_INT32  , BOX_OPT, 0x4) \
  BOX_ATTR_DEF(BOX_INT64  , BOX_OPT, 0x5) \
  BOX_ATTR_DEF(BOX_UINT8  , BOX_OPT, 0x6) \
  BOX_ATTR_DEF(BOX_UINT16 , BOX_OPT, 0x7) \
  BOX_ATTR_DEF(BOX_UINT32 , BOX_OPT, 0x8) \
  BOX_ATTR_DEF(BOX_UINT64 , BOX_OPT, 0x9) \
  BOX_ATTR_DEF(BOX_BOOL   , BOX_OPT, 0xa) \
  BOX_ATTR_DEF(BOX_STRING , BOX_OPT, 0xb) \

typedef enum box_type {
#define BOX_ATTR_DEF(x, r, y) x = UINT16_C(y),
BOX_TYPE_TABLE
#undef BOX_ATTR_DEF
} box_type;

extern struct attrs16 * box_type_attrs;

typedef struct box {
  box_type type;
  yyu_location loc;
  uint32_t hash;
} box;

typedef struct box_int {
  box bx;
  struct {
    int v;
  };
} box_int;

typedef struct box_int8 {
  box bx;
  struct {
    int8_t v;
  };
} box_int8;

typedef struct box_int16 {
  box bx;
  struct {
    int16_t v;
  };
} box_int16;

typedef struct box_int32 {
  box bx;
  struct {
    int32_t v;
  };
} box_int32;

typedef struct box_int64 {
  box bx;
  struct {
    int64_t v;
  };
} box_int64;

typedef struct box_uint8 {
  box bx;
  struct {
    uint8_t v;
  };
} box_uint8;

typedef struct box_uint16 {
  box bx;
  struct {
    uint16_t v;
  };
} box_uint16;

typedef struct box_uint32 {
  box bx;
  struct {
    uint32_t v;
  };
} box_uint32;

typedef struct box_uint64 {
  box bx;
  struct {
    uint64_t v;
  };
} box_uint64;

typedef struct box_bool {
  box bx;
  struct {
    bool v;
  };
} box_bool;

typedef struct box_string {
  box bx;
  struct {
    uint16_t l;
    char * v;
  };
} box_string;

/// box_int_mk
//
// SUMMARY
//  create a BOX_INT boxed value
//
// PARAMETERS
//  store - store
//  [loc] - location for the value
//  rv    - (returns) value containing the int
//  v     - integer value
//
xapi box_int8_mk(
    const struct yyu_location * const restrict loc
  , box_int8 ** rv
  , int8_t v
)
  __attribute__((nonnull(2)));

xapi box_int16_mk(
    const struct yyu_location * const restrict loc
  , box_int16 ** rv
  , int16_t v
)
  __attribute__((nonnull(2)));

xapi box_int32_mk(
    const struct yyu_location * const restrict loc
  , box_int32 ** rv
  , int32_t v
)
  __attribute__((nonnull(2)));

xapi box_int64_mk(
    const struct yyu_location * const restrict loc
  , box_int64 ** rv
  , int64_t v
)
  __attribute__((nonnull(2)));

void box_int8_setif(box_int8 * restrict b, int8_t * v)
  __attribute__((nonnull(2)));

void box_int16_setif(box_int16 * restrict b, int16_t * v)
  __attribute__((nonnull(2)));

void box_int32_setif(box_int32 * restrict b, int32_t * v)
  __attribute__((nonnull(2)));

void box_int64_setif(box_int64 * restrict b, int64_t * v)
  __attribute__((nonnull(2)));

/// box_uint_mk
//
// SUMMARY
//  create a BOX_UINT boxed value
//
// PARAMETERS
//  store - store
//  [loc] - location for the value
//  rv    - (returns) value containing the uint
//  v     - integer value
//
xapi box_uint8_mk(
    const struct yyu_location * const restrict loc
  , box_uint8 ** rv
  , uint8_t v
)
  __attribute__((nonnull(2)));

xapi box_uint16_mk(
    const struct yyu_location * const restrict loc
  , box_uint16 ** rv
  , uint16_t v
)
  __attribute__((nonnull(2)));

xapi box_uint32_mk(
    const struct yyu_location * const restrict loc
  , box_uint32 ** rv
  , uint32_t v
)
  __attribute__((nonnull(2)));

xapi box_uint64_mk(
    const struct yyu_location * const restrict loc
  , box_uint64 ** rv
  , uint64_t v
)
  __attribute__((nonnull(2)));

void box_uint8_setif(box_uint8 * restrict b, uint8_t * v)
  __attribute__((nonnull(2)));

void box_uint16_setif(box_uint16 * restrict b, uint16_t * v)
  __attribute__((nonnull(2)));

void box_uint32_setif(box_uint32 * restrict b, uint32_t * v)
  __attribute__((nonnull(2)));

void box_uint64_setif(box_uint64 * restrict b, uint64_t * v)
  __attribute__((nonnull(2)));

/// box_int_mk
//
// SUMMARY
//  create a BOX_INT boxed value
//
// PARAMETERS
//  store - store
//  [loc] - location for the value
//  rv    - (returns) value containing the int
//  v     - intean value
//
xapi box_int_mk(
    const struct yyu_location * const restrict loc
  , box_int ** rv
  , int v
)
  __attribute__((nonnull(2)));

void box_int_setif(box_int * restrict b, int * v)
  __attribute__((nonnull(2)));

/// box_bool_mk
//
// SUMMARY
//  create a BOX_BOOL boxed value
//
// PARAMETERS
//  store - store
//  [loc] - location for the value
//  rv    - (returns) value containing the bool
//  v     - boolean value
//
xapi box_bool_mk(
    const struct yyu_location * const restrict loc
  , box_bool ** rv
  , bool v
)
  __attribute__((nonnull(2)));

void box_bool_setif(box_bool * restrict b, bool * v)
  __attribute__((nonnull(2)));

/// box_string_mk
//
// SUMMARY
//  create a BOX_STRING boxed value
//
// PARAMETERS
//  store - store
//  [loc] - location for the value
//  rv    - (returns) value containing the string
//  v     - string value
//
xapi box_string_mk(
    const struct yyu_location * const restrict loc
  , box_string ** rv
  , char * restrict v
  , uint16_t len
)
  __attribute__((nonnull(2)));

void box_string_setif(box_string * restrict b, char ** v)
  __attribute__((nonnull(2)));

void box_copyif(box ** restrict dst, box * restrict src)
  __attribute__((nonnull(1)));

void box_free(box * bx);

int box_int_cmp(const box_int * a, const box_int *b);
int box_bool_cmp(const box_bool * a, const box_bool *b);
int box_string_cmp(const box_string * a, const box_string *b);
int box_int8_cmp(const box_int8 * a, const box_int8 *b);
int box_int16_cmp(const box_int16 * a, const box_int16 *b);
int box_int32_cmp(const box_int32 * a, const box_int32 *b);
int box_int64_cmp(const box_int64 * a, const box_int64 *b);
int box_uint8_cmp(const box_uint8 * a, const box_uint8 *b);
int box_uint16_cmp(const box_uint16 * a, const box_uint16 *b);
int box_uint32_cmp(const box_uint32 * a, const box_uint32 *b);
int box_uint64_cmp(const box_uint64 * a, const box_uint64 *b);

#endif
