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

#include "valyria/pstring.h"
#include "valyria/list.h"
#include "valyria/set.h"

#include "value.h"
#include "make.internal.h"
#include "parser.internal.h"
#include "store.internal.h"

#include "macros.h"

//
// static
//

static void __attribute__((nonnull)) location_copy(value_location * const restrict dst, const yyu_location * const restrict src)
{
  dst->f_lin = src->f_lin;
  dst->f_col = src->f_col;
  dst->l_lin = src->l_lin;
  dst->l_col = src->l_col;
  dst->s = src->s;
  dst->e = src->e;
  dst->l = src->l;
}

//
// api
//

void API value_float_mk(
    value_parser * const restrict parser
  , const yyu_location * const restrict loc
  , struct value ** rv
  , float fp
)
{
  value * e = 0;
  store_float(parser, &e);

  if(loc)
    location_copy(&e->loc, loc);

  e->f = fp;
  *rv = e;
}

void API value_posint_mk(
    value_parser * const restrict parser
  , const yyu_location * const restrict loc
  , struct value ** rv
  , uint64_t u
)
{
  value * e = 0;
  store_posint(parser, &e);

  if(loc)
    location_copy(&e->loc, loc);

  e->u = u;
  *rv = e;
}

void API value_negint_mk(
    value_parser * const restrict parser
  , const yyu_location * const restrict loc
  , struct value ** rv
  , int64_t i
)
{
  value * e = 0;
  store_negint(parser, &e);

  if(loc)
    location_copy(&e->loc, loc);

  e->i = i;
  *rv = e;
}

void API value_boolean_mk(
    value_parser * const restrict parser
  , const yyu_location * const restrict loc
  , struct value ** rv
  , const int b
)
{
  value * e = 0;
  store_boolean(parser, &e);

  if(loc)
    location_copy(&e->loc, loc);

  e->b = b;
  *rv = e;
}

void API value_string_mks(
    value_parser * const restrict parser
  , const yyu_location * const restrict loc
  , struct value * e
  , struct value ** rv
  , const char * const restrict s
)
{
  if(!e)
    store_string(parser, &e);
  if(loc)
    location_copy(&e->loc, loc);

  pscats(e->s, s);

  *rv = e;
}

void API value_string_mkw(
    value_parser * const restrict parser
  , const yyu_location * const restrict loc
  , struct value * e
  , struct value ** rv
  , const char * const restrict b
  , size_t l
)
{
  if(!e)
    store_string(parser, &e);
  if(loc)
    location_copy(&e->loc, loc);

  pscatw(e->s, b, l);

  *rv = e;
}

void API value_string_mkc(
    value_parser * const restrict parser
  , const yyu_location * const restrict loc
  , struct value * e
  , struct value ** rv
  , const uint8_t c
)
{
  if(!e)
    store_string(parser, &e);
  if(loc)
    location_copy(&e->loc, loc);

  pscatc(e->s, c);

  *rv = e;
}

void API value_string_mkv(
    value_parser * const restrict parser
  , const yyu_location * const restrict loc
  , struct value * e
  , struct value ** rv
  , struct value * v
)
{
  if(!e)
    store_string(parser, &e);
  if(loc)
    location_copy(&e->loc, loc);

  pscatw(e->s, v->s->s, v->s->size);

  *rv = e;
}

void API value_mapping_mk(
    value_parser * const restrict parser
  , const yyu_location * const restrict loc
  , struct value ** rv
  , struct value * k
  , struct value * v
  , uint16_t attr
)
{
  value * e = 0;

  store_mapping(parser, &e);
  if(loc)
    location_copy(&e->loc, loc);

  e->key = k;
  e->val = v;
  e->attr = attr;

  *rv = e;
}

void API value_list_mkv(
    value_parser * const restrict parser
  , const yyu_location * const restrict loc
  , struct value * e
  , struct value ** rv
  , struct value * el
)
{
  if(!e)
    store_list(parser, &e);
  if(loc)
    location_copy(&e->loc, loc);

  if(el)
    list_push(e->items, el, 0);

  *rv = e;
}

void API value_set_mkv(
    value_parser * const restrict parser
  , const yyu_location * const restrict loc
  , struct value * e
  , struct value ** rv
  , struct value * el
)
{
  if(!e)
    store_set(parser, &e);
  if(loc)
    location_copy(&e->loc, loc);

  if(el)
    set_put(e->els, el, 0);

  *rv = e;
}

void API value_variable_mkw(
    value_parser * const restrict parser
  , const yyu_location * const restrict loc
  , struct value ** rv
  , const char * const restrict name
  , uint16_t len
)
{
  value *e = 0;

  store_variable(parser, &e);

  if(loc)
    location_copy(&e->loc, loc);

  len = MIN(len, sizeof(e->v.name) - 1);
  memcpy(e->v.name, name, len);
  e->v.name[len] = 0;
  e->v.len = len;

  *rv = e;
}
