/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "yyutil/parser.h"

struct yyu_location;
struct config;
struct pattern_parser;
struct bacon_parser;
struct bacon_value;
struct pattern;

typedef struct configset_parser {
  yyu_parser yyu;

  // sub parsers
  struct pattern_parser *pattern_parser;
  struct bacon_parser *bacon_parser;

  // under construction
  struct config_fstree_mapping *fsm;
  struct config_plugin_entry *pe;

  // (returns)
  struct config *cfg; // parsed config
} configset_parser;

/* create a config parser */
void configset_parser_create(configset_parser ** const restrict p)
  __attribute__((nonnull));

/* free a config parser with free semantics */
void configset_parser_free(configset_parser * restrict);

/* free a config parser with ifree semantics */
void configset_parser_ifree(configset_parser ** restrict)
  __attribute__((nonnull));

/// configset_parse
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
//  [rv]      - config object to parse onto
//
// REMARKS
//  this parser uses YYU_INPLACE, so the buffer must have two terminating null bytes
//
int configset_parser_parse(
    configset_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , struct yyu_location * restrict init_loc
  , struct config * restrict cfg
)
  __attribute__((nonnull(1, 2)));

void configset_parser_fsm_alloc(configset_parser * restrict parser, struct pattern * restrict key)
  __attribute__((nonnull));

struct config_plugin_entry *configset_parser_plugin_alloc(configset_parser * restrict parser)
  __attribute__((nonnull));

void configset_parser_plugin_put(configset_parser * restrict parser, struct config_plugin_entry * restrict pe);

void configset_parser_wse_alloc(configset_parser * restrict parser, struct bacon_value * restrict string)
  __attribute__((nonnull));

void configset_parser_fpde_alloc(configset_parser * restrict parser, struct bacon_value * restrict string)
  __attribute__((nonnull));

void configset_parser_ppde_alloc(configset_parser * restrict parser, struct bacon_value * restrict string)
  __attribute__((nonnull));

void configset_parser_we_put(configset_parser * restrict parser, struct pattern * restrict pat)
  __attribute__((nonnull));

void configset_parser_plugin_arg_put(configset_parser * restrict parser, struct bacon_value * restrict string)
  __attribute__((nonnull));
