/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"
#include "bacon/parser.h"
#include "bacon/make.h"
#include "bacon/location.h"
#include "common/attrs.h"

#include "narrator.h"

/* flex and bison do not agree on these names */
#define YYSTYPE MAK_YYSTYPE
#define YYLTYPE yyu_location

#include "mak_parser/glue.h"
#include "mak_parser.h"
#include "mak.tab.h"
#include "mak.tokens.h"
#include "mak.lex.h"
#include "mak.states.h"

#include "mak_parser.h"
#include "config.h"
#include "zcat.h"

static YYU_VTABLE(vtable, mak, mak);

//
// public
//

mak_parser *mak_parser_create(void)
{
  mak_parser *p;

  xmalloc(&p, sizeof(*p));

  yyu_parser_init(&p->yyu, &vtable);
  yyu_parser_init_tokens(&p->yyu, mak_token_table, mak_TOKEN_TABLE_SIZE);
  yyu_parser_init_states(
      &p->yyu
    , mak_numstates
    , mak_statenumbers
    , mak_statenames
  );

  bacon_parser_alloc(&p->bacon_parser);

  return p;
}

void mak_parser_free(mak_parser* const p)
{
  if(!p) {
    return;
  }

  bacon_parser_free(p->bacon_parser);

  yyu_parser_destroy(&p->yyu);

  free(p);
}

int mak_parser_parse(
    mak_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , struct bacon_value ** restrict root
)
{
  int r;

  parser->root = 0;
  r = yyu_parse(&parser->yyu, buf, size, fname, 0, 0, 0);

  *root = parser->root;

  /* error aggregation */
  if(r == 0) {
    parser->error = 0;
    parser->error_len = 0;
  } else if(parser->bacon_parser->error) {
    parser->error = parser->bacon_parser->error;
    parser->error_len = parser->bacon_parser->error_len;
  } else {
    parser->error = parser->yyu.error_str;
    parser->error_len = parser->yyu.error_len;
  }

  return r;
}
