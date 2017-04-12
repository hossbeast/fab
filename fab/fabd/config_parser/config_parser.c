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

#include "valyria/map.h"
#include "valyria/list.h"
#include "valyria/pstring.h"

#include "value.h"
#include "value/store.h"

struct config_xtra;
#define YYU_EXTRA_TYPE struct config_xtra
#include "internal.h"
#include "logging.h"
#include "config_parser.internal.h"
#include "config_parser/config.tab.h"
#include "config_parser/config.lex.h"
#include "config_parser/config.tokens.h"
#include "config_parser/config.states.h"
#include "errtab/CONFIG.errtab.h"

#define restrict __restrict

struct config_parser
{
  void * p;
};

//
// static
//

static const char * tokenname(int token)
{
  return config_tokennames[token];
}

static const char * statename(int state)
{
  return state >= 0 ? config_statenames[state] : "";
}

static void lvalstr(int token, void * restrict lval, struct config_xtra * restrict xtra, char ** restrict buf, size_t * restrict bufl)
{
}

//
// protected
//

//
// public
//

xapi config_parser_create(config_parser ** const parser)
{
  enter;

  fatal(xmalloc, parser, sizeof(**parser));
  tfatalize(perrtab_KERNEL, ENOMEM, config_yylex_init, &(*parser)->p);

  finally : coda;
}

xapi config_parser_xfree(config_parser* const p)
{
  enter;

  if(p)
  {
    config_yylex_destroy(p->p);
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
    config_parser ** restrict parser
  , value_store ** restrict stor
  , const char * const restrict text
  , size_t len
  , const char * restrict fname
  , value ** restrict root
)
{
  enter;

  config_parser * lp = 0;
  value_store * lvs = 0;
  void * state = 0;

  // parser
  if(!parser)
    parser = &lp;
  if(!*parser)
    fatal(config_parser_create, parser);

  // storage
  if(!stor)
    stor = &lvs;
  if(!*stor)
    fatal(value_store_create, stor);

  config_xtra pp = {
      .tokname      = tokenname
    , .statename    = statename
    , .lvalstr      = lvalstr
    , .fname        = fname
#if DEBUG || DEVEL || XUNIT
    , .state_logs   = L_CONFIG
    , .token_logs   = L_CONFIG
#endif
  };

  // create state specific to this parse
  if((state = config_yy_scan_bytes(text, len, (*parser)->p)) == 0)
    fail(KERNEL_ENOMEM);

  pp.scanner = (*parser)->p;
  pp.stor = *stor;

  // make available to the lexer
  config_yyset_extra(&pp, (*parser)->p);

  // invoke the appropriate parser, raise errors as needed
  fatal(yyu_reduce, config_yyparse, &pp, CONFIG_SYNTAX);

  if(pp.root)
  {
    if(root)
      *root = pp.root;

    // ownership transfer
    pp.root = 0;
  }

finally:
  // cleanup state for this parse
  config_yy_delete_buffer(state, (*parser)->p);
  yyu_extra_destroy(&pp.yyu);

  fatal(config_parser_xfree, lp);
  fatal(value_store_xfree, lvs);
coda;
}
