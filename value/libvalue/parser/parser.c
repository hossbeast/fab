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

#include "internal.h"
#include "parser.internal.h"
#include "VALUE.errtab.h"
#include "logging.internal.h"
#include "value.tab.h"
#include "value.tokens.h"
#include "value.lex.h"
#include "value.states.h"

struct value_parser
{
  void * p;
};

//
// static
//

static const char * tokenname(int token)
{
  return value_tokennames[token];
}

static const char * statename(int state)
{
  return state >= 0 ? value_statenames[state] : "";
}

//
// protected
//

//
// public
//

API xapi value_parser_create(value_parser ** const parser)
{
  enter;

  fatal(xmalloc, parser, sizeof(**parser));
  tfatalize(perrtab_KERNEL, ENOMEM, value_yylex_init, &(*parser)->p);

  finally : coda;
}

API xapi value_parser_xfree(value_parser* const p)
{
  enter;

  if(p)
  {
    value_yylex_destroy(p->p);
  }

  wfree(p);

  finally : coda;
}

API xapi value_parser_ixfree(value_parser ** const p)
{
  enter;

  fatal(value_parser_xfree, *p);
  *p = 0;

  finally : coda;
}

API xapi value_parser_parse(
    value_parser ** restrict parser
  , value_store ** restrict stor
  , const char * const restrict text
  , size_t len
  , const char * restrict fname
  , value ** restrict root
#if DEBUG || DEVEL || XUNIT
  , uint64_t logs
#endif
)
{
  enter;

  value_parser * lp = 0;
  value_store * lvs = 0;
  void * state = 0;

  // parser
  if(!parser)
    parser = &lp;
  if(!*parser)
    fatal(value_parser_create, parser);

  // storage
  if(!stor)
    stor = &lvs;
  if(!*stor)
    fatal(value_store_create, stor);

  value_xtra pp = {
      .tokname      = tokenname
    , .statename    = statename
    , .fname        = fname
#if DEBUG || DEVEL || XUNIT
    , .state_logs   = logs | L_VALUE
    , .token_logs   = logs | L_VALUE
#endif
  };

  // create state specific to this parse
  if((state = value_yy_scan_bytes(text, len, (*parser)->p)) == 0)
    fail(KERNEL_ENOMEM);

  pp.scanner = (*parser)->p;
  pp.stor = *stor;

  // make available to the lexer
  value_yyset_extra(&pp, (*parser)->p);

  // invoke the appropriate parser, raise errors as needed
  fatal(yyu_reduce, value_yyparse, &pp, VALUE_SYNTAX);

  if(pp.root)
  {
    if(root)
      *root = pp.root;

    // ownership transfer
    pp.root = 0;
  }

finally:
  // cleanup state for this parse
  value_yy_delete_buffer(state, (*parser)->p);
  yyu_extra_destroy(&pp.yyu);

  fatal(value_parser_xfree, lp);
  fatal(value_store_xfree, lvs);
coda;
}
