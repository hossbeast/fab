/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include <stdint.h>

struct narrator;

void inotify_mask_say(uint32_t mask, struct narrator * const restrict N)
  __attribute__((nonnull));

size_t inotify_mask_cat(char * restrict dst, size_t sz, uint32_t mask)
  __attribute__((nonnull));
