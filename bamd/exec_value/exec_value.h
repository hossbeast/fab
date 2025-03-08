/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

exec_value - values to be provided to a formula when executed, as specified in the bacon block in
the formula text

special-purpose bacon language

*/

#include "types.h"
#include "valyria/llist.h"
#include "fsent.h"
#include "yyutil/grammar.h"
#include "bacon/value.h"

struct attrs32;
struct exec_value;
struct narrator;
struct rbtree;
struct bacon_writer;
struct bacon_value;
struct bacon_parser;
struct lookup_context;
struct lookup_pattern;

#define EXEC_VALUE_OPT 0xfff
#define EXEC_VALUE_TABLE                                                \
  /* operations */                                                      \
  DEF(EXEC_VALUE_SELECT      , "select"      , EXEC_VALUE_OPT , 0x100)  \
  DEF(EXEC_VALUE_PROPERTY    , "property"    , EXEC_VALUE_OPT , 0x200)  \
  DEF(EXEC_VALUE_PREPEND     , "prepend"     , EXEC_VALUE_OPT , 0x300)  \
  DEF(EXEC_VALUE_SEQUENCE    , "sequence"    , EXEC_VALUE_OPT , 0x400)  \
  DEF(EXEC_VALUE_LOOKUP      , "lookup"      , EXEC_VALUE_OPT , 0x500)  \

typedef enum exec_value_type {
#undef DEF
#define DEF(x, s, r, y) x = UINT32_C(y),
EXEC_VALUE_TABLE
} exec_value_type;

extern struct attrs32 exec_value_attrs;

typedef struct exec_value {
  bacon_value bacon;

  struct selector *select;          // select
  fsent_property property;          // property
  struct bacon_value *prepend;      // prepend
  llist sequence;                   // sequence

  struct {                          // lookup
    struct lookup_pattern *pattern;
    char namespace[128];
    uint16_t namespace_len;
  } lookup;
} exec_value;

void exec_value_setup(void);
void exec_value_cleanup(void);

void exec_value_free(exec_value * restrict v);
void exec_value_ifree(exec_value ** restrict v);

void exec_value_render(const exec_value * restrict fv, struct narrator * restrict N)
  __attribute__((nonnull));

struct bacon_value * exec_value_sysvar_mk(
    struct bacon_parser * restrict parser
  , const struct yyu_location * restrict loc
  , struct bacon_value ** rv
  , uint16_t token
)
  __attribute__((nonnull));

struct bacon_value * exec_value_select_mk(
    struct bacon_parser * restrict parser
  , const struct yyu_location * restrict loc
  , struct bacon_value ** rv
  , struct selector * restrict select
)
  __attribute__((nonnull));

struct bacon_value * exec_value_property_mk(
    struct bacon_parser * restrict parser
  , const struct yyu_location * restrict loc
  , struct bacon_value ** rv
  , fsent_property property
)
  __attribute__((nonnull));

struct bacon_value * exec_value_prepend_mk(
    struct bacon_parser * restrict parser
  , const struct yyu_location * restrict loc
  , struct bacon_value ** rv
  , struct bacon_value * v
)
  __attribute__((nonnull));

struct bacon_value * exec_value_lookup_mk(
    struct bacon_parser * restrict parser
  , const struct yyu_location * restrict loc
  , struct bacon_value ** rv
  , struct lookup_pattern * restrict pattern
  , const char * restrict namespace
  , uint16_t namespace_len
)
  __attribute__((nonnull(1, 2, 3)));

struct bacon_value *exec_value_sequence_mkv(
    struct bacon_parser * restrict parser
  , const struct yyu_location * restrict loc
  , struct bacon_value * e
  , struct bacon_value ** rv
  , struct bacon_value * el
)
  __attribute__((nonnull(1, 2)));

struct bacon_value *exec_value_select_mk(
    struct bacon_parser * restrict parser
  , const yyu_location * restrict loc
  , struct bacon_value ** rv
  , struct selector * restrict select
)
  __attribute__((nonnull(1, 2, 3)));
