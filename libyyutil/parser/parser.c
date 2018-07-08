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
#include "xlinux/KERNEL.errtab.h"

#include "internal.h"
#include "yyutil/parser.h"

#include "macros.h"
#include "stresc.h"
#include "zbuffer.h"

//
// api
//

API void yyu_grammar_error(yyu_location * const lloc, void * const scanner, yyu_extra * const xtra, char const * err)
{
  char abuf[256];
  char bbuf[256];

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
      size_t alen = strescw(s, e - s, abuf, sizeof(abuf));

      // token value
      size_t blen = 0;
      if(xtra->lvalstr)
        blen = xtra->lvalstr(xtra->last_token, xtra->last_lval, xtra, bbuf, sizeof(bbuf));

      s = xtra->tokenstring;
      size_t sz = sizeof(xtra->tokenstring);
      size_t z = 0;
      size_t zt;

      // token name
      zt = z, z += znloads(s + z, sz - z, xtra->tokname(xtra->last_token) ?: "");

      // escaped string from which the token was scanned
      if(alen)
      {
        if(zt != z)
          z += znloads(s + z, sz - z, " ");
        zt = z, z += znloadf(s + z, sz - z, "(%.*s)", (int)alen, abuf);
      }

      // representation of the semantic value for the token
      if(blen)
      {
        if(zt != z)
          z += znloads(s + z, sz - z, " ");
        zt = z, z += znloadf(s + z, sz - z, "(%.*s)", (int)blen, bbuf);
      }
    }
  }
}

API xapi yyu_reduce(int (*parser)(void *, yyu_extra *), yyu_extra * xtra, xapi syntax_error)
{
  enter;

  // error from the parser means failure to reduce
  int r;
  if((r = parser(xtra->scanner, xtra)) || xtra->scanerr)
  {
    if(r == 2)
    {
      // memory exhaustion error from the parser
      fail(KERNEL_ENOMEM);
    }
    else if(XAPI_UNWINDING)
    {
      // fail from within a lexer or parser rule
      fail(0);
    }
    else if(xtra->gramerr)
    {
      // failure to reduce from the parser
      fails(syntax_error, "message", xtra->error_str);
    }
  }

finally :
  if(XAPI_UNWINDING)
  {
    if(xtra->scanerr || xtra->gramerr)
    {
      xapi_infof("location", "[%d,%d - %d,%d]"
        , xtra->error_loc.f_lin + 1
        , xtra->error_loc.f_col + 1
        , xtra->error_loc.l_lin + 1
        , xtra->error_loc.l_col + 1
      );

      if(xtra->gramerr)
      {
        xapi_infos("token", xtra->tokenstring);
      }
    }
  }
coda;
}

API void yyu_extra_destroy(yyu_extra * const xtra)
{
  wfree(xtra->last_lval);
}
