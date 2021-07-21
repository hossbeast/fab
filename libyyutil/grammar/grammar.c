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

#include "parser.internal.h"
#include "scanner.internal.h"

#include "grammar.internal.h"

#include "common/stresc.h"
#include "zbuffer.h"
#include "macros.h"

//
// api
//

void API yyu_aggregate_location(yyu_location * loc, yyu_location * A, yyu_location * B)
{
  if(A)
  {
    loc->f_col = A->f_col;
    loc->f_lin = A->f_lin;
    loc->l_col = B->l_col;
    loc->l_lin = B->l_lin;
    loc->s = A->s;
    loc->e = B->e;
    loc->l = loc->e - loc->s;
  }
  else
  {
    *loc = *B;
  }
}

void API yyu_loc_initialize(struct yyu_parser * parser, struct yyu_location * lloc)
{
  memset(lloc, 0, sizeof(*lloc));
}

void API yyu_loc_final(struct yyu_parser * parser, struct yyu_location * lloc)
{
  memcpy(parser->final_loc, lloc, sizeof(*lloc));
}

void API yyu_grammar_error(yyu_location * const restrict lloc, void * const restrict scanner, yyu_parser * const restrict parser, char const * err)
{
  char abuf[256];
  char bbuf[256];
  char *s;
  char *e;
  size_t blen;
  size_t alen;
  size_t sz;
  size_t z;
  size_t zt;
  const char *tokname;

  if(parser->gramerr || parser->scanerr)
  {
    // already called
  }
  else
  {
    parser->gramerr = true;

    // save the error string
    if(err)
    {
      if(err && strstr(err, "syntax error, ") == err)
      {
        err += 14;
      }

      parser->error_len = znloadf(parser->error_str, sizeof(parser->error_str), "%s", err);
    }

    // save the location
    memcpy(&parser->error_loc, lloc, sizeof(parser->error_loc));

    // save the tokenstring for the last scanned token
    if(parser->last_token)
    {
      // token source string
      s    = parser->last_loc.s;
      e    = parser->last_loc.e;
      alen = strescw(abuf, sizeof(abuf), s, e - s);

      // token value
      blen = 0;
      if(parser->lvalstr)
        blen = parser->lvalstr(parser, parser->last_token, parser->last_lval, bbuf, sizeof(bbuf));

      s = parser->tokenstring;
      sz = sizeof(parser->tokenstring);
      z = 0;
      zt = 0;

      // token name
      tokname = scanner_tokenname(parser, parser->last_token);
      if(tokname)
      {
        zt = z;
        z += znloads(s + z, sz - z, tokname);
      }

      // escaped string from which the token was scanned
      if(alen)
      {
        if(zt != z)
          z += znloads(s + z, sz - z, " ");
        zt = z;
        z += znloadf(s + z, sz - z, "(%.*s)", (int)alen, abuf);
      }

      // representation of the semantic value for the token
      if(blen)
      {
        if(zt != z)
          z += znloads(s + z, sz - z, " ");
        zt = z;
        z += znloadf(s + z, sz - z, "(%.*s)", (int)blen, bbuf);
      }
    }
  }
}
