/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

size_t bech32_encode(
    char * restrict dst
  , size_t dst_size
  , const char * restrict hrp
  , uint16_t hrp_len
  , const void * restrict data
  , uint16_t len
)
  __attribute__((nonnull));

int bech32_decode(
    void * restrict data
  , size_t data_size
  , size_t * restrict data_lenp
  , char * restrict hrp
  , size_t hrp_size
  , size_t * restrict hrp_lenp
  , const char * restrict input
  , size_t input_len
)
  __attribute__((nonnull));
