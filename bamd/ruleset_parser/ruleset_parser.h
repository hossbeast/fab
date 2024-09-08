/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "yyutil/parser.h"
#include "valyria/llist.h"

#include "graph.h"
#include "block.h"
#include "rule.h"
#include "entlist.h"

struct pattern;
struct pattern_parser;
struct bacon_parser;
struct block;
struct anchor;

typedef struct rulesset_parser {
  yyu_parser yyu;
  llist lln;        // freelist

  // state
  struct moria_graph *g;
  struct module *mod;
  struct ruleset *rules;
  entlist lookup_hits;
  const char *relpath;
  uint16_t relpath_len;

  // sub parsers
  struct bacon_parser *bacon_parser;
  struct pattern_parser *pattern_parser;

  // under construction
  llist blocks;
  rule_bacon bacon;

  // (returns)
  struct block *block;  // top of the stack
  struct anchor *anchor;
  llist varsets;        // struct pattern
  llist rulesets;       // struct pattern
} ruleset_parser;

/* create a rules parser */
void ruleset_parser_alloc(ruleset_parser ** const restrict p)
  __attribute__((nonnull));

/// rules_parser_free
//
// SUMMARY
//  free a rules parser with free semantics
//
void ruleset_parser_free(ruleset_parser * restrict);

/// rules_parser_ifree
//
// SUMMARY
//  free a rules parser with ifree semantics
//
void ruleset_parser_ifree(ruleset_parser ** restrict)
  __attribute__((nonnull));

/// module_parse
//
// SUMMARY
//  parse rules text in place
//
// PARAMETERS
//  parser       - reusable parser
//  buf          - buffer containing rules text
//  size         - size of buffer
//  relpath      - project-relative path to the file being parsed
//
int ruleset_parser_parse(
    ruleset_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict relpath
  , uint16_t relpath_len
)
  __attribute__((nonnull));
