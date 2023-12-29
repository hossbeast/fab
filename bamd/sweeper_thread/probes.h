/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct sweeper_event;

void sweeper_thread_probe(const struct sweeper_event * restrict ev)
  __attribute__((nonnull));
