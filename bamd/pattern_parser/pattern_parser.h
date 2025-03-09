/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct yyu_location;
struct pattern;
struct search_pattern;
struct generate_pattern;
struct attachment_pattern;
struct lookup_pattern;
struct include_pattern;
struct match_pattern;
struct path_pattern;

struct pattern_parser;
typedef struct pattern_parser pattern_parser;

/// pattern_parser_create
//
// SUMMARY
//  create a pattern parser
//
void pattern_parser_create(pattern_parser ** const restrict p)
  __attribute__((nonnull));

/// pattern_parser_free
//
// SUMMARY
//  free a pattern parser with free semantics
//
void pattern_parser_free(pattern_parser * restrict);

/// pattern_parser_ifree
//
// SUMMARY
//  free a pattern parser with ifree semantics
//
void pattern_parser_ifree(pattern_parser ** restrict)
  __attribute__((nonnull));

/*
 * parse pattern text in place
 *
 * parser     - reusable parser
 * buf        - buffer containing pattern text
 * size       - size of buffer
 * [fname]    - filename, for error messages
 * [init_loc] - starting location
 * [used_loc] - location delta from init_loc
 * [rv]       - (returns) parsed pattern object
 */
int search_pattern_parse_partial(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , struct yyu_location * restrict init_loc
  , struct yyu_location * restrict used_loc
  , struct search_pattern ** restrict rv
)
  __attribute__((nonnull(1, 2)));

int generate_pattern_parse_partial(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , struct yyu_location * restrict init_loc
  , struct yyu_location * restrict used_loc
  , struct generate_pattern ** restrict rv
)
  __attribute__((nonnull(1, 2)));

int attachment_pattern_parse_partial(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , struct yyu_location * restrict init_loc
  , struct yyu_location * restrict used_loc
  , struct attachment_pattern ** restrict rv
)
  __attribute__((nonnull(1, 2)));

int lookup_pattern_parse_partial(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , struct yyu_location * restrict init_loc
  , struct yyu_location * restrict used_loc
  , struct lookup_pattern ** restrict rv
)
  __attribute__((nonnull(1, 2)));

int include_pattern_parse_partial(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , struct yyu_location * restrict init_loc
  , struct yyu_location * restrict used_loc
  , struct include_pattern ** restrict rv
)
  __attribute__((nonnull(1, 2)));

int match_pattern_parse_partial(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , struct yyu_location * restrict init_loc
  , struct yyu_location * restrict used_loc
  , struct match_pattern ** restrict rv
)
  __attribute__((nonnull(1, 2)));

int lookup_pattern_parse(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , struct lookup_pattern ** restrict rv
)
  __attribute__((nonnull(1, 2)));

int path_pattern_parse_partial(
    pattern_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , struct yyu_location * restrict init_loc
  , struct yyu_location * restrict used_loc
  , struct path_pattern ** restrict rv
)
  __attribute__((nonnull(1, 2)));
