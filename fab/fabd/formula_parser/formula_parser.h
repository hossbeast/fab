/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct formula;

typedef struct formula_parser formula_parser;

void formula_parser_create(formula_parser ** const restrict p)
  __attribute__((nonnull));

/* free a formula parser with free semantics */
void formula_parser_xfree(formula_parser * restrict);

/* free a formula parser with iwfree semantics */
void formula_parser_ixfree(formula_parser ** restrict)
  __attribute__((nonnull));

int formula_parser_parse(
    formula_parser * restrict parser
  , char * const restrict buf
  , size_t len
  , const char * restrict fname
  , struct formula * restrict fml
)
  __attribute__((nonnull(1, 2, 5)));
