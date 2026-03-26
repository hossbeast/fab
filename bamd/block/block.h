/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "valyria/llist.h"

struct bacon_parser;
struct vertices;

typedef struct block {
  llist lln;

  /* child blocks */
  llist blocks;            // struct block

  /* conditional expressions */
  llist exprs;             // struct bacon_expr

  /* uses / requires statements */
  llist imports;           // struct import

  /* rules */
  llist rules;             // struct rule
} block;

void block_alloc(block ** restrict blockp)
  __attribute__((nonnull));

void block_release_bulk(struct bacon_parser * restrict parser, llist * restrict list)
  __attribute__((nonnull));

void block_cleanup(void);

void blocks_gather_rules(struct vertices * restrict list, llist * restrict blocks)
  __attribute__((nonnull));
