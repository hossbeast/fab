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

#include "xapi.h"
#include "xlinux.h"
#include "logger.h"

#include "yyutil.h"

#include "wstdlib.h"
#include "macros.h"
#include "strutil.h"

void yyu_locreset(yyu_location * const lloc, yyu_extra * const xtra, const int del)
{
  // assign location for current token
  lloc->f_col = xtra->loc.f_col + del;
  lloc->l_col = xtra->loc.l_col - 1;
  lloc->f_lin = xtra->loc.f_lin;
  lloc->l_lin = xtra->loc.l_lin;
  lloc->s = xtra->loc.s + del;
  lloc->e = xtra->loc.e;
  lloc->l = lloc->e - lloc->s;

  // reset running location track
  xtra->loc.f_lin++;
  xtra->loc.l_lin = xtra->loc.f_lin;
  xtra->loc.f_col = 0;
  xtra->loc.l_col = 0;
}

void yyu_locwrite(yyu_location * const lloc, yyu_extra * const xtra, char * const text, const int leng, const int del)
{
  // update running location track
  xtra->loc.f_col = xtra->loc.l_col;
  xtra->loc.l_col = xtra->loc.f_col + leng;
  xtra->loc.s     = text;
  xtra->loc.e     = text + leng;

  // assign location to the current token
  lloc->f_col = xtra->loc.f_col + del;
  lloc->l_col = xtra->loc.l_col - 1;
  lloc->f_lin = xtra->loc.f_lin;
  lloc->l_lin = xtra->loc.l_lin;
  if(xtra->loc.s == xtra->loc.e)
  {
    lloc->s = xtra->loc.s + del;
    lloc->e = xtra->loc.s + del;
  }
  else
  {
    lloc->s = xtra->loc.s + del;
    lloc->e = xtra->loc.e;
  }
  lloc->l = lloc->e - lloc->s;
}

xapi yyu_pushstate(const int state, yyu_extra * const xtra)
{
  enter;

  if(log_would(xtra->state_logs))
  {
    int al = snprintf(xtra->space, sizeof(xtra->space), "%s -> %s"
      , xtra->statename(yyu_nstate(xtra, 0))
      , xtra->statename(state)
    );

    logf(xtra->state_logs
      , "(%2d) %.*s %*s @ %*s "
      , xtra->states_n
      , al
      , xtra->space
      , MAX(58 - al, 0)
      , ""
      , 18
      , ""
    );
  }

  xtra->states[xtra->states_n++] = state;

  finally : coda;
}

void yyu_dropstate(yyu_extra * const xtra)
{
  xtra->states_n--;
}

xapi yyu_popstate(yyu_extra * const xtra)
{
  enter;

  int x = yyu_nstate(xtra, 0);
  xtra->states_n--;

  if(log_would(xtra->state_logs))
  {
    int al = snprintf(xtra->space, sizeof(xtra->space), "%s <- %s"
      , xtra->statename(yyu_nstate(xtra, 0))
      , xtra->statename(x)
    );

    logf(xtra->state_logs
      , "(%2d) %.*s %*s @ %*s "
      , xtra->states_n
      , al
      , xtra->space
      , MAX(58 - al, 0)
      , ""
      , 18
      , ""
    );
  }

  finally : coda;
}

int yyu_nstate(yyu_extra * const xtra, const int n)
{
  return (xtra->states_n > n) ? xtra->states[xtra->states_n - n - 1] : -1;
}

