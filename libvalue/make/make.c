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
#include "valyria/pstring.h"
#include "valyria/list.h"
#include "valyria/hashtable.h"
#include "yyutil/parser.h"

#include "internal.h"
#include "make.internal.h"
#include "store.internal.h"

#include "macros.h"

//
// static
//

static void __attribute__((nonnull)) location_copy(value_location * const restrict dst, const yyu_location * const restrict src)
{
  dst->fname = src->fname;
  dst->f_lin = src->f_lin;
  dst->f_col = src->f_col;
  dst->l_lin = src->l_lin;
  dst->l_col = src->l_col;
  dst->s = src->s;
  dst->e = src->e;
  dst->l = src->l;
}

struct context {
  size_t lx;
  int lc;
  const struct value * el;
};

static int map_mk_compare(void * _ctx, const void * _A, size_t idx)
{
  struct context * ctx = (typeof(ctx)) _ctx;
  const value * A = (typeof(A)) _A;
  ctx->lx = idx;
  return ctx->lc = pscmp(ctx->el->s, A->s);
};

//
// api
//

API xapi value_float_mk(
    struct value_store * const restrict stor
  , const struct yyu_location * const restrict loc
  , struct value ** rv
  , float fp
)
{
  enter;

  value * e = 0;
  fatal(store_float, stor, &e);

  if(loc)
    location_copy(&e->loc, loc);

  e->f = fp;
  *rv = e;

  finally : coda;
}

API xapi value_integer_mk(
    struct value_store * const restrict stor
  , const struct yyu_location * const restrict loc
  , struct value ** rv
  , int64_t i
)
{
  enter;

  value * e = 0;
  fatal(store_integer, stor, &e);

  if(loc)
    location_copy(&e->loc, loc);

  e->i = i;
  *rv = e;

  finally : coda;
}

API xapi value_boolean_mk(
    struct value_store * const restrict stor
  , const struct yyu_location * const restrict loc
  , struct value ** rv
  , const int b
)
{
  enter;

  value * e = 0;
  fatal(store_boolean, stor, &e);

  if(loc)
    location_copy(&e->loc, loc);

  e->b = b;
  *rv = e;

  finally : coda;
}

API xapi value_string_mks(
    struct value_store * const restrict stor
  , const struct yyu_location * const restrict loc
  , struct value * e
  , struct value ** rv
  , const char * const restrict s
)
{
  enter;

  if(!e)
    fatal(store_string, stor, &e);
  if(loc)
    location_copy(&e->loc, loc);

  fatal(pscats, e->s, s);

  *rv = e;

  finally : coda;
}

API xapi value_string_mkw(
    struct value_store * const restrict stor
  , const struct yyu_location * const restrict loc
  , struct value * e
  , struct value ** rv
  , const char * const restrict b
  , size_t l
)
{
  enter;

  if(!e)
    fatal(store_string, stor, &e);
  if(loc)
    location_copy(&e->loc, loc);

  fatal(pscatw, e->s, b, l);

  *rv = e;

  finally : coda;
}

API xapi value_string_mkc(
    struct value_store * const restrict stor
  , const struct yyu_location * const restrict loc
  , struct value * e
  , struct value ** rv
  , const uint8_t c
)
{
  enter;

  if(!e)
    fatal(store_string, stor, &e);
  if(loc)
    location_copy(&e->loc, loc);

  fatal(pscatc, e->s, c);

  *rv = e;

  finally : coda;
}

API xapi value_string_mkv(
    struct value_store * const restrict stor
  , const struct yyu_location * const restrict loc
  , struct value * e
  , struct value ** rv
  , struct value * v
)

{
  enter;

  if(!e)
    fatal(store_string, stor, &e);
  if(loc)
    location_copy(&e->loc, loc);

  fatal(pscatw, e->s, v->loc.s, v->loc.l);

  *rv = e;

  finally : coda;
}

API xapi value_map_mkv(
    struct value_store * const restrict stor
  , const struct yyu_location * const restrict loc
  , struct value * e
  , struct value ** rv
  , struct value * key
  , struct value * val
  , uint16_t attr
)
{
  enter;

  if(!e)
    fatal(store_map, stor, &e);
  if(loc)
    location_copy(&e->loc, loc);

  key->attr = attr;

  if(e->keys->l == 0)
  {
    fatal(list_push, e->keys, key);
    fatal(list_push, e->vals, val);
  }
  else
  {
    // insert in sorted order
    struct context ctx = { el : key };
    list_search(e->keys, &ctx, map_mk_compare);

    if(ctx.lc == 0) // duplicate key
    {
      fatal(list_set, e->keys, ctx.lx, key);
      fatal(list_set, e->vals, ctx.lx, val);
    }
    else if(ctx.lc < 0)
    {
      fatal(list_insert, e->keys, ctx.lx, key);
      fatal(list_insert, e->vals, ctx.lx, val);
    }
    else
    {
      fatal(list_insert, e->keys, ctx.lx + 1, key);
      fatal(list_insert, e->vals, ctx.lx + 1, val);
    }
  }

  *rv = e;

  finally : coda;
}

API xapi value_map_mks(
    struct value_store * const restrict stor
  , const struct yyu_location * const restrict loc
  , struct value * e
  , struct value ** rv
  , const char * const key
  , struct value * val
  , uint16_t attr
)
{
  enter;

  value * keyval = 0;

  fatal(value_string_mks, stor, 0, 0, &keyval, key);
  fatal(value_map_mkv, stor, loc, e, rv, keyval, val, attr);

  finally : coda;
}

API xapi value_list_mkv(
    struct value_store * const restrict stor
  , const struct yyu_location * const restrict loc
  , struct value * e
  , struct value ** rv
  , struct value * el
)
{
  enter;

  if(!e)
    fatal(store_list, stor, &e);
  if(loc)
    location_copy(&e->loc, loc);

  if(el)
    fatal(list_push, e->items, el);

  *rv = e;

  finally : coda;
}

API xapi value_set_mkv(
    struct value_store * const restrict stor
  , const struct yyu_location * const restrict loc
  , struct value * e
  , struct value ** rv
  , struct value * el
)
{
  enter;

  if(!e)
    fatal(store_set, stor, &e);
  if(loc)
    location_copy(&e->loc, loc);

  if(el)
    fatal(hashtable_put, e->els, &el);

  *rv = e;

  finally : coda;
}
