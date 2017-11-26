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
#include "xlinux/xstdlib.h"
#include "xlinux/KERNEL.errtab.h"

#include "logging.h"
#include "ff_parser.internal.h"
#include "FF.errtab.h"
#include "ff_node.h"
#include "ff.tab.h"
#include "ff.lex.h"
#include "ff.tokens.h"
#include "ff.states.h"

#include "zbuffer.h"

#define FF_CREF_TABLE       \
  FF_CREF(0x0   , '0')      \
  FF_CREF(0x7   , 'a')      \
  FF_CREF(0x8   , 'b')      \
  FF_CREF(0x9   , 't')      \
  FF_CREF(0xa   , 'n')      \
  FF_CREF(0xb   , 'v')      \
  FF_CREF(0xc   , 'f')      \
  FF_CREF(0xd   , 'r')      \
  FF_CREF('\\'  , '\\')     \
  FF_CREF('"'   , '"')      \

const char * ff_cref_char_table = (char[]) {
#define FF_CREF(a, b) [ a ] = b,
FF_CREF_TABLE
#undef FF_CREF

[0xff] = 0
};

const char * ff_cref_byte_table = (char[]) {
#define FF_CREF(a, b) [ b ] = a,
FF_CREF_TABLE
#undef FF_CREF

[0xff] = 0
};

struct ff_parser
{
  void * p;
};

//
// static
//

static const char * tokenname(int token)
{
  return ff_tokennames[token];
}

static const char * statename(int state)
{
  return state >= 0 ? ff_statenames[state] : "";
}

static size_t lvalstr(int token, void * restrict _lval, yyu_extra * restrict extra, char * restrict buf, size_t bufl)
{
  union YYSTYPE * lval = _lval;

  size_t z = 0;

  if(token == ff_CREF)
  {
    if(lval->u8 < 0x20 || lval->u8 > 0x7e)
    {
      z += znloadf(buf + z, bufl - z, "byte 0x%02hhx", lval->u8);
    }
  }

  return z;
}

//
// protected
//

//
// public
//

xapi ff_parser_create(ff_parser ** const parser)
{
  enter;

  fatal(xmalloc, parser, sizeof(**parser));
  tfatalize(perrtab_KERNEL, ENOMEM, ff_yylex_init, &(*parser)->p);

  finally : coda;
}

void ff_parser_free(ff_parser * p)
{
  if(p)
  {
    ff_yylex_destroy(p->p);
  }

  wfree(p);
}

void ff_parser_ifree(ff_parser ** p)
{
  ff_parser_free(*p);
  *p = 0;
}

xapi ff_parser_parse(
    ff_parser ** restrict parser
  , const char * const restrict text
  , size_t len
  , const char * restrict fname
  , ff_node ** restrict root
)
{
  enter;

  ff_parser * lp = 0;
  void * state = 0;

  // parser
  if(!parser)
    parser = &lp;
  if(!*parser)
    fatal(ff_parser_create, parser);

  ff_extra pp = {
      .tokname      = tokenname
    , .statename    = statename
    , .lvalstr      = lvalstr
    , .fname        = fname
#if DEBUG || DEVEL || XUNIT
    , .state_logs   = L_FF
    , .token_logs   = L_FF
#endif
  };

  // create state specific to this parse
  if((state = ff_yy_scan_bytes(text, len, (*parser)->p)) == 0)
    fail(KERNEL_ENOMEM);

  pp.scanner = (*parser)->p;

  // make available to the lexer
  ff_yyset_extra(&pp, (*parser)->p);

  // invoke the appropriate parser, raise errors as needed
  fatal(yyu_reduce, ff_yyparse, &pp, FF_SYNTAX);

  if(pp.root)
  {
    if(root)
      *root = pp.root;

    // ownership transfer
    pp.root = 0;
  }

finally:
  // cleanup state for this parse
  ff_yy_delete_buffer(state, (*parser)->p);
  yyu_extra_destroy(&pp.yyu);

  ff_parser_free(lp);
coda;
}
