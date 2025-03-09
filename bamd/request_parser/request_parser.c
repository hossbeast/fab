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
  llist_init(&p->commands);

  *rv = p;
}

void request_parser_free(request_parser* const p)
{
  if(!p) {
    return;
  }

  RUNTIME_ASSERT(p->command == 0);
  RUNTIME_ASSERT(llist_empty(&p->commands));

  selector_parser_free(p->selector_parser);
  yyu_parser_destroy(&p->yyu);

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
  int r;

  /* request must end with two null bytes */
  RUNTIME_ASSERT(size >= 2);
  RUNTIME_ASSERT(buf[size - 1] == 0);
  RUNTIME_ASSERT(buf[size - 2] == 0);

  r = yyu_parse(&parser->yyu, buf, size, fname, YYU_INPLACE, 0, 0);

  command_free(parser->command);
  parser->command = 0;

  /* error aggregation */
  if(r == 0) {
    parser->error = 0;
    parser->error_len = 0;
  } if(parser->selector_parser->error) {
    parser->error = parser->selector_parser->error;
    parser->error_len = parser->selector_parser->error_len;
  } else {
    parser->error = parser->yyu.error_str;
    parser->error_len = parser->yyu.error_len;
  }

  if(r == 0) {
    llist_splice_head(&req->commands, &parser->commands);
  } else {
    command_free_list(&parser->commands);
  }

  return r;
}
