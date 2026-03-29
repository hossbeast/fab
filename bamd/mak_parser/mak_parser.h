/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "yyutil/parser.h"

struct yyu_location;
struct bacon_parser;
struct bacon_value;

typedef struct mak_parser {
  yyu_parser yyu;

  // sub parsers
  struct bacon_parser *bacon_parser;

  // (returns)
  struct bacon_value *root; // parsed config
  const char *error;
  uint16_t error_len;
} mak_parser;

/* create a config parser */
mak_parser *mak_parser_create(void);

/* free a config parser with free semantics */
void mak_parser_free(mak_parser * restrict);

/* parse config text in place
 *
 * parser    - reusable parser
 * buf       - buffer containing config text
 * size      - size of buffer
 * [fname]   - filename, for error messages
 * [used]    - (returns) number of bytes parsed
 * [rv]      - config object to parse onto
 *
 * REMARKS
 *  this parser uses YYU_INPLACE, so the buffer must have two terminating null bytes
 */
int mak_parser_parse(
    mak_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , struct bacon_value ** restrict root
)
  __attribute__((nonnull(1, 2)));
