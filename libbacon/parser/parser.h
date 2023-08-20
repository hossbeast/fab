/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "yyutil/parser.h"

#include "value.h"

struct value;       // value.h
struct yyu_location;

typedef struct value_parser {
  yyu_parser value_yyu;
  yyu_parser value_set_yyu;
  yyu_parser value_list_yyu;
  yyu_parser *yyu;

  struct value_store * store; // value storage
  struct value *       root;  // (returns) root of the parsed tree
} value_parser;

/// value_parser_create
//
// SUMMARY
//  create a config parser
//
void value_parser_create(value_parser ** const restrict p)
  __attribute__((nonnull));

/// value_parser_xfree
//
// SUMMARY
//  free a config parser with free semantics
//
void value_parser_xfree(value_parser * const restrict);

/// value_parser_ixfree
//
// SUMMARY
//  free a config parser with iwfree semantics
//
void value_parser_ixfree(value_parser ** const restrict)
  __attribute__((nonnull));

/// value_parser_recycle
//
void value_parser_recycle(value_parser * const restrict)
  __attribute__((nonnull));

/// value_parse
//
// SUMMARY
//  parse the config at the specified path
//
// PARAMETERS
//  [parser]  - (returns) reusable parser
//  text      - config text
//  len       - size of text
//  [tree]    - (returns) root of the parsed config tree
//
// REMARKS
//  not passing stor means that the parsed config tree will have been freed before this function
//  returns, and is therefore only useful to log the parse tree
//
int value_parser_parse(
    value_parser * restrict parser
  , char * const restrict buf
  , size_t len
  , const char * restrict fname
  , enum value_type initial_state
  , struct value ** restrict root
)
  __attribute__((nonnull(1, 2)));

int value_parser_parse_partial(
    value_parser * restrict parser
  , char * const restrict buf
  , size_t len
  , const char * restrict fname
  , struct yyu_location * init_loc
  , struct yyu_location * used_loc
  , enum value_type initial_state
  , struct value ** restrict root
)
  __attribute__((nonnull(1, 2)));
