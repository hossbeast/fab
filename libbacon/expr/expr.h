/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "valyria/llist.h"

struct bacon_parser;
struct bacon_location;

#define BACON_OPERATION_OPT 0xFF
#define BACON_OPERATION_TABLE             \
  /* logical operations */                \
  DEF(BACON_LOGAND      , 0x01) /* && */  \
  DEF(BACON_LOGOR       , 0x02) /* || */  \
  DEF(BACON_LT          , 0x03) /* <  */  \
  DEF(BACON_LE          , 0x04) /* <= */  \
  DEF(BACON_GT          , 0x05) /* >  */  \
  DEF(BACON_GE          , 0x06) /* >= */  \
  DEF(BACON_EQ          , 0x07) /* == */  \
  DEF(BACON_NE          , 0x08) /* != */  \
  /* arithmetic operations */             \
  DEF(BACON_DIV         , 0x0c) /* /  */  \
  DEF(BACON_MOD         , 0x0d) /* %  */  \
  DEF(BACON_MUL         , 0x0e) /* *  */  \
  DEF(BACON_ADD         , 0x0f) /* +  */  \
  DEF(BACON_SUB         , 0x10) /* -  */  \
  DEF(BACON_SHL         , 0x11) /* << */  \
  DEF(BACON_SHR         , 0x12) /* >> */  \
  /* bitwise operations */                \
  DEF(BACON_BITOR       , 0x13) /* |  */  \
  DEF(BACON_BITAND      , 0x14) /* &  */  \
  DEF(BACON_BITXOR      , 0x15) /* ^  */  \
  /* unary operations */                  \
  DEF(BACON_BITINV      , 0x0a) /* ~  */  \
  DEF(BACON_LOGINV      , 0x0b) /* !  */  \
  /* wraps a bacon_value */                     \
  DEF(BACON_VALUE       , 0x16)           \

typedef enum bacon_operation {
#undef DEF
#define DEF(x, y) x = y,
BACON_OPERATION_TABLE
#undef DEF
} bacon_operation;

extern struct attrs32 bacon_operation_attrs;

typedef struct bacon_expr {
  struct bacon_location *loc;
  bacon_operation op;
  llist lln;                  // on freelist

  union {
    struct bacon_value *val;
    struct bacon_expr *oper;
    struct bacon_expr *loper;
  };
  struct bacon_expr *roper;
} bacon_expr;

bacon_expr *bacon_expr_alloc(struct bacon_parser * restrict parser)
  __attribute__((nonnull));

void bacon_expr_free(struct bacon_parser * restrict parser, bacon_expr * restrict e);
