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

#include <string.h>
#include <inttypes.h>

#include "xapi.h"
#include "valyria/list.h"
#include "valyria/pstring.h"

#include "internal.h"
#include "query.internal.h"

#include "strutil.h"
#include "parseint.h"

#define restrict __restrict

//
// static
//

struct search_context
{
  const char * key;
  size_t len;
  size_t idx;
};

static int value_query_compare(void * ud, const void * _el, size_t idx)
{
  const value * el = _el;
  struct search_context * ctx = ud;
  ctx->idx = idx;

  // keys are strings
  return estrcmp(ctx->key, ctx->len, el->s->s, el->s->l, 0);
}

//
// api
//

API value * value_query(const value * restrict val, const char * const restrict query)
{
  const char * start = query;
  const char * end = 0;

  while(val && (val->type & VALUE_TYPE_AGGREGATE) && *start)
  {
    end = start + 1;
    while(*end && *end != '.')
      end++;

    if(val->type == VALUE_TYPE_MAP)
    {
      struct search_context ctx = { key : start, len : end - start };
      if(!list_search(val->keys, &ctx, value_query_compare))
        break;

      val = list_get(val->vals, ctx.idx);
    }
    else if(val->type == VALUE_TYPE_LIST)
    {
      uint16_t idx;
      if(parseuint(start, SCNu16, 0, UINT16_MAX, end - start, end - start, &idx, 0))
        break;

      val = list_get(val->els, idx);
    }

    start = end;
    if(*start == '.')
      start++;
  }

  if(!*start)
    return (value*) val;

  return 0;
}
