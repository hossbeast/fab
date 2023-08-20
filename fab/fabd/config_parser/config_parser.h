/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct yyu_location;
struct configblob;
struct config_parser;
typedef struct config_parser config_parser;

/// config_parser_create
//
// SUMMARY
//  create a config parser
//
void config_parser_create(config_parser ** const restrict p)
  __attribute__((nonnull));

/// config_parser_xfree
//
// SUMMARY
//  free a config parser with free semantics
//
void config_parser_xfree(config_parser * restrict);

/// config_parser_ixfree
//
// SUMMARY
//  free a config parser with iwfree semantics
//
void config_parser_ixfree(config_parser ** restrict)
  __attribute__((nonnull));

/// config_parse
//
// SUMMARY
//  parse config text in place
//
// PARAMETERS
//  parser    - reusable parser
//  buf       - buffer containing config text
//  size      - size of buffer
//  [fname]   - filename, for error messages
//  [used]    - (returns) number of bytes parsed
//  [rv]      - (returns) parsed config object
//
// REMARKS
//  this parser uses YYU_INPLACE, so the buffer must have two terminating null bytes
//
int config_parser_parse(
    config_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , struct yyu_location * restrict init_loc
  , struct configblob ** restrict rv
)
  __attribute__((nonnull(1, 2)));
