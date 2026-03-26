/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

struct build_record;
struct build_slot;

void build_event_start(struct build_record * restrict rec)
  __attribute__((nonnull));

void build_event_end(struct build_record * restrict rec)
  __attribute__((nonnull));

void build_event_ok(void);

void build_event_error(void);

void build_event_slot_ok(struct build_slot * restrict bs);

void build_event_slot_err(struct build_slot * restrict bs)
  __attribute__((nonnull));
