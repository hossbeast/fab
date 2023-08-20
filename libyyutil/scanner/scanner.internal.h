/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "scanner.h"

struct yyu_parser;

const char * scanner_tokenname(const struct yyu_parser * restrict parser, int token)
  __attribute__((nonnull));

const char * scanner_statename(const struct yyu_parser * restrict parser, int state)
  __attribute__((nonnull));

int token_table_bytoken_cmp_items(const void *, const void *)
  __attribute__((nonnull));
