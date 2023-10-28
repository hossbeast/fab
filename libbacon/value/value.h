/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "valyria/rbtree.h"
#include "valyria/llist.h"

struct narrator;
struct bacon_location;
struct bacon_parser;

#define BACON_TYPE_CLASS      0x1
#define BACON_TYPE_SCALAR     0x0
#define BACON_TYPE_AGGREGATE  0x1

#define BACON_TYPE_OPT 0xFF
#define BACON_TYPE_TABLE                  \
  /* aggregates */                        \
  /* unordered list of unique values */   \
  DEF(BACON_TYPE_SET      , 0x11)         \
  /* ordered list of distinct values */   \
  DEF(BACON_TYPE_LIST     , 0x21)         \
  /* association between two values */    \
  DEF(BACON_TYPE_MAPPING  , 0x31)         \
  /* scalars */                           \
  DEF(BACON_TYPE_STRING   , 0x10)         \
  DEF(BACON_TYPE_FLOAT    , 0x20)         \
  DEF(BACON_TYPE_BOOLEAN  , 0x30)         \
  DEF(BACON_TYPE_INTEGER  , 0x40)         \
  DEF(BACON_TYPE_VARIABLE , 0x50)         \

typedef enum bacon_type {
#undef DEF
#define DEF(x, y) x = UINT16_C(y),
BACON_TYPE_TABLE
#undef DEF
} bacon_type;

extern struct attrs32 bacon_value_type_attrs;

typedef struct bacon_value {
  bacon_type type;
  struct bacon_location *loc;

  rbnode rbn;           // in a set
  llist lln;            // in a list
  uint32_t hash;

  union {
    rbtree set;         // set
    llist list;         // list
    struct {            // mapping
      struct bacon_value * key;
      struct bacon_value * val;
    };

    int128_t i;         // signed integer
    bool b;             // bool
    double f;           // float
    struct {
      char *s;          // string
      size_t len;       // length
      size_t sa;        // allocated size
    };
  };
} bacon_value;

/* compare two values */
int bacon_value_cmp(const bacon_value * const restrict A, const bacon_value * const restrict B);

/* determine whether two values are equal */
bool bacon_value_equal(const bacon_value * const restrict A, const bacon_value * const restrict B);

/* write a (multiline) representation of a bacon_value to a narrator */
void bacon_value_say(const bacon_value * const restrict val, struct narrator * const restrict N)
  __attribute__((nonnull(2)));

/* write a (single-line) representation of a bacon_value to a narrator */
void bacon_value_say_flat(const bacon_value * const restrict val, struct narrator * const restrict N)
  __attribute__((nonnull(2)));

void bacon_value_free(struct bacon_parser * restrict parser, struct bacon_value * restrict v);

struct bacon_value *bacon_string_alloc(struct bacon_parser * restrict parser)
  __attribute__((nonnull));

struct bacon_value *bacon_float_alloc(struct bacon_parser * restrict parser)
  __attribute__((nonnull));

struct bacon_value *bacon_boolean_alloc(struct bacon_parser * restrict parser)
  __attribute__((nonnull));

struct bacon_value *bacon_integer_alloc(struct bacon_parser * restrict parser)
  __attribute__((nonnull));

struct bacon_value *bacon_mapping_alloc(struct bacon_parser * restrict parser)
  __attribute__((nonnull));

struct bacon_value *bacon_list_alloc(struct bacon_parser * restrict parser)
  __attribute__((nonnull));

struct bacon_value *bacon_set_alloc(struct bacon_parser * restrict parser)
  __attribute__((nonnull));

struct bacon_value *bacon_variable_alloc(struct bacon_parser * restrict parser)
  __attribute__((nonnull));
