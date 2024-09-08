/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

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
#include "selector_parser.h"

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

  *rv = p;
}

void request_parser_free(request_parser* const p)
{
  if(!p) {
    return;
  }

  request_command_free(p->command);
  selector_parser_free(p->selector_parser);
  yyu_parser_xdestroy(&p->yyu);

  free(p);
}

void request_parser_ifree(request_parser ** const p)
{
  request_parser_free(*p);
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
  RUNTIME_ASSERT(size >= 2);
  RUNTIME_ASSERT(buf[size - 1] == 0);
  RUNTIME_ASSERT(buf[size - 2] == 0);

  request_init(parser->request = req);

  return yyu_parse(&parser->yyu, buf, size, fname, YYU_INPLACE, 0, 0);
}
