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

#include "xapi.h"
#include "xapi/exit.h"
#include "xlinux/xstdlib.h"
#include "xlinux/SYS.errtab.h"

#include "internal.h"
#include "yyutil/parser.h"

#include "macros.h"
#include "strutil.h"

//
// api
//

API void yyu_grammar_error(yyu_location * const lloc, void * const scanner, yyu_extra * const xtra, char const * err)
{
  if(xtra->gramerr || xtra->scanerr)
  {
    // already called
  }
  else
  {
    xtra->gramerr = 1;

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

API xapi yyu_reduce(int (*parser)(void *, yyu_extra *), yyu_extra * pp, xapi syntax_error)
{
  enter;

  // error from the parser means failure to reduce
  int r;
  if((r = parser(pp->scanner, pp)) || pp->scanerr)
  {
    if(r == 2)
    {
      // memory exhaustion error from the parser
      tfail(perrtab_SYS, SYS_ENOMEM);
    }
    else if(XAPI_UNWINDING)
    {
      // fail from within a lexer or parser rule
      tfail(0, 0);
    }
    else if(pp->gramerr)
    {
      // failure to reduce from the parser
#if XAPI_STACKTRACE
      tfails(xapi_exit_errtab(syntax_error), xapi_exit_errcode(syntax_error), "message", pp->error_str);
#else
      tfails(0, syntax_error, "message", pp->error_str);
#endif
    }
  }

finally :
  if(XAPI_UNWINDING)
  {
    if(pp->scanerr || pp->gramerr)
    {
      xapi_infof("location", "[%d,%d - %d,%d]"
        , pp->error_loc.f_lin + 1
        , pp->error_loc.f_col + 1
        , pp->error_loc.l_lin + 1
        , pp->error_loc.l_col + 1
      );

      if(pp->gramerr)
      {
        xapi_infos("token", pp->tokenstring);
      }
    }
  }
coda;
}

API void yyu_extra_destroy(yyu_extra * const xtra)
{
  xfree(xtra->last_lval);
}
