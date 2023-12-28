/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "bam/metadata.h"

struct channel;
struct bamipc_message;

extern enum bam_system_state system_state;
extern bool system_error;

void system_state_change(enum bam_system_state state);

struct bamipc_message *system_error_event(struct channel * restrict * restrict first)
  __attribute__((nonnull));
