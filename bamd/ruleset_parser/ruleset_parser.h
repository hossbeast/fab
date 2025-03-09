/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#include "yyutil/parser.h"
#include "valyria/llist.h"

#include "graph.h"
#include "block.h"
#include "rule.h"
#include "edges.h"
#include "vertices.h"

struct pattern;
struct pattern_parser;
struct bacon_parser;
struct block;
struct ruleset;

typedef struct ruleset_parser {
  yyu_parser yyu;
  llist lln;        // freelist

  //struct module *mod;
  //struct ruleset *rules;
  const struct ruleset *container;

  // state
  struct moria_graph *g;
  struct vertices lookup_hits;
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
  struct lookup_pattern *anchor;
  bool varsets_set;
  llist varsets_names;    // struct module_special_name
  bool rulesets_set;
  llist rulesets_names;   // struct module_special_name
  bool managed;

  const char *error;
  uint16_t error_len;
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

void ruleset_parser_varsets_append(ruleset_parser * restrict, const char *s, uint16_t len)
  __attribute__((nonnull));

void ruleset_parser_rulesets_append(ruleset_parser * restrict, const char *s, uint16_t len)
  __attribute__((nonnull));

void ruleset_special_name_list_free(llist * restrict list)
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
