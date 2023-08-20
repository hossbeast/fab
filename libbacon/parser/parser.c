/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"

#include "narrator.h"

#include "valyria/list.h"
#include "valyria/pstring.h"
#include "valyria/hashtable.h"

#include "value.h"

#include "value.h"
#include "store.internal.h"

/* flex and bison do not agree on these names */
#define YYSTYPE struct value_yystype
#define YYLTYPE yyu_location

/* the token enums shadow each other */
#define VALUE_SET_YYTOKENTYPE
#define VALUE_LIST_YYTOKENTYPE

#include "parser.internal.h"

#include "value.lex.h"
#include "value.states.h"
#include "value.tab.h"
#include "value.tokens.h"
#include "value_set.tab.h"
#include "value_set.tokens.h"
#include "value_list.tab.h"
#include "value_list.tokens.h"

#include "common/attrs.h"

static YYU_VTABLE(value_vtable, value, value);
static YYU_VTABLE(value_set_vtable, value, value_set);
static YYU_VTABLE(value_list_vtable, value, value_list);

//
// public
//

void API value_parser_create(value_parser ** const rp)
{
  value_parser *p = 0;

  xmalloc(&p, sizeof(*p));

  value_store_create(&p->store);

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

  *rp = p;
}

void API value_parser_xfree(value_parser* const p)
{
  if(p)
  {
    value_store_xfree(p->store);
    yyu_parser_xdestroy(&p->value_yyu);
    yyu_parser_xdestroy(&p->value_set_yyu);
    yyu_parser_xdestroy(&p->value_list_yyu);
  }

  wfree(p);
}

void API value_parser_ixfree(value_parser ** const p)
{
  value_parser_xfree(*p);
  *p = 0;
}

void API value_parser_recycle(value_parser * restrict p)
{
  value_store_recycle(p->store);
}

int value_parser_parse(
    value_parser * restrict parser
  , char * const restrict text
  , size_t len
  , const char * restrict fname
  , value_type initial_state
  , value ** restrict root
)
{
  int r;

  parser->root = 0;

  switch(initial_state) {
    case VALUE_TYPE_LIST:
      parser->yyu = &parser->value_list_yyu;
      break;
    case VALUE_TYPE_SET:
      parser->yyu = &parser->value_set_yyu;
      break;
    default:
      parser->yyu = &parser->value_yyu;
  }

  r = yyu_parse(parser->yyu, text, len, fname, 0, 0, 0);

  // ownership transfer
  if(root) {
    *root = parser->root;
  }

  return r;
}

int API value_parser_parse_partial(
    value_parser * restrict parser
  , char * const restrict text
  , size_t len
  , const char * restrict fname
  , yyu_location * init_loc
  , yyu_location * used_loc
  , value_type initial_state
  , value ** restrict root
)
{
  int r;

  parser->root = 0;

  if(initial_state == 0)
    parser->yyu = &parser->value_yyu;
  else if(initial_state == VALUE_TYPE_LIST)
    parser->yyu = &parser->value_list_yyu;
  else if(initial_state == VALUE_TYPE_SET)
    parser->yyu = &parser->value_set_yyu;

  r = yyu_parse(parser->yyu, text, len, fname, YYU_PARTIAL | YYU_INPLACE, init_loc, used_loc);

  // ownership transfer
  if(root)
    *root = parser->root;

  return r;
}
