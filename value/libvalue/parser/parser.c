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

#include "xapi.h"
#include "xapi/exit.h"

#include "xlinux/xstdlib.h"
#include "xlinux/KERNEL.errtab.h"

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
#include "VALUE.errtab.h"
#include "logging.internal.h"

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

xapi API value_parser_create(value_parser ** const rp)
{
  enter;

  value_parser *p = 0;

  fatal(xmalloc, &p, sizeof(*p));

  fatal(value_store_create, &p->store);

  // value
  fatal(yyu_parser_init, &p->value_yyu, &value_vtable, VALUE_SYNTAX);
  fatal(yyu_parser_init_tokens, &p->value_yyu, value_token_table, value_TOKEN_TABLE_SIZE);
  fatal(yyu_parser_init_states
    , &p->value_yyu
    , value_numstates
    , value_statenumbers
    , value_statenames
  );

  // value-set
  fatal(yyu_parser_init, &p->value_set_yyu, &value_set_vtable, VALUE_SYNTAX);
  fatal(yyu_parser_init_tokens, &p->value_set_yyu, value_set_token_table, value_set_TOKEN_TABLE_SIZE);
  fatal(yyu_parser_init_states
    , &p->value_set_yyu
    , value_numstates
    , value_statenumbers
    , value_statenames
  );

  // value-list
  fatal(yyu_parser_init, &p->value_list_yyu, &value_list_vtable, VALUE_SYNTAX);
  fatal(yyu_parser_init_tokens, &p->value_list_yyu, value_list_token_table, value_list_TOKEN_TABLE_SIZE);
  fatal(yyu_parser_init_states
    , &p->value_list_yyu
    , value_numstates
    , value_statenumbers
    , value_statenames
  );

#if DEBUG || DEVEL || XUNIT
  p->value_yyu.logs = L_VALUE;
  p->value_set_yyu.logs = L_VALUE;
  p->value_list_yyu.logs = L_VALUE;
#endif

  *rp = p;
  p = 0;

finally:
  fatal(value_parser_xfree, p);
coda;
}

xapi API value_parser_xfree(value_parser* const p)
{
  enter;

  if(p)
  {
    fatal(value_store_xfree, p->store);
    fatal(yyu_parser_xdestroy, &p->value_yyu);
    fatal(yyu_parser_xdestroy, &p->value_set_yyu);
    fatal(yyu_parser_xdestroy, &p->value_list_yyu);
  }

  wfree(p);

  finally : coda;
}

xapi API value_parser_ixfree(value_parser ** const p)
{
  enter;

  fatal(value_parser_xfree, *p);
  *p = 0;

  finally : coda;
}

xapi API value_parser_recycle(value_parser * restrict p)
{
  enter;

  fatal(value_store_recycle, p->store);

  finally : coda;
}

xapi API value_parser_parse(
    value_parser * restrict parser
  , char * const restrict text
  , size_t len
  , const char * restrict fname
  , value_type initial_state
  , value ** restrict root
)
{
  enter;

  typeof(parser->value_yyu) *pfn = 0;

  parser->root = 0;

  if(initial_state == 0)
    pfn = &parser->value_yyu;
  else if(initial_state == VALUE_TYPE_LIST)
    pfn = &parser->value_list_yyu;
  else if(initial_state == VALUE_TYPE_SET)
    pfn = &parser->value_set_yyu;

  fatal(yyu_parse, pfn, text, len, fname, 0, 0, 0);

  // ownership transfer
  if(root) {
    *root = parser->root;
  }

finally:
  xapi_infos("fname", fname);
coda;
}

xapi API value_parser_parse_partial(
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
  enter;

  typeof(parser->value_yyu) *pfn = 0;

  parser->root = 0;

  if(initial_state == 0)
    pfn = &parser->value_yyu;
  else if(initial_state == VALUE_TYPE_LIST)
    pfn = &parser->value_list_yyu;
  else if(initial_state == VALUE_TYPE_SET)
    pfn = &parser->value_set_yyu;

  fatal(yyu_parse, pfn, text, len, fname, YYU_PARTIAL | YYU_INPLACE, init_loc, used_loc);

  // ownership transfer
  if(root)
    *root = parser->root;

  finally : coda;
}
