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
#include "value/parser.h"
#include "common/attrs.h"

#include "narrator.h"

/* flex and bison do not agree on these names */
#define YYSTYPE CONFIG_YYSTYPE
#define YYLTYPE yyu_location

#include "config_parser.internal.h"
#include "errtab/CONFIG.errtab.h"
#include "logging.h"
#include "config.tab.h"
#include "config.tokens.h"
#include "config.lex.h"
#include "config.states.h"

#include "pattern_parser.h"

static YYU_VTABLE(vtable, config, config);

//
// public
//

xapi config_parser_create(config_parser ** rv)
{
  enter;

  config_parser * p = 0;

  fatal(xmalloc, &p, sizeof(*p));

  fatal(yyu_parser_init, &p->yyu, &vtable, CONFIG_SYNTAX);
  fatal(yyu_parser_init_tokens, &p->yyu, config_token_table, config_TOKEN_TABLE_SIZE);
  fatal(yyu_parser_init_states
    , &p->yyu
    , config_numstates
    , config_statenumbers
    , config_statenames
  );

  fatal(pattern_parser_create, &p->pattern_parser);

#if DEBUG || DEVEL || XUNIT
  p->yyu.logs = L_CONFIG;
#endif

  *rv = p;
  p = 0;

finally:
  fatal(config_parser_xfree, p);
coda;
}

xapi config_parser_xfree(config_parser* const p)
{
  enter;

  if(p)
  {
    fatal(pattern_parser_xfree, p->pattern_parser);

    wfree(p->fse);
    fatal(yyu_parser_xdestroy, &p->yyu);
  }

  wfree(p);

  finally : coda;
}

xapi config_parser_ixfree(config_parser ** const p)
{
  enter;

  fatal(config_parser_xfree, *p);
  *p = 0;

  finally : coda;
}

xapi config_parser_parse(
    config_parser * restrict parser
  , char * const restrict buf
  , size_t size
  , const char * restrict fname
  , yyu_location * restrict init_loc
  , configblob ** restrict rv
)
{
  enter;

  fatal(yyu_parse, &parser->yyu, buf, size, fname, YYU_INPLACE, init_loc, 0);
  if(rv)
  {
    *rv = parser->cfg;
    parser->cfg = 0;
  }

finally:
  fatal(config_ixfree, &parser->cfg);
  xapi_infos("fname", fname);
coda;
}
