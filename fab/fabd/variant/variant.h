/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

typedef struct variant {
  struct variant_tag {
    const char * text;
    uint16_t len;
  } *tags;
  uint16_t tags_len;

  uint16_t norm_len;
  char norm[];        // normalized
} variant;

void variant_setup(void);
void variant_cleanup(void);

void variant_get(const char * restrict name, uint16_t len, variant ** restrict variantp)
  __attribute__((nonnull));

bool variant_key_compatible(
    const variant * restrict var
  , const char * restrict keytext
  , const char ** restrict name
  , uint16_t * restrict name_len
)
  __attribute__((nonnull(2, 3, 4)));
