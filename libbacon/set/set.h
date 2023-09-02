/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct rbnode;
struct bacon_value;

int bacon_set_rbn_cmp(const rbnode * restrict A, const struct rbnode * restrict B);
int bacon_set_key_cmp(void * restrict key, const struct rbnode * restrict B);

/* find a mapping value in a set whose key is a string having the specified text, if any */
struct bacon_value * bacon_set_lookupw(const struct bacon_value * restrict set, const char * restrict key, uint16_t key_len)
  __attribute__((nonnull));
