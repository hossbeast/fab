/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "valyria/llist.h"

struct yyu_location;
struct bacon_parser;

typedef struct bacon_location {
  llist lln;
  const char * fname;
  int f_lin;
  int f_col;
  int l_lin;
  int l_col;

  char * s;
  char * e;
  size_t l;
} bacon_location;

struct bacon_location *bacon_location_alloc(struct bacon_parser * restrict parser)
  __attribute__((nonnull));

void bacon_location_clone(
    struct bacon_location ** const restrict dst
  , const struct yyu_location * const restrict src
  , struct bacon_parser * restrict parser
)
  __attribute__((nonnull));

void bacon_location_free(struct bacon_parser * restrict parser, struct bacon_location * restrict loc)
  __attribute__((nonnull(1)));
