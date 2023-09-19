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


#include "narrator.h"
#include "xlinux/xstdlib.h"

/* flex and bison do not agree on these names */
#define YYSTYPE REQUEST_YYSTYPE
#define YYLTYPE yyu_location

#include "request_parser.internal.h"
#include "request.tab.h"
#include "request.tokens.h"
#include "request.lex.h"
#include "request.states.h"
#include "selector_parser.internal.h"
#include "config_parser.internal.h"

static YYU_VTABLE(vtable, request, request);

//
// public
//

void request_parser_create(request_parser ** const rv)
{
  request_parser * p = 0;

  xmalloc(&p, sizeof(*p));
  yyu_parser_init(&p->yyu, &vtable);
  yyu_parser_init_tokens(&p->yyu, request_token_table, request_TOKEN_TABLE_SIZE);
  yyu_parser_init_states(
      &p->yyu
    , request_numstates
    , request_statenumbers
    , request_statenames
  );

  selector_parser_create(&p->selector_parser);
  config_parser_create(&p->config_parser);

  *rv = p;
}

void request_parser_xfree(request_parser* const p)
{
  if(p)
  {
    config_parser_xfree(p->config_parser);
    selector_parser_xfree(p->selector_parser);
    yyu_parser_xdestroy(&p->yyu);
  }

  wfree(p);
}

void request_parser_ixfree(request_parser ** const p)
{
  request_parser_xfree(*p);
  *p = 0;
}

int request_parser_parse(
    request_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , request * restrict req
)
{
  /* request must end with two null bytes */
  RUNTIME_ASSERT(size > 2);
  RUNTIME_ASSERT(buf[size - 1] == 0);
  RUNTIME_ASSERT(buf[size - 2] == 0);

  request_init(parser->request = req);

  return yyu_parse(&parser->yyu, buf, size, fname, YYU_INPLACE, 0, 0);
}
