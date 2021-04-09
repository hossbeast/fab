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

/*

formula_value - values to be provided to a formula when executed, as specified in the bacon block in
the formula text

*/

#include "xapi.h"
#include "types.h"

#include "valyria/chain.h"

#include "formula.h"
#include "fsent.h"

struct attrs32;
struct formula_value;
struct narrator;
struct rbtree;
struct value_writer;

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
  DEF(FORMULA_VALUE_VARIABLE    , "variable"    , FORMULA_VALUE_OPT , 0x060) /* value of variant var by name */  \
  DEF(FORMULA_VALUE_SYSVAR      , "sysvar"      , FORMULA_VALUE_OPT , 0x070) /* implicit sysvar by name */ \
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

#define FORMULA_SYSVAR_OPT 0xf
#define FORMULA_SYSVAR_TABLE                                              \
  DEF(FORMULA_SYSVAR_TARGET   , "bm_target"   , FORMULA_SYSVAR_OPT, 0x1)  \
  DEF(FORMULA_SYSVAR_TARGETS  , "bm_targets"  , FORMULA_SYSVAR_OPT, 0x2)  \
  DEF(FORMULA_SYSVAR_SOURCE   , "bm_source"   , FORMULA_SYSVAR_OPT, 0x3)  \
  DEF(FORMULA_SYSVAR_SOURCES  , "bm_sources"  , FORMULA_SYSVAR_OPT, 0x4)  \
  DEF(FORMULA_SYSVAR_VARIANT  , "bm_variant"  , FORMULA_SYSVAR_OPT, 0x5)  \

typedef enum formula_sysvar {
#undef DEF
#define DEF(x, s, r, y) x = UINT32_C(y),
FORMULA_SYSVAR_TABLE
} formula_sysvar;

extern struct attrs32 * formula_sysvar_attrs;

typedef struct formula_operation {
  formula_value_type type;
  union {
    struct selector * selector;       // select
    fsent_property property;          // property
    struct formula_value *operand;    // prepend, path-search
    struct formula_value *list_head;  // sequence
  };
} formula_operation;

typedef struct formula_value {
  formula_value_type type;

  union {
    chain chn;      /* in list */
    rbnode rbn;     /* in set */
  };

  llist lln;        /* used in cleanup */

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
    struct rbtree *set;

    /* operations */
    formula_operation op;
  };
} formula_value;

void formula_value_free(formula_value * restrict v);
void formula_value_ifree(formula_value ** restrict v);
void formula_value_set_free(struct rbtree * restrict rbt);

xapi formula_value_render(const formula_value * restrict fv, struct narrator * restrict N)
  __attribute__((nonnull));

/* tracing */
xapi formula_value_say(const formula_value * restrict fv, struct narrator * restrict N)
  __attribute__((nonnull));

#endif
