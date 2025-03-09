/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct descriptor_type;
struct bacon_writer;
struct request_handler;
struct command;

typedef union stats_record {
  uint8_t u8[0];
  uint16_t u16[0];
  uint32_t u32[0];
  uint64_t u64[0];
} stats_record;

void stats_build(
    const union stats_record * restrict record
  , const struct descriptor_type * restrict descriptor
  , struct bacon_writer * restrict builder_writer
)
  __attribute__((nonnull));

void stats_handler(struct request_handler * restrict ctx, struct command * restrict cmd, bool reset)
  __attribute__((nonnull));
