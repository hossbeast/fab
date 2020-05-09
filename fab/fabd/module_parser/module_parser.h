/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.

   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef _FABD_MODULE_PARSER_H
#define _FABD_MODULE_PARSER_H

#include "xapi.h"
#include "types.h"

#include "yyutil/parser.h"
#include "valyria/llist.h"

#include "module.h"
#include "graph.h"

struct module;
struct pattern_parser;
struct set;
struct yyu_location;
struct pattern;
struct graph;
struct value_parser;
struct rule_run_context;
struct selection;

typedef struct module_parser {
  yyu_parser yyu;
  llist lln;        // freelist

  // state
  struct graph *g;
  struct set * variants;
  struct selection * scratch;
  struct graph_invalidation_context *invalidation;
  llist statement_block_freelist;
  module * mod;

  // sub parsers
  struct value_parser * value_parser; // the value parser is owned by the module
  struct pattern_parser * pattern_parser;
  xapi (*use_resolve)(struct module_parser * restrict parser, struct pattern * restrict ref);
  xapi (*require_resolve)(struct module_parser * restrict parser, struct pattern * restrict ref);
  xapi (*import_resolve)(struct module_parser * restrict parser, struct pattern * restrict ref, bool scoped, char * restrict name, uint16_t namel);

  // under construction
  struct statement_block *block;

  // struct module_formula_show_settings show_settings;
  // struct module_filesystem_entry * fse;
  // struct module_logging_section * logging_section;

  // (returns)
  statement_block *unscoped_block;
  llist scoped_blocks;
  struct value * var_value;
  bool var_merge_overwrite;
} module_parser;

/// module_parser_create
//
// SUMMARY
//  create a module parser
//
xapi module_parser_create(module_parser ** const restrict p)
  __attribute__((nonnull));

/// module_parser_xfree
//
// SUMMARY
//  free a module parser with free semantics
//
xapi module_parser_xfree(module_parser * restrict);

/// module_parser_ixfree
//
// SUMMARY
//  free a module parser with iwfree semantics
//
xapi module_parser_ixfree(module_parser ** restrict)
  __attribute__((nonnull));

/// module_parse
//
// SUMMARY
//  parse module text in place
//
// PARAMETERS
//  parser       - reusable parser
//  value_parser - value parser owned by the module
//  buf          - buffer containing module text
//  size         - size of buffer
//  [fname]      - filename, for error messages
//
xapi module_parser_parse(
    module_parser * restrict parser
  , struct module * restrict mod
  , struct graph_invalidation_context * restrict invalidation
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
)
  __attribute__((nonnull));

xapi module_parser_block_alloc(module_parser * restrict parser, statement_block ** restrict blockp)
  __attribute__((nonnull));

#endif
