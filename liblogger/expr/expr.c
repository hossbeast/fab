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
#include "narrator.h"
#include "valyria/array.h"
#include "valyria/list.h"

#include "logger.h"
#include "expr.internal.h"
#include "LOGGER.errtab.h"
#include "category.internal.h"
#include "stream.internal.h"
#include "filter.internal.h"
#include "opts.internal.h"

#include "macros.h"

#include <stdio.h>

//
// public
//

int expr_lex(const char * restrict expr, size_t exprl, filter * restrict filterp, uint32_t * restrict attrsp)
{
  int x;
  filter filter = {};
  uint32_t attrs = 0;
  size_t off;
  logger_category * category;

  if(exprl == 0 || (expr[0] != '+' && expr[0] != '-' && expr[0] != '%'))
    return 0;

  if(expr[0] == '+' || expr[0] == '-')
    filter.o = expr[0];

  off = 1;
  while(off != exprl)
  {
    // skip leading delimiters
    while(off < exprl && expr[off] == ',')
      off++;

    // find the end of the name
    for(x = off; x < exprl; x++)
    {
      if((x == off || expr[x] < '0' || expr[x] > '9') && (expr[x] < 'A' || expr[x] > 'Z') && expr[x] != '_')
        break;
    }

    if(x != exprl && expr[x] != ',')
      return 0;

    if(expr[0] == '%') {
      attrs = attrs_combine2(attrs, attrs_value(expr + off, x - off));
    } else if((category = category_byname(expr + off, x - off))) {
      filter.v |= category->id;
    } else {
      filter.u = 1;
    }

    // skip trailing delimiters
    while(x < exprl && expr[x] == ',')
      x++;

    off += (x - off);
  }

  if(off != exprl)
    return 0;

  if(filterp)
    *filterp = filter;

  if(attrsp)
    *attrsp = attrs;

  return 1;
}

int expr_validate(const char * restrict expr, int resolve)
{
  size_t exprl = strlen(expr);

  int x;
  int y = 0;
  for(x = 0; x < exprl; x = y)
  {
    while(x < exprl && (expr[x] == ' ' || expr[x] == '\t'))
      x++;

    y = x;
    while(y < exprl && expr[y] != ' ' && expr[y] != '\t')
      y++;

    /* any logger expression must be at least 3 characters
     *  fixes '-V' as an  argument
     */
    if((y > x) && (y - x) < 3) {
      return 0;
    }

    if(y > x)
    {
      filter f;
      uint32_t a;
      if(!expr_lex(expr + x, y - x, &f, &a))
        return 0;

      if(resolve && f.v == 0 && a == 0)
        return 0;
    }
  }

  return 1;
}

xapi expr_parse(const char * restrict expr, list * restrict filters, uint32_t * restrict attrs)
{
  enter;

  filter * filterp = 0;
  size_t exprl = strlen(expr);
  uint32_t attr;

  int x;
  int y;
  for(x = 0; x < exprl; x = y)
  {
    while(x < exprl && (expr[x] == ' ' || expr[x] == '\t'))
      x++;

    y = x;
    while(y < exprl && expr[y] != ' ' && expr[y] != '\t')
      y++;

    if(y > x)
    {
      if(filterp == 0)
        fatal(xmalloc, &filterp, sizeof(*filterp));
      else
        memset(filterp, 0, sizeof(*filterp));

      attr = 0;
      if(!expr_lex(expr + x, y - x, filterp, &attr))
      {
        xapi_info_pushw("expr", expr, exprl);
        fail(LOGGER_BADEXPR);
      }

      /* if the expr had any filters */
      if(filterp->v) {
        fatal(list_push, filters, filterp, 0);
        filterp = 0;
      }

      /* if the expr had any attrs */
      *attrs = attrs_combine2(*attrs, attr);
    }
  }

finally:
  filter_free(filterp);
coda;
}

//
// api
//
int API logger_expr_resolve(const char * restrict expr)
{
  return expr_validate(expr, 1);
}

int API logger_expr_validate(const char * restrict expr)
{
  return expr_validate(expr, 0);
}

xapi API logger_expr_push(uint8_t stream_id, const char * restrict expr)
{
  enter;

  if(stream_id)
  {
    fatal(stream_expr_push, stream_byid(stream_id), expr);
  }
  else
  {
    int x;
    for(x = 0; x < g_streams_l; x++)
      fatal(stream_expr_push, &g_streams[x], expr);
  }

  finally : coda;
}

xapi API logger_expr_pop(uint8_t stream_id)
{
  enter;

  if(stream_id)
  {
    fatal(stream_expr_pop, stream_byid(stream_id));
  }
  else
  {
    int x;
    for(x = 0; x < g_streams_l; x++)
      fatal(stream_expr_pop, &g_streams[x]);
  }

  finally : coda;
}

xapi API logger_expr_unshift(uint8_t stream_id, const char * restrict expr)
{
  enter;

  if(stream_id)
  {
    fatal(stream_expr_unshift, stream_byid(stream_id), expr);
  }
  else
  {
    int x;
    for(x = 0; x < g_streams_l; x++)
      fatal(stream_expr_unshift, &g_streams[x], expr);
  }

  finally : coda;
}

xapi API logger_expr_shift(uint8_t stream_id)
{
  enter;

  if(stream_id)
  {
    fatal(stream_expr_shift, stream_byid(stream_id));
  }
  else
  {
    int x;
    for(x = 0; x < g_streams_l; x++)
      fatal(stream_expr_shift, &g_streams[x]);
  }

  finally : coda;
}

xapi API logger_expr_reset(uint8_t stream_id)
{
  enter;

  if(stream_id)
  {
    fatal(stream_expr_reset, stream_byid(stream_id));
  }
  else
  {
    int x;
    for(x = 0; x < g_streams_l; x++)
      fatal(stream_expr_reset, &g_streams[x]);
  }

  finally : coda;
}
