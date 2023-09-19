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

#include "valyria/hashtable.h"
#include "valyria/list.h"
#include "valyria/pstring.h"
#include "valyria/set.h"
#include "xlinux/xstdlib.h"

#include "parser.internal.h"
#include "store.internal.h"

static uint32_t set_value_hash(uint32_t h, const void * restrict _val, size_t sz)
{
  value * v = (void*)_val;

  return value_hash(h, v);
}

static int set_value_cmp(const void * _A, size_t Az, const void * _B, size_t Bz)
{
  value * A = (void*)_A;
  value * B = (void*)_B;

  return value_cmp(A, B);
}

//
// public
//

void store_string(value_parser * const restrict parser, value ** rv)
{
  value * val = 0;
  pstring * ps = 0;

  xmalloc(&val, sizeof(*val));
  pscreate(&ps);

  val->s = ps;
  val->type = VALUE_TYPE_STRING;

  list_push(parser->store->strings, ps, 0);
  list_push(parser->store->values, val, 0);
  *rv = val;
}

void store_float(value_parser * const restrict parser, value ** rv)
{
  value * val = 0;

  xmalloc(&val, sizeof(*val));
  val->type = VALUE_TYPE_FLOAT;

  list_push(parser->store->values, val, 0);
  *rv = val;
}

void store_boolean(value_parser * const restrict parser, value ** rv)
{
  value * val = 0;

  xmalloc(&val, sizeof(*val));
  val->type = VALUE_TYPE_BOOLEAN;

  list_push(parser->store->values, val, 0);
  *rv = val;
}

void store_posint(value_parser * const restrict parser, value ** rv)
{
  value * val = 0;

  xmalloc(&val, sizeof(*val));
  val->type = VALUE_TYPE_POSINT;

  list_push(parser->store->values, val, 0);
  *rv = val;
}

void store_negint(value_parser * const restrict parser, value ** rv)
{
  value * val = 0;

  xmalloc(&val, sizeof(*val));
  val->type = VALUE_TYPE_NEGINT;

  list_push(parser->store->values, val, 0);
  *rv = val;
}

void store_variable(value_parser * const restrict parser, value ** rv)
{
  value * val = 0;

  xmalloc(&val, sizeof(*val));
  val->type = VALUE_TYPE_VARIABLE;

  list_push(parser->store->values, val, 0);
  *rv = val;
}

void store_mapping(value_parser * const restrict parser, value ** rv)
{
  value * val = 0;

  xmalloc(&val, sizeof(*val));
  val->type = VALUE_TYPE_MAPPING;

  list_push(parser->store->values, val, 0);
  *rv = val;
}

void store_list(value_parser * const restrict parser, value ** rv)
{
  value * val = 0;
  list * li = 0;

  xmalloc(&val, sizeof(*val));
  list_create(&li);

  val->items = li;
  val->type = VALUE_TYPE_LIST;

  list_push(parser->store->lists, li, 0);
  li = 0;

  list_push(parser->store->values, val, 0);
  *rv = val;
}

void store_set(value_parser * const restrict parser, value ** rv)
{
  value * val = 0;
  set * s = 0;

  xmalloc(&val, sizeof(*val));
  set_createx(&s, 0, set_value_hash, set_value_cmp, 0);

  val->els = s;
  val->type = VALUE_TYPE_SET;

  list_push(parser->store->sets, s, 0);
  list_push(parser->store->values, val, 0);
  *rv = val;
}

//
// api
//

void API value_store_create(value_store ** const restrict stor)
{
  xmalloc(stor, sizeof(**stor));
  list_createx(&(*stor)->values, 0, 0, wfree);
  list_createx(&(*stor)->lists, 0, 0, list_xfree);
  list_createx(&(*stor)->strings, 0, 0, psfree);
  list_createx(&(*stor)->sets, 0, 0, hashtable_xfree);
}

void API value_store_xfree(value_store * const restrict stor)
{
  if(stor)
  {
    list_xfree(stor->values);
    list_xfree(stor->lists);
    list_xfree(stor->strings);
    list_xfree(stor->sets);
  }

  wfree(stor);
}

void API value_store_ixfree(value_store ** const stor)
{
  value_store_xfree(*stor);
  *stor = 0;
}

void API value_store_recycle(value_store * const restrict stor)
{
  list_recycle(stor->values);
  list_recycle(stor->lists);
  list_recycle(stor->strings);
  list_recycle(stor->sets);
}
