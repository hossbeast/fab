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

#include "xapi.h"

struct value;
#define LIST_ELEMENT_TYPE struct value
#include "valyria/list.h"
#include "valyria/pstring.h"

#include "internal.h"
#include "query.internal.h"

#include "strutil.h"

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

static int config_query_compare(void * ud, const value * el, size_t idx)
{
  struct search_context * ctx = ud;
  ctx->idx = idx;

  // keys are strings
  return estrcmp(ctx->key, ctx->len, el->s->s, el->s->l, 0);
}

//
// api
//

API xapi value_query(value * restrict val, const char * const restrict query, value ** restrict rv)
{
  enter;

  const char * start;
  const char * end = query - 1;

  do
  {
    start = end + 1;

    if((end = strchr(start, '.')) == 0)
      end = start + strlen(start);

    if(val->type != VALUE_TYPE_MAP)
      val = 0;

    struct search_context ctx = { key : start, len : end - start };
    if(list_search(val->keys, &ctx, config_query_compare))
      val = list_get(val->vals, ctx.idx);
    else
      val = 0;
  } while(val && *end);

  *rv = val;

  finally : coda;
}
