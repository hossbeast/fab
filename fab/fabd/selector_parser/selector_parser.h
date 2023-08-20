/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct yyu_location;
struct selector;
typedef struct selector_parser selector_parser;

/// selector_parser_create
//
// SUMMARY
//  create a config parser
//
void selector_parser_create(selector_parser ** const restrict p)
  __attribute__((nonnull));

/// selector_parser_free
//
// SUMMARY
//  free a config parser with free semantics
//
void selector_parser_xfree(selector_parser * const restrict);

/// selector_parser_ifree
//
// SUMMARY
//  free a config parser with iwfree semantics
//
void selector_parser_ixfree(selector_parser ** const restrict)
  __attribute__((nonnull));

/// selector_parse
//
// SUMMARY
//  parse selector text in place
//
// PARAMETERS
//  [parser]  - reusable parser
//  buf       - buffer containing selector text
//  size      - size of buffer
//  [fname]   - descriptive name about this text
//  [used]    - (returns) number of bytes parsed
//  [rv]      - (returns) parsed selector
//
// REMARKS
//  this parser uses YYU_INPLACE, so the buffer must have two terminating null bytes
//
// VARIANTS
//  partial - permit partial
//
int selector_parser_parse_partial(
    selector_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , struct yyu_location * init_loc
  , struct yyu_location * used_loc
  , struct selector ** restrict rv
)
  __attribute__((nonnull(1, 2)));
