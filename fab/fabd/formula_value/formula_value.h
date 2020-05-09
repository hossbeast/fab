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

#ifndef FORMULA_VALUE_H
#define FORMULA_VALUE_H

#include "xapi.h"
#include "types.h"

#include "valyria/chain.h"

#include "formula.h"
#include "node.h"

struct attrs32;
struct build_slot;
struct narrator;
struct value_writer;
struct map;
struct exec_builder;
struct exec_render_context;

#define FORMULA_VALUE_OPT 0xfff
#define FORMULA_VALUE_TABLE                                               \
  /* aggregates */                                                        \
  DEF(FORMULA_VALUE_SET       , "set"       , FORMULA_VALUE_OPT , 0x001)  \
  DEF(FORMULA_VALUE_LIST      , "list"      , FORMULA_VALUE_OPT , 0x002)  \
  DEF(FORMULA_VALUE_MAPPING   , "mapping"   , FORMULA_VALUE_OPT , 0x003)  \
  /* scalars */                                                           \
  DEF(FORMULA_VALUE_STRING    , "string"    , FORMULA_VALUE_OPT , 0x010)  \
  DEF(FORMULA_VALUE_FLOAT     , "float"     , FORMULA_VALUE_OPT , 0x020)  \
  DEF(FORMULA_VALUE_BOOLEAN   , "bool"      , FORMULA_VALUE_OPT , 0x030)  \
  DEF(FORMULA_VALUE_POSINT    , "posint"    , FORMULA_VALUE_OPT , 0x040)  \
  DEF(FORMULA_VALUE_NEGINT    , "negint"    , FORMULA_VALUE_OPT , 0x050)  \
  DEF(FORMULA_VALUE_VARIABLE  , "variable"  , FORMULA_VALUE_OPT , 0x060)  \
  DEF(FORMULA_VALUE_SYSVAR    , "sysvar"    , FORMULA_VALUE_OPT , 0x070)  \
  /* operations */                                                        \
  DEF(FORMULA_VALUE_SELECT    , "select"    , FORMULA_VALUE_OPT , 0x100)  \
  DEF(FORMULA_VALUE_PROPERTY  , "property"  , FORMULA_VALUE_OPT , 0x200)  \
  DEF(FORMULA_VALUE_PREPEND   , "prepend"   , FORMULA_VALUE_OPT , 0x300)  \
  DEF(FORMULA_VALUE_SEQUENCE  , "sequence"  , FORMULA_VALUE_OPT , 0x400)  \

typedef enum formula_value_type {
#define DEF(x, s, r, y) x = UINT32_C(y),
FORMULA_VALUE_TABLE
#undef DEF
} formula_value_type;

struct attrs32 * formula_value_attrs;

#define FORMULA_SYSVAR_OPT 0xf
#define FORMULA_SYSVAR_TABLE                                              \
  DEF(FORMULA_SYSVAR_TARGET   , "bm_target"   , FORMULA_SYSVAR_OPT, 0x1)  \
  DEF(FORMULA_SYSVAR_TARGETS  , "bm_targets"  , FORMULA_SYSVAR_OPT, 0x2)  \
  DEF(FORMULA_SYSVAR_SOURCE   , "bm_source"   , FORMULA_SYSVAR_OPT, 0x3)  \
  DEF(FORMULA_SYSVAR_SOURCES  , "bm_sources"  , FORMULA_SYSVAR_OPT, 0x4)  \
  DEF(FORMULA_SYSVAR_VARIANT  , "bm_variant"  , FORMULA_SYSVAR_OPT, 0x5)  \

typedef enum formula_sysvar {
#define DEF(x, s, r, y) x = UINT32_C(y),
FORMULA_SYSVAR_TABLE
#undef DEF
} formula_sysvar;

struct attrs32 * formula_sysvar_attrs;

struct formula_value;
struct formula_operation;

typedef struct formula_operation {
  formula_value_type type;
  union {
    struct selector * selector;       // select
    node_property property;           // property
    struct formula_value *operand;    // prepend
    struct formula_value *list_head;  // sequence
  };
} formula_operation;

typedef struct formula_value {
  formula_value_type type;

  union {
    chain chn;      /* in list */
    rbnode rbn;     /* in set */
  };

  llist lln;      // for cleanup

  union {
    /* scalars */
    bool b;
    char *s;
    char c;
    uint64_t u;
    int64_t i;
    double f;

    /* variable */
    struct {
      char name[64];
      uint8_t name_len;
    } v;

    /* sysvar */
    enum formula_sysvar sv;

    /* mapping */
    struct {
      char name[64];
      uint8_t name_len;

      struct formula_value *value;
    } m;

    /* aggregates */
    struct formula_value *list_head;
    rbtree *set;

    /* operations */
    formula_operation op;
  };
} formula_value;

void formula_value_free(formula_value * restrict v);

xapi formula_value_say(const formula_value * restrict fv, struct narrator * restrict N)
  __attribute__((nonnull));

xapi formula_value_render(const formula_value * restrict fv, struct narrator * restrict N)
  __attribute__((nonnull));

xapi formula_value_write(const formula_value * restrict fv, struct value_writer * restrict writer)
  __attribute__((nonnull));

xapi exec_render_formula_value(const formula_value * restrict val, struct exec_render_context * restrict ctx)
  __attribute__((nonnull));

xapi exec_render_value(const struct value * restrict val, struct exec_render_context * restrict ctx)
  __attribute__((nonnull));

#endif
