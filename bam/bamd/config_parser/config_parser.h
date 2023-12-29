/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "yyutil/parser.h"

struct yyu_location;
struct configuration;
struct pattern_parser;
struct bacon_parser;
struct bacon_value;
struct pattern;

typedef struct config_parser {
  yyu_parser yyu;

  // sub parsers
  struct pattern_parser *pattern_parser;
  struct bacon_parser *bacon_parser;

  // under construction
  struct configuration_filesystems_mapping *fm;

  // (returns)
  struct configuration *cfg; // parsed config
} config_parser;

/* create a config parser */
void config_parser_create(config_parser ** const restrict p)
  __attribute__((nonnull));

/* free a config parser with free semantics */
void config_parser_free(config_parser * restrict);

/* free a config parser with ifree semantics */
void config_parser_ifree(config_parser ** restrict)
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
//  [rv]      - configuration object to parse onto
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
  , struct configuration * restrict cfg
)
  __attribute__((nonnull(1, 2)));

void config_parser_fm_alloc(config_parser * restrict parser, struct bacon_value * restrict key)
  __attribute__((nonnull));

void config_parser_wse_alloc(config_parser * restrict parser, struct bacon_value * restrict string)
  __attribute__((nonnull));

void config_parser_fpde_alloc(config_parser * restrict parser, struct bacon_value * restrict string)
  __attribute__((nonnull));

void config_parser_we_put(config_parser * restrict parser, struct pattern * restrict pat)
  __attribute__((nonnull));

void config_parser_wi_put(config_parser * restrict parser, struct pattern * restrict pat)
  __attribute__((nonnull));
