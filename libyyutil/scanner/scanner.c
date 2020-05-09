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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "xapi.h"
#include "xlinux/xstdlib.h"
#include "logger.h"
#include "valyria/hashtable.h"
#include "xlinux/KERNEL.errtab.h"

#include "internal.h"
#include "scanner.internal.h"
#include "logging.internal.h"
#include "parser.internal.h"

#include "macros.h"
#include "stresc.h"
#include "attrs.h"
#include "parseint.h"

//
// internal
//

const char * scanner_tokenname(yyu_parser * restrict parser, int token)
{
  if(token < parser->mintoken || token > parser->maxtoken)
    return 0;

  int index = parser->tokenindexes[token - parser->mintoken];
  return parser->tokennames[index];
}

const char * scanner_statename(yyu_parser * restrict parser, int state)
{
  if(state < 0 || state >= parser->numstates)
    return 0;

  return parser->statenames[state];
}

/// ptoken
//
// SUMMARY
//  log an info message about a scanned token
//
// PARAMETERS
//  token - token
//  lval  - yylval
//  lloc  - yylloc
//  xtra  - yyextra
//  text  - yytext (text the token was parsed from)
//  bytes - token extent in bytes
//
static xapi __attribute__((nonnull)) ptoken(
    yyu_parser * const restrict xtra
  , int token
  , void * const restrict lval
  , yyu_location * const restrict lloc
  , char * restrict text
  , size_t bytes
)
{
  enter;

#if DEBUG || DEVEL || XUNIT
  char abuf[256];
  char bbuf[256];
  const char *cbuf;

  if(log_would(xtra->logs | L_YYUTIL | L_TOKENS))
  {
    // token source string
    size_t alen = strescw(abuf, sizeof(abuf), text, bytes);

    // token value
    size_t blen = 0;
    if(xtra->lvalstr)
      blen = xtra->lvalstr(xtra, token, lval, bbuf, sizeof(bbuf));

    // input name
    size_t clen = 0;
    if(xtra->fname)
    {
      clen = strlen(xtra->fname);
      cbuf = xtra->fname;
    }

    const char * tokname = scanner_tokenname(xtra, token);

    logf(xtra->logs | L_YYUTIL | L_TOKENS
      , "%25s ) '%.*s'%s%.*s%s %*s @ %s%.*s%s[%3hu,%3hu - %3hu,%3hu)"
      , tokname ?: ""
      , (int)alen, abuf                  // escaped string from which the token was scanned
      , blen ? " (" : ""
      , (int)blen, bbuf                  // representation of the semantic value for the token
      , blen ? ")" : ""
      , (int)MAX(50 - alen - blen - (blen ? 3 : 0), 0), ""   // padding
      , clen ? "(" : ""             // name of input
      , (int)clen, cbuf
      , clen ? ")" : ""
      , lloc->f_lin                 // location within input
      , lloc->f_col
      , lloc->l_lin
      , lloc->l_col
    );
  }
#endif

  finally : coda;
}

//
// api
//

API void yyu_locreset(yyu_location * const lloc, yyu_parser * const xtra, char * const text)
{
  if(!xtra->yybuffer)
    xtra->yybuffer = text;

  // assign location for current token
  lloc->f_col = xtra->loc.f_col;
  lloc->l_col = xtra->loc.l_col - 1;
  lloc->f_lin = xtra->loc.f_lin;
  lloc->l_lin = xtra->loc.l_lin;
  lloc->s = xtra->loc.s;
  lloc->e = xtra->loc.e;
  lloc->l = lloc->e - lloc->s;

  // reset running location track
  xtra->loc.f_lin = xtra->loc.l_lin + 1;
  xtra->loc.l_lin = xtra->loc.f_lin;
  xtra->loc.f_col = 0;
  xtra->loc.l_col = 0;
}

API void yyu_locwrite(
    yyu_location * const lloc
  , yyu_parser * const xtra
  , char * const text
  , uint16_t end_line
  , uint16_t end_col
  , size_t bytes
)
{
  if(!xtra->yybuffer)
    xtra->yybuffer = text;

  // update running location track
  xtra->loc.f_lin = xtra->loc.l_lin;
  xtra->loc.f_col = xtra->loc.l_col;
  xtra->loc.l_lin = end_line;
  xtra->loc.l_col = end_col;
  xtra->loc.s     = text;
  xtra->loc.e     = text + bytes;
  xtra->loc.l     = bytes;

  // assign location to the current token
  *lloc = xtra->loc;
}

