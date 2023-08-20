/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include "xlinux/xstdlib.h"
#include "bacon/parser.h"
#include "common/attrs.h"

#include "narrator.h"

/* flex and bison do not agree on these names */
#define YYSTYPE CONFIG_YYSTYPE
#define YYLTYPE yyu_location

#include "config_parser.internal.h"
#include "config.tab.h"
#include "config.tokens.h"
#include "config.lex.h"
#include "config.states.h"

#include "pattern_parser.h"

static YYU_VTABLE(vtable, config, config);

//
// public
//

void config_parser_create(config_parser ** rv)
{
  config_parser * p = 0;

  xmalloc(&p, sizeof(*p));

  yyu_parser_init(&p->yyu, &vtable);
  yyu_parser_init_tokens(&p->yyu, config_token_table, config_TOKEN_TABLE_SIZE);
  yyu_parser_init_states(
      &p->yyu
    , config_numstates
    , config_statenumbers
    , config_statenames
  );

  pattern_parser_create(&p->pattern_parser);

  *rv = p;
}

void config_parser_xfree(config_parser* const p)
{
  if(p)
  {
    pattern_parser_xfree(p->pattern_parser);

    wfree(p->fse);
    yyu_parser_xdestroy(&p->yyu);
  }

  wfree(p);
}

void config_parser_ixfree(config_parser ** const p)
{
  config_parser_xfree(*p);
  *p = 0;
}

int config_parser_parse(
    config_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict path
  , yyu_location * restrict init_loc
  , configblob ** restrict rv
)
{
  int r;

  r = yyu_parse(&parser->yyu, buf, size, path, YYU_INPLACE, init_loc, 0);
  if(r == 0 && rv)
  {
    *rv = parser->cfg;
    parser->cfg = 0;
  }

  config_ixfree(&parser->cfg);

  return r;
}
