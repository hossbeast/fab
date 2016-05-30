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

#if 0
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xapi.h"
#include "xlinux.h"
#include "logger.h"

#include "macros.h"
#endif

#include <stdio.h>
#include <string.h>

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
