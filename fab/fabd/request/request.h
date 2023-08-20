/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "valyria/llist.h"
#include "moria/traverse.h"

struct narrator;
struct attrs32;
struct command;

typedef struct request {
  llist commands;           // struct command
  bool has_last_command;
} request;

#define TRAVERSE_TYPE_OPT 0x3
#define TRAVERSE_TYPE_TABLE                                                                                             \
  TRAVERSE_ATTR_DEF(TRAVERSE_TYPE_UP    , "up"    , TRAVERSE_TYPE_OPT , MORIA_TRAVERSE_UP)    /* traverse upwards */    \
  TRAVERSE_ATTR_DEF(TRAVERSE_TYPE_DOWN  , "down"  , TRAVERSE_TYPE_OPT , MORIA_TRAVERSE_DOWN)  /* traverse downwards */  \

typedef enum traverse_type {
#define TRAVERSE_ATTR_DEF(x, s, r, y) x = y,
TRAVERSE_TYPE_TABLE
#undef TRAVERSE_ATTR_DEF
} traverse_type;

extern struct attrs32 * traverse_type_attrs;

void request_init(request * restrict req)
  __attribute__((nonnull));

void request_destroy(request * restrict req)
  __attribute__((nonnull));

void request_say(request * restrict req, struct narrator * restrict N)
  __attribute__((nonnull));

void request_command_alloc(request * restrict req, struct command ** cmdp)
  __attribute__((nonnull));

void request_cleanup(void);
