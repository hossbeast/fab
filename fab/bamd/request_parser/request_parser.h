/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "yyutil/parser.h"

struct request;
struct selector_parser;
struct config_parser;
struct command;

typedef struct request_parser {
  yyu_parser yyu;

  // sub-parsers
  struct selector_parser * selector_parser;
  struct config_parser * config_parser;

  // under construction
  struct command * command;

  // (returns)
  struct request * request;
} request_parser;

/// request_parser_create
//
// SUMMARY
//  create a config parser
//
void request_parser_create(request_parser ** const restrict p)
  __attribute__((nonnull));

/// request_parser_free
//
// SUMMARY
//  free a config parser with free semantics
//
void request_parser_free(request_parser * restrict);

/// request_parser_ifree
//
// SUMMARY
//  free a config parser with ifree semantics
//
void request_parser_ifree(request_parser ** restrict)
  __attribute__((nonnull));

/*
 * parse a request
 *
 *  parser    - reusable parser
 *  text      - config text
 *  len       - size of text
 *  req       - request to return the result in
 */
int request_parser_parse(
    request_parser * restrict parser
  , char * const restrict buf
  , size_t len
  , const char * restrict fname
  , struct request * restrict req
)
  __attribute__((nonnull(1, 2)));