API xapi yyu_pushstate(int state, yyu_parser * const xtra)
{
  enter;

#if DEBUG || DEVEL || XUNIT
  char abuf[256];

  const char * pstate = scanner_statename(xtra, yyu_nstate(xtra, 0));
  const char * nstate = scanner_statename(xtra, state);

  if(log_would(xtra->logs | L_YYUTIL | L_STATES))
  {
    int alen = snprintf(abuf, sizeof(abuf), "%s -> %s"
      , pstate
      , nstate
    );

    // input name
    const char *cbuf;
    size_t clen = 0;
    if(xtra->fname)
    {
      clen = strlen(xtra->fname);
      cbuf = xtra->fname;
    }

    logf(xtra->logs | L_YYUTIL | L_STATES
      , "(%2d) %.*s %*s @ %s%.*s%s%*s "
      , xtra->states_n
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
  }
#endif

  xtra->states[xtra->states_n++] = state;

  finally : coda;
}

API void yyu_dropstate(yyu_parser * const xtra)
{
  xtra->states_n--;
}

API xapi yyu_popstate(yyu_parser * const xtra)
{
  enter;

#if DEBUG || DEVEL || XUNIT
  char abuf[256];
  int x = yyu_nstate(xtra, 0);
#endif
  xtra->states_n--;

#if DEBUG || DEVEL || XUNIT
  const char * pstate = scanner_statename(xtra, yyu_nstate(xtra, 0));
  const char * nstate = scanner_statename(xtra, x);

  if(log_would(xtra->logs | L_YYUTIL | L_STATES))
  {
    int alen = snprintf(abuf, sizeof(abuf), "%s <- %s"
      , pstate
      , nstate
    );

    // input name
    const char *cbuf;
    size_t clen = 0;
    if(xtra->fname)
    {
      clen = strlen(xtra->fname);
      cbuf = xtra->fname;
    }

    logf(xtra->logs | L_YYUTIL | L_STATES
      , "(%2d) %.*s %*s @ %s%.*s%s%*s "
      , xtra->states_n
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
  }
#endif

  finally : coda;
}

API int yyu_nstate(yyu_parser * const xtra, int n)
{
  return (xtra->states_n > n) ? xtra->states[xtra->states_n - n - 1] : -1;
}

API void yyu_scanner_error(yyu_location * const lloc, yyu_parser * const xtra)
{
  xtra->scanerr = 1;

  // save the location
  memcpy(&xtra->error_loc, lloc, sizeof(xtra->error_loc));
}

API xapi yyu_lexify(
    yyu_parser * const restrict xtra
  , int * const restrict ytoken
  , void * const restrict lval
  , size_t lvalsz
  , yyu_location * const restrict lloc
  , char * const restrict text
  , uint16_t end_line
  , uint16_t end_col
  , size_t bytes
  , int token
)
{
  enter;

  yyu_locwrite(lloc, xtra, text, end_line, end_col, bytes);
  fatal(ptoken, xtra, token, lval, lloc, text, bytes);

  // store location for error reporting
  memcpy(&xtra->last_loc, lloc, sizeof(xtra->last_loc));
  xtra->last_token = token;

  // store lval for error reporting
  if(lvalsz >= xtra->last_lval_aloc)
  {
    fatal(xrealloc, &xtra->last_lval, lvalsz + 1, 1, xtra->last_lval_aloc);
    xtra->last_lval_aloc = lvalsz + 1;
  }

  memcpy(xtra->last_lval, lval, lvalsz);
  ((char*)xtra->last_lval)[lvalsz] = 0;

  *ytoken = token;

  finally : coda;
}

API xapi yyu_lexify_valtoken(
    yyu_parser * const restrict xtra
  , int * const restrict ytoken
  , void * const restrict _lval
  , size_t lvalsz
  , yyu_location * const restrict lloc
  , char * const restrict text
  , uint16_t end_line
  , uint16_t end_col
  , size_t bytes
  , int token
)
{
  enter;

  yyu_lval* lval = (typeof(lval))_lval;

  lval->u16 = token;

  fatal(yyu_lexify, xtra, ytoken, lval, lvalsz, lloc, text, end_line, end_col, bytes, token);

  finally : coda;
}

API xapi yyu_lexify_attrs(
    yyu_parser * const restrict parser
  , int * const restrict ytoken
  , void * const restrict lval
  , size_t lvalsz
  , yyu_location * const restrict lloc
  , char * const restrict text
  , int leng
)
{
  enter;

  int token;

  str_token_entry key = { name : text, namel : leng };
  str_token_entry * ent = hashtable_get(parser->str_token_table, &key);
  if(ent)
  {
    token = ent->token;
  }
  else
  {
    RUNTIME_ASSERT(parser->tokens.STR < parser->numtokens);
    token = parser->tokennumbers[parser->tokens.STR];
  }

  fatal(yyu_lexify, parser, ytoken, lval, lvalsz, lloc, text, parser->loc.l_lin, parser->loc.l_col + leng, leng, token);

  finally : coda;
}

API xapi yyu_lexify_enum(
    yyu_parser * const restrict parser
  , int * const restrict ytoken
  , void * const restrict lval
  , size_t lvalsz
  , yyu_location * const restrict lloc
  , char * const restrict text
  , int leng
)
{
  enter;

  int token = 0;

  str_token_entry key = { name : text, namel : leng };
  str_token_entry * ent = hashtable_get(parser->str_token_table, &key);
  if(ent)
  {
    token = ent->token;
  }

  fatal(yyu_lexify, parser, ytoken, lval, lvalsz, lloc, text, parser->loc.l_lin, parser->loc.l_col + leng, leng, token);

  finally : coda;
}

API xapi yyu_lexify_int(
    yyu_parser * const restrict parser
  , int * ytoken
  , void * const restrict _lval
  , size_t lvalsz
  , yyu_location * const restrict lloc
  , char * const text
  , int leng
  , char * restrict fmt
)
{
  enter;

  yyu_lval* lval = (typeof(lval))_lval;

  int token = 0;
  if(strchr(fmt, 'd') || strchr(fmt, 'i'))
  {
    errno = 0;
    sscanf(text, fmt, &lval->imax);

    if(errno == ERANGE)
      fail(KERNEL_ERANGE);

    if(lval->imax < INT32_MIN)
      token = parser->tokennumbers[parser->tokens.INTMIN64];
    else if(lval->imax < INT16_MIN)
      token = parser->tokennumbers[parser->tokens.INTMIN32];
    else if(lval->imax < INT8_MIN)
      token = parser->tokennumbers[parser->tokens.INTMIN16];
    else if(lval->imax < 0)
      token = parser->tokennumbers[parser->tokens.INTMIN8];
    else if(lval->imax <= INT8_MAX)
      token = parser->tokennumbers[parser->tokens.INTMAX8];
    else if(lval->imax <= UINT8_MAX)
      token = parser->tokennumbers[parser->tokens.UINTMAX8];
    else if(lval->imax <= INT16_MAX)
      token = parser->tokennumbers[parser->tokens.INTMAX16];
    else if(lval->imax <= UINT16_MAX)
      token = parser->tokennumbers[parser->tokens.UINTMAX16];
    else if(lval->imax <= INT32_MAX)
      token = parser->tokennumbers[parser->tokens.INTMAX32];
    else if(lval->imax <= UINT32_MAX)
      token = parser->tokennumbers[parser->tokens.UINTMAX32];
    else
      token = parser->tokennumbers[parser->tokens.INTMAX64];
  }
  else if(strchr(fmt, 'u'))
  {
    errno = 0;
    sscanf(text, fmt, &lval->umax);

    if(errno == ERANGE)
      fail(KERNEL_ERANGE);

    if(lval->umax <= INT8_MAX)
      token = parser->tokennumbers[parser->tokens.INTMAX8];
    else if(lval->umax <= UINT8_MAX)
      token = parser->tokennumbers[parser->tokens.UINTMAX8];
    else if(lval->umax <= INT16_MAX)
      token = parser->tokennumbers[parser->tokens.INTMAX16];
    else if(lval->umax <= UINT16_MAX)
      token = parser->tokennumbers[parser->tokens.UINTMAX16];
    else if(lval->umax <= INT32_MAX)
      token = parser->tokennumbers[parser->tokens.INTMAX32];
    else if(lval->umax <= UINT32_MAX)
      token = parser->tokennumbers[parser->tokens.UINTMAX32];
    else if(lval->umax <= INT64_MAX)
      token = parser->tokennumbers[parser->tokens.INTMAX64];
    else
      token = parser->tokennumbers[parser->tokens.UINTMAX64];
  }
  else if(strchr(fmt, 'x'))
  {
    errno = 0;
    sscanf(text, fmt, &lval->umax);

    if(errno == ERANGE)
      fail(KERNEL_ERANGE);

    else if(lval->umax <= UINT8_MAX)
      token = parser->tokennumbers[parser->tokens.HEX8];
    else if(lval->umax <= UINT16_MAX)
      token = parser->tokennumbers[parser->tokens.HEX16];
    else if(lval->umax <= UINT32_MAX)
      token = parser->tokennumbers[parser->tokens.HEX32];
    else
      token = parser->tokennumbers[parser->tokens.HEX64];
  }

  fatal(yyu_lexify, parser, ytoken, lval, lvalsz, lloc, text, parser->loc.l_lin, parser->loc.l_col + leng, leng, token);

  finally : coda;
}

API xapi yyu_lexify_cref(
    yyu_parser * const restrict parser
  , int * ytoken
  , void * const restrict _lval
  , size_t lvalsz
  , yyu_location * const restrict lloc
  , char * const text
  , int leng
  , int off
)
{
  enter;

  yyu_lval* lval = (typeof(lval))_lval;

  if(text[off] == '0')
    lval->c = 0;
  else if(text[off] == 'a')
    lval->c = 0x7;
  else if(text[off] == 'b')
    lval->c = 0x8;
  else if(text[off] == 't')
    lval->c = 0x9;
  else if(text[off] == 'n')
    lval->c = 0xa;
  else if(text[off] == 'v')
    lval->c = 0xb;
  else if(text[off] == 'f')
    lval->c = 0xc;
  else if(text[off] == 'r')
    lval->c = 0xd;
  else if(text[off] == 's')
    lval->c = 0x20;
  else if(text[off] == '\\')
    lval->c = '\\';
  else if(text[off] == '"')
    lval->c = '"';

  RUNTIME_ASSERT(parser->tokens.CREF < parser->numtokens);
  int token = parser->tokennumbers[parser->tokens.CREF];
  fatal(yyu_lexify, parser, ytoken, lval, lvalsz, lloc, text, parser->loc.l_lin, parser->loc.l_col + leng, leng, token);

  finally : coda;
}

API xapi yyu_lexify_href(
    yyu_parser * const restrict parser
  , int * ytoken
  , void * const restrict _lval
  , size_t lvalsz
  , yyu_location * const restrict lloc
  , char * const text
  , int leng
  , int off
)
{
  enter;

  yyu_lval* lval = (typeof(lval))_lval;

  fatal(xparseuint, text + off, "hhx", 0, UINT8_MAX, 1, 2, &lval->u8, 0);

  RUNTIME_ASSERT(parser->tokens.HREF < parser->numtokens);
  int token = parser->tokennumbers[parser->tokens.HREF];
  fatal(yyu_lexify, parser, ytoken, lval, lvalsz, lloc, text, parser->loc.l_lin, parser->loc.l_col + leng, leng, token);

  finally : coda;
}

API xapi yyu_lexify_bool(
    yyu_parser * const restrict parser
  , int * ytoken
  , void * const restrict _lval
  , size_t lvalsz
  , yyu_location * const restrict lloc
  , char * const text
  , int leng
)
{
  enter;

  yyu_lval* lval = (typeof(lval))_lval;

  if(memncmp(text, leng, "true", 4) == 0)
    lval->b = true;
  else
    lval->b = false;

  int token = parser->tokennumbers[parser->tokens.BOOL];
  fatal(yyu_lexify, parser, ytoken, lval, sizeof(*lval), lloc, text, parser->loc.l_lin, parser->loc.l_col + leng, leng, token);

  finally : coda;
}

API xapi yyu_lexify_float(
    yyu_parser * const restrict parser
  , int * ytoken
  , void * const restrict _lval
  , size_t lvalsz
  , yyu_location * const restrict lloc
  , char * const text
  , int leng
)
{
  enter;

  yyu_lval* lval = (typeof(lval))_lval;

  errno = 0;
  sscanf(text, "%lf", &lval->f);

  if(errno != 0)
    fail(KERNEL_ERANGE);

  int token = parser->tokennumbers[parser->tokens.FLOAT];
  fatal(yyu_lexify, parser, ytoken, lval, sizeof(*lval), lloc, text, parser->loc.l_lin, parser->loc.l_col + leng, leng, token);

  finally : coda;
}