xapi yyu_ptoken(const int token, void * const lval, yyu_location * const lloc, yyu_extra * const xtra, char * text, const int leng)
{
  enter;

  if(log_would(xtra->token_logs))
  {
    // token source string
    char * abuf = xtra->space;
    size_t alen = stresc(text, leng, xtra->space, sizeof(xtra->space));

    // token value
    char * bbuf = 0;
    size_t blen = 0;
    if(xtra->lvalstr)
      xtra->lvalstr(token, lval, xtra, &bbuf, &blen);

    // input name
    char * cbuf = 0;
    size_t clen = 0;
    if(xtra->inputstr)
      xtra->inputstr(xtra, &cbuf, &clen);

    logf(xtra->token_logs
      , "%8s ) '%.*s'%s%.*s%s %*s @ %s%.*s%s[%3d,%3d - %3d,%3d]"
      , xtra->tokname(token) ?: ""  // token name
      , alen                        // escaped string from which the token was scanned
      , abuf
      , blen ? " (" : ""
      , blen                        // representation of the semantic value for the token
      , bbuf
      , blen ? ")" : ""
      , MAX(50 - alen - blen - (blen ? 3 : 0), 0)   // padding
      , ""
      , clen ? "(" : ""             // name of input
      , clen
      , cbuf
      , clen ? ")" : ""
      , lloc->f_lin + 1             // location within input
      , lloc->f_col + 1
      , lloc->l_lin + 1
      , lloc->l_col + 1
    );
  }

  finally : coda;
}

void yyu_scanner_error(yyu_location * const lloc, yyu_extra * const xtra)//, const int error, char const * fmt, ...)
{
  // save the error
  xtra->scanerr = 1;//error;

/*
  // save the error string
  if(fmt)
  {
    va_list va;
    va_start(va, fmt);
    vsnprintf(xtra->error_str, sizeof(xtra->error_str), fmt, va);
    va_end(va);
  }
*/

  // save the location
  memcpy(&xtra->error_loc, lloc, sizeof(xtra->error_loc));
}

void yyu_grammar_error(yyu_location * const lloc, void * const scanner, yyu_extra * const xtra, char const * err)
{
  if(xtra->gramerr || xtra->scanerr)
  {
    // already called
  }
  else
  {
    xtra->gramerr = -1;

    // save the error string
    if(err)
    {
      if(err && strstr(err, "syntax error, ") == err)
      {
        err += 14;
      }

      int errlen = MIN(sizeof(xtra->error_str) - 1, strlen(err));
      memcpy(xtra->error_str, err, errlen);
      xtra->error_str[errlen] = 0;
    }

    // save the location
    memcpy(&xtra->error_loc, lloc, sizeof(xtra->error_loc));

    // save the tokenstring for the last scanned token
    if(xtra->last_token)
    {
      // token source string
      char * s    = xtra->last_loc.s;
      char * e    = xtra->last_loc.e;
      char * abuf = xtra->last_loc.s;
      size_t alen = stresc(s, e - s, xtra->space, sizeof(xtra->space));

      // token value
      char * bbuf = 0;
      size_t blen = 0;
      if(xtra->lvalstr)
        xtra->lvalstr(xtra->last_token, xtra->last_lval, xtra, &bbuf, &blen);

      snprintf(xtra->tokenstring, sizeof(xtra->tokenstring)
        , "%s%s%.*s%s%s%.*s%s"
        , xtra->tokname(xtra->last_token) ?: ""   // token name
        , alen ? " (" : ""
        , (int)alen                         // escaped string from which the token was scanned
        , abuf
        , alen ? ")" : ""
        , blen ? " (" : ""
        , (int)blen                         // representation of the semantic value for the token
        , bbuf
        , blen ? ")" : ""
      );
    }
  }
}

xapi yyu_lexify(const int token, void * const lval, const size_t lvalsz, yyu_location * const lloc, yyu_extra * const xtra, char * const text, const int leng, const int del, const int isnl)
{
  enter;

  if(isnl)
    yyu_locreset(lloc, xtra, del);
  else
    yyu_locwrite(lloc, xtra, text, leng, del);

  // possibly log the token
  fatal(yyu_ptoken, token, lval, lloc, xtra, text, leng);

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

  finally : coda;
}

void yyu_extra_destroy(yyu_extra * const xtra)
{
  xfree(xtra->last_lval);
}
