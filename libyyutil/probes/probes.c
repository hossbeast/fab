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

#include <stdio.h>
#include <string.h>

#include "types.h"

#include "common/stresc.h"

#include "probes.h"
#include "parser.h"
#include "scanner.internal.h"

#include "usdt-sem.h"
#include "macros.h"

PROBE_SEM(yyu, token);
PROBE_SEM(yyu, pushstate);
PROBE_SEM(yyu, popstate);

void token_probe(
    yyu_parser * const restrict xtra
  , int token
  , void * const restrict lval
  , yyu_location * const restrict lloc
  , char * restrict text
  , size_t bytes
)
{
  char abuf[256];
  char bbuf[256];
  char buf[256];
  const char *cbuf = 0;
  const char *tokname;
  size_t alen;
  size_t blen;
  size_t clen;

  // token source string
  alen = strescw(abuf, sizeof(abuf), text, bytes);

  // token value
  blen = 0;
  if(xtra->lvalstr)
    blen = xtra->lvalstr(xtra, token, lval, bbuf, sizeof(bbuf));

  // input name
  clen = 0;
  if(xtra->fname)
  {
    clen = strlen(xtra->fname);
    cbuf = xtra->fname;
  }

  tokname = scanner_tokenname(xtra, token);

  snprintf(
      buf
    , sizeof(buf)
    , "%25s ) '%.*s'%s%.*s%s %*s @ %s%.*s%s[%3hu,%3hu - %3hu,%3hu) %d"
    , tokname ?: ""
    , (int)alen, abuf             // escaped string from which the token was scanned
    , blen ? " (" : ""
    , (int)blen, bbuf             // representation of the semantic value for the token
    , blen ? ")" : ""
    , (int)MAX(50 - alen - blen - (blen ? 3 : 0), 0), ""   // padding
    , clen ? "(" : ""             // name of input
    , (int)clen, cbuf
    , clen ? ")" : ""
    , lloc->f_lin                 // location within input
    , lloc->f_col
    , lloc->l_lin
    , lloc->l_col
    , token
  );

  PROBE2(yyu, token, xtra, buf);
}

void popstate_probe(const yyu_parser * restrict parser, int prev, int next)
{
  char abuf[256];
  const char * pstate;
  const char * nstate;
  int alen;
  const char *cbuf;
  size_t clen;
  char buf[256];

  pstate = scanner_statename(parser, prev);
  nstate = scanner_statename(parser, next);

  alen = snprintf(abuf, sizeof(abuf), "%s <- %s"
    , pstate
    , nstate
  );

  // input name
  cbuf = 0;
  clen = 0;
  if(parser->fname)
  {
    clen = strlen(parser->fname);
    cbuf = parser->fname;
  }

  snprintf(buf, sizeof(buf)
    , "(%2d) %.*s %*s @ %s%.*s%s%*s "
    , parser->states_n
    , (int)alen
    , abuf
    , (int)MAX(75 - alen, 0)
    , ""
    , clen ? "(" : ""             // name of input
    , (int)clen, cbuf
    , clen ? ")" : ""
    , 18
    , ""
  );

  PROBE2(yyu, popstate, parser, buf);
}

void pushstate_probe(const yyu_parser * const parser, int prev, int next)
{
  char abuf[256];
  char buf[256];
  const char * pstate;
  const char * nstate;
  int alen;
  const char *cbuf;
  size_t clen;

  pstate = scanner_statename(parser, prev);
  nstate = scanner_statename(parser, next);

  alen = snprintf(abuf, sizeof(abuf), "%s -> %s"
    , pstate
    , nstate
  );

  // input name
  cbuf = 0;
  clen = 0;
  if(parser->fname)
  {
    clen = strlen(parser->fname);
    cbuf = parser->fname;
  }

  snprintf(buf, sizeof(buf)
    , "(%2d) %.*s %*s @ %s%.*s%s%*s "
    , parser->states_n
    , (int)alen
    , abuf
    , (int)MAX(75 - alen, 0)
    , ""
    , clen ? "(" : ""             // name of input
    , (int)clen, cbuf
    , clen ? ")" : ""
    , 18
    , ""
  );

  PROBE2(yyu, pushstate, parser, buf);
}
