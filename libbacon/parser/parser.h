/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"
#include "yyutil/parser.h"
#include "valyria/llist.h"

#include "value.h"

struct bacon_value;
struct bacon_expr;
struct yyu_location;

typedef struct bacon_parser {
  yyu_parser value_yyu;
  yyu_parser value_set_yyu;
  yyu_parser value_list_yyu;
  yyu_parser expr_yyu;

  /* active */
  yyu_parser *yyu;

  llist value_freelist;        // struct bacon_value freelist
  llist expr_freelist;         // struct bacon_expr freelist
  llist location_freelist;     // struct bacon_location freelist

  struct bacon_value *value;   // (returns) parsed value root
  struct bacon_expr *expr;     // (returns) parsed expr root
} bacon_parser;

void bacon_parser_init(bacon_parser * const restrict p);
void bacon_parser_destroy(bacon_parser * const restrict);

/* create a config parser */
void bacon_parser_alloc(bacon_parser ** const restrict p)
  __attribute__((nonnull));

/* free a config parser with free semantics */
void bacon_parser_free(bacon_parser * const restrict);

/* free a config parser with ifree semantics */
void bacon_parser_ifree(bacon_parser ** const restrict)
  __attribute__((nonnull));

/*
 * parse bacon
 */
int bacon_parser_parse(
    bacon_parser * restrict parser
  , char * const restrict buf
  , size_t len
  , const char * restrict fname
  , enum bacon_type initial_state
  , struct bacon_value ** restrict root
)
  __attribute__((nonnull(1, 2)));

int bacon_parser_parse_partial(
    bacon_parser * restrict parser
  , char * const restrict buf
  , size_t len
  , const char * restrict fname
  , struct yyu_location * init_loc
  , struct yyu_location * used_loc
  , enum bacon_type initial_state
  , struct bacon_value ** restrict root
)
  __attribute__((nonnull(1, 2)));

/*
 * parse bacon expr
 */
int bacon_parser_expr_parse(
    bacon_parser * restrict parser
  , char * const restrict buf
  , size_t len
  , const char * restrict fname
  , struct bacon_expr ** restrict expr
)
  __attribute__((nonnull(1, 2)));

int bacon_parser_expr_parse_partial(
    bacon_parser * restrict parser
  , char * const restrict buf
  , size_t len
  , const char * restrict fname
  , struct yyu_location * init_loc
  , struct yyu_location * used_loc
  , struct bacon_expr ** restrict expr
)
  __attribute__((nonnull(1, 2)));

void bacon_parser_value_free(bacon_parser * const restrict parser, struct bacon_value * restrict v);
void bacon_parser_expr_free(bacon_parser * const restrict parser, struct bacon_expr * restrict e);
void bacon_parser_location_free(bacon_parser * const restrict parser, struct bacon_location * restrict e);
