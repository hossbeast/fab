/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "valyria/llist.h"

struct narrator;
struct pattern;
struct module;
struct rule_run_context;

#define IMPORT_TYPE_OPT 0xff00
#define IMPORT_TYPE_TABLE                               \
  DEF(IMPORT_TYPE_USES      , "uses"      , 0x0100)     \
  DEF(IMPORT_TYPE_REQUIRES  , "requires"  , 0x0300)     \

typedef enum import_type {
#undef DEF
#define DEF(x, s, y) x = y,
IMPORT_TYPE_TABLE
} import_type;

#define IMPORT_FLAGS_NONAME 1

typedef struct import {
  llist owner;
  llist lln;    /* adhoc */

  union {
    import_type type;
    uint32_t attrs;
  };

  /* use cc */
  /* use fab/test as fabtest */
  /* require xunit */
  struct pattern *lookup;
  char *name;
  uint16_t name_len;
} import;

void import_uses_mk(
    import ** restrict importp
  , struct pattern * restrict lookup
  , char * restrict name
  , uint16_t name_len
  , uint32_t attrs
)
  __attribute__((nonnull(1)));

void import_requires_mk(
    import ** restrict importp
  , struct pattern * restrict lookup
  , char * restrict name
  , uint16_t name_len
  , uint32_t attrs
)
  __attribute__((nonnull(1, 2)));

void import_dispose(import * restrict)
  __attribute__((nonnull));

void import_release(import * restrict)
  __attribute__((nonnull));

void import_say(const import * restrict r, struct narrator * restrict N)
  __attribute__((nonnull));

void import_resolve_targets(import * restrict import, struct module * restrict mod, struct rule_run_context * restrict ctx)
  __attribute__((nonnull));

struct fsent *import_resolve_attachment(import * restrict import, struct module * restrict mod, struct rule_run_context * restrict ctx)
  __attribute__((nonnull));
