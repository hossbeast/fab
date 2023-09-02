/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "valyria/llist.h"

struct bacon_parser;
struct entlist;

typedef struct block {
  llist lln;

  /* child blocks */
  llist blocks;            // struct block

  /* conditional expressions */
  llist exprs;             // struct bacon_expr

  /* uses / requires statements */
  llist imports;           // struct import

  /* search rules */
  llist rules;             // struct rule
} block;

void block_alloc(block ** restrict blockp)
  __attribute__((nonnull));

void block_init(block * restrict blockp)
  __attribute__((nonnull));

void block_release_bulk(struct bacon_parser * restrict parser, llist * restrict list)
  __attribute__((nonnull));

void block_release(struct bacon_parser * restrict parser, block * restrict block)
  __attribute__((nonnull));

void block_dispose_bulk(struct bacon_parser * restrict parser, llist * restrict list)
  __attribute__((nonnull));

void block_dump(block * restrict b)
  __attribute__((nonnull));

void block_dump_bulk(llist * restrict list)
  __attribute__((nonnull));

void block_cleanup(void);

void blocks_dump(llist * restrict list)
  __attribute__((nonnull));

void block_gather_rule_edges(struct entlist * restrict list, block * restrict block)
  __attribute__((nonnull));
