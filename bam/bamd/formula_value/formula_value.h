/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

formula_value - values to be provided to a formula when executed, as specified in the bacon block in
the formula text

*/

#include "types.h"

#include "valyria/llist.h"

#include "formula.h"
#include "fsent.h"

struct attrs32;
struct formula_value;
struct narrator;
struct rbtree;
struct bacon_writer;

#define FORMULA_VALUE_OPT 0xfff
#define FORMULA_VALUE_TABLE                                                   \
  /* aggregates */                                                            \
  DEF(FORMULA_VALUE_SET         , "set"         , FORMULA_VALUE_OPT , 0x001)  \
  DEF(FORMULA_VALUE_LIST        , "list"        , FORMULA_VALUE_OPT , 0x002)  \
  DEF(FORMULA_VALUE_MAPPING     , "mapping"     , FORMULA_VALUE_OPT , 0x003)  \
  /* scalars */                                                               \
  DEF(FORMULA_VALUE_STRING      , "string"      , FORMULA_VALUE_OPT , 0x010)  \
  DEF(FORMULA_VALUE_FLOAT       , "float"       , FORMULA_VALUE_OPT , 0x020)  \
  DEF(FORMULA_VALUE_BOOLEAN     , "bool"        , FORMULA_VALUE_OPT , 0x030)  \
  DEF(FORMULA_VALUE_POSINT      , "posint"      , FORMULA_VALUE_OPT , 0x040)  \
  DEF(FORMULA_VALUE_NEGINT      , "negint"      , FORMULA_VALUE_OPT , 0x050)  \
  DEF(FORMULA_VALUE_VARIABLE    , "variable"    , FORMULA_VALUE_OPT , 0x060)  \
  /* operations */                                                            \
  DEF(FORMULA_VALUE_SELECT      , "select"      , FORMULA_VALUE_OPT , 0x100)  \
  DEF(FORMULA_VALUE_PROPERTY    , "property"    , FORMULA_VALUE_OPT , 0x200)  \
  DEF(FORMULA_VALUE_PREPEND     , "prepend"     , FORMULA_VALUE_OPT , 0x300)  \
  DEF(FORMULA_VALUE_SEQUENCE    , "sequence"    , FORMULA_VALUE_OPT , 0x400)  \
  DEF(FORMULA_VALUE_PATH_SEARCH , "path-search" , FORMULA_VALUE_OPT , 0x500)  \

typedef enum formula_value_type {
#undef DEF
#define DEF(x, s, r, y) x = UINT32_C(y),
FORMULA_VALUE_TABLE
} formula_value_type;

extern struct attrs32 * formula_value_attrs;

typedef struct formula_operation {
  formula_value_type type;
  union {
    struct selector *selector;        // select
    struct formula_value *operand;    // prepend, path-search
    llist list;                       // sequence
    fsent_property property;          // property
  };
} formula_operation;

typedef struct formula_value {
  formula_value_type type;

  rbnode rbn;     /* in set */
  llist lln;      /* in list */

  union {
    /* scalars */
    bool b;
    char c;
    uint64_t u;
    int64_t i;
    double f;
    struct {
      char *s;      // string
      size_t len; // length
      size_t sa;  // size
    };

    /* variable */
    struct {
      char name[64];
      uint8_t name_len;
    } v;

    /* mapping */
    struct {
      char name[64];
      uint8_t name_len;

      struct formula_value *value;
    } m;

    /* aggregates */
    llist list;
    rbtree set;

    /* operations */
    formula_operation op;
  };
} formula_value;

void formula_value_free(formula_value * restrict v);
void formula_value_ifree(formula_value ** restrict v);

void formula_value_render(const formula_value * restrict fv, struct narrator * restrict N)
  __attribute__((nonnull));

/* tracing */
void formula_value_say(const formula_value * restrict fv, struct narrator * restrict N)
  __attribute__((nonnull));
