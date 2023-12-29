/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct fsent;

extern unsigned short fsent_deleted_semaphore;

void fsent_deleted_probe(const struct fsent * restrict n)
  __attribute__((nonnull));
