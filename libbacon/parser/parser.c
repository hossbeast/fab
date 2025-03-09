/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"

#include "narrator.h"

#include "value.h"
#include "location.h"

/* flex and bison do not agree on these names */
#define YYSTYPE struct value_yystype
#define YYLTYPE yyu_location

/* the token enums shadow each other */
#define VALUE_SET_YYTOKENTYPE
#define VALUE_LIST_YYTOKENTYPE
#define EXPR_YYTOKENTYPE

#include "glue.h"

#include "value.lex.h"
#include "value.states.h"
#include "value.tab.h"
#include "value.tokens.h"
#include "value_set.tab.h"
#include "value_set.tokens.h"
#include "value_list.tab.h"
#include "value_list.tokens.h"
#include "expr.tab.h"
#include "expr.tokens.h"

#include "common/attrs.h"

static YYU_VTABLE(value_vtable, value, value);
static YYU_VTABLE(value_set_vtable, value, value_set);
static YYU_VTABLE(value_list_vtable, value, value_list);
static YYU_VTABLE(expr_vtable, value, expr);

//
// public
//

void bacon_parser_init(bacon_parser * p)
{
  llist_init(&p->value_freelist);
  llist_init(&p->expr_freelist);
  llist_init(&p->location_freelist);

  // value
  yyu_parser_init(&p->value_yyu, &value_vtable);
  yyu_parser_init_tokens(&p->value_yyu, value_token_table, value_TOKEN_TABLE_SIZE);
  yyu_parser_init_states(&p->value_yyu
    , value_numstates
    , value_statenumbers
    , value_statenames
  );

  // value-set
  yyu_parser_init(&p->value_set_yyu, &value_set_vtable);
  yyu_parser_init_tokens(&p->value_set_yyu, value_set_token_table, value_set_TOKEN_TABLE_SIZE);
  yyu_parser_init_states(&p->value_set_yyu
    , value_numstates
    , value_statenumbers
    , value_statenames
  );

  // value-list
  yyu_parser_init(&p->value_list_yyu, &value_list_vtable);
  yyu_parser_init_tokens(&p->value_list_yyu, value_list_token_table, value_list_TOKEN_TABLE_SIZE);
  yyu_parser_init_states(&p->value_list_yyu
    , value_numstates
    , value_statenumbers
    , value_statenames
  );

  // expressions
  yyu_parser_init(&p->expr_yyu, &expr_vtable);
  yyu_parser_init_tokens(&p->expr_yyu, expr_token_table, expr_TOKEN_TABLE_SIZE);
  yyu_parser_init_states(&p->expr_yyu
    , value_numstates
    , value_statenumbers
    , value_statenames
  );
}

void bacon_parser_alloc(bacon_parser ** const rp)
{
  bacon_parser *p = 0;

  xmalloc(&p, sizeof(*p));
  bacon_parser_init(p);

  *rp = p;
}

void bacon_parser_destroy(bacon_parser* const parser)
{
  bacon_value *v;
  bacon_expr *e;
  bacon_location *loc;
  llist *T;

  yyu_parser_destroy(&parser->value_yyu);
  yyu_parser_destroy(&parser->value_set_yyu);
  yyu_parser_destroy(&parser->value_list_yyu);
  yyu_parser_destroy(&parser->expr_yyu);

  llist_foreach_safe(&parser->value_freelist, v, lln, T) {
    free(v);
  }

  llist_foreach_safe(&parser->expr_freelist, e, lln, T) {
    free(e);
  }

  llist_foreach_safe(&parser->location_freelist, loc, lln, T) {
    free(loc);
  }
}

void bacon_parser_free(bacon_parser* const parser)
{
  if(parser) {
    bacon_parser_destroy(parser);
  }

  free(parser);
}

void bacon_parser_ifree(bacon_parser ** const parser)
{
  bacon_parser_free(*parser);
  *parser = 0;
}

static void errors_aggregate(bacon_parser * restrict parser, int r)
{
  if(r == 0) {
    parser->error = 0;
    parser->error_len = 0;
  } else {
    parser->error = parser->yyu->error_str;
    parser->error_len = parser->yyu->error_len;
  }
}

int bacon_parser_parse(
    bacon_parser * restrict parser
  , char * const restrict text
  , size_t len
  , const char * restrict fname
  , bacon_type initial_state
  , bacon_value ** restrict root
)
{
  int r;

  parser->value = 0;

  switch(initial_state) {
    case BACON_TYPE_LIST:
      parser->yyu = &parser->value_list_yyu;
      break;
    case BACON_TYPE_SET:
      parser->yyu = &parser->value_set_yyu;
      break;
    default:
      parser->yyu = &parser->value_yyu;
  }

  r = yyu_parse(parser->yyu, text, len, fname, 0, 0, 0);
  errors_aggregate(parser, r);
  if(root) {
    *root = parser->value;
  }
  return r;
}

int bacon_parser_parse_partial(
    bacon_parser * restrict parser
  , char * const restrict text
  , size_t len
  , const char * restrict fname
  , yyu_location * init_loc
  , yyu_location * used_loc
  , bacon_type initial_state
  , bacon_value ** restrict root
)
{
  int r;

  parser->value = 0;

  if(initial_state == 0)
    parser->yyu = &parser->value_yyu;
  else if(initial_state == BACON_TYPE_LIST)
    parser->yyu = &parser->value_list_yyu;
  else if(initial_state == BACON_TYPE_SET)
    parser->yyu = &parser->value_set_yyu;

  r = yyu_parse(parser->yyu, text, len, fname, YYU_PARTIAL | YYU_INPLACE, init_loc, used_loc);
  errors_aggregate(parser, r);
  if(root) {
    *root = parser->value;
  }

  return r;
}

int bacon_parser_expr_parse(
    bacon_parser * restrict parser
  , char * const restrict text
  , size_t len
  , const char * restrict fname
  , bacon_expr ** restrict expr
)
{
  int r;

  parser->expr = 0;
  parser->yyu = &parser->expr_yyu;

  r = yyu_parse(parser->yyu, text, len, fname, 0, 0, 0);
  errors_aggregate(parser, r);
  if(expr) {
    *expr = parser->expr;
  }
  return r;
}

int bacon_parser_expr_parse_partial(
    bacon_parser * restrict parser
  , char * const restrict text
  , size_t len
  , const char * restrict fname
  , yyu_location * init_loc
  , yyu_location * used_loc
  , bacon_expr ** restrict expr
)
{
  int r;

  parser->expr = 0;
  parser->yyu = &parser->expr_yyu;

  r = yyu_parse(parser->yyu, text, len, fname, YYU_PARTIAL | YYU_INPLACE, init_loc, used_loc);
  errors_aggregate(parser, r);
  if(expr) {
    *expr = parser->expr;
  }

  return r;
}

void bacon_parser_value_free(bacon_parser * const restrict parser, struct bacon_value * restrict v)
{
  bacon_value_free(parser, v);
}

void bacon_parser_expr_free(bacon_parser * const restrict parser, struct bacon_expr * restrict e)
{
  bacon_expr_free(parser, e);
}

void bacon_parser_location_free(bacon_parser * const restrict parser, struct bacon_location * restrict loc)
{
  bacon_location_free(parser, loc);
}
