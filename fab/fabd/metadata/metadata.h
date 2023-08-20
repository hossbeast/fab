/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

void metadata_collate(void *dst, size_t sz, size_t * restrict z)
  __attribute__((nonnull));
