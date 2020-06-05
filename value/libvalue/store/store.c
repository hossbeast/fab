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

#include "types.h"
#include "xapi.h"

#include "xlinux/xstdlib.h"

#include "valyria/list.h"
#include "valyria/set.h"
#include "valyria/pstring.h"

#include "value.h"
#include "value.h"
#include "parser.internal.h"
#include "store.internal.h"

#include "common/grow.h"
#include "common/assure.h"

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

xapi store_string(value_parser * const restrict parser, value ** rv)
{
  enter;

  value * val = 0;
  pstring * ps = 0;

  fatal(xmalloc, &val, sizeof(*val));
  fatal(pscreate, &ps);

  val->s = ps;
  val->type = VALUE_TYPE_STRING;

  fatal(list_push, parser->store->strings, ps, 0);
  ps = 0;

  fatal(list_push, parser->store->values, val, 0);
  *rv = val;
  val = 0;

finally:
  wfree(val);
  psfree(ps);
coda;
}

xapi store_float(value_parser * const restrict parser, value ** rv)
{
  enter;

  value * val = 0;

  fatal(xmalloc, &val, sizeof(*val));
  val->type = VALUE_TYPE_FLOAT;

  fatal(list_push, parser->store->values, val, 0);
  *rv = val;
  val = 0;

  finally : coda;
}

xapi store_boolean(value_parser * const restrict parser, value ** rv)
{
  enter;

  value * val = 0;

  fatal(xmalloc, &val, sizeof(*val));
  val->type = VALUE_TYPE_BOOLEAN;

  fatal(list_push, parser->store->values, val, 0);
  *rv = val;
  val = 0;

  finally : coda;
}

xapi store_posint(value_parser * const restrict parser, value ** rv)
{
  enter;

  value * val = 0;

  fatal(xmalloc, &val, sizeof(*val));
  val->type = VALUE_TYPE_POSINT;

  fatal(list_push, parser->store->values, val, 0);
  *rv = val;
  val = 0;

  finally : coda;
}

xapi store_negint(value_parser * const restrict parser, value ** rv)
{
  enter;

  value * val = 0;

  fatal(xmalloc, &val, sizeof(*val));
  val->type = VALUE_TYPE_NEGINT;

  fatal(list_push, parser->store->values, val, 0);
  *rv = val;
  val = 0;

finally:
  wfree(val);
coda;
}

xapi store_variable(value_parser * const restrict parser, value ** rv)
{
  enter;

  value * val = 0;

  fatal(xmalloc, &val, sizeof(*val));
  val->type = VALUE_TYPE_VARIABLE;

  fatal(list_push, parser->store->values, val, 0);
  *rv = val;
  val = 0;

finally:
  wfree(val);
coda;
}

xapi store_mapping(value_parser * const restrict parser, value ** rv)
{
  enter;

  value * val = 0;

  fatal(xmalloc, &val, sizeof(*val));
  val->type = VALUE_TYPE_MAPPING;

  fatal(list_push, parser->store->values, val, 0);
  *rv = val;
  val = 0;

finally:
  wfree(val);
coda;
}

xapi store_list(value_parser * const restrict parser, value ** rv)
{
  enter;

  value * val = 0;
  list * li = 0;

  fatal(xmalloc, &val, sizeof(*val));
  fatal(list_create, &li);

  val->items = li;
  val->type = VALUE_TYPE_LIST;

  fatal(list_push, parser->store->lists, li, 0);
  li = 0;

  fatal(list_push, parser->store->values, val, 0);
  *rv = val;
  val = 0;

finally:
  wfree(val);
  fatal(list_xfree, li);
coda;
}

xapi store_set(value_parser * const restrict parser, value ** rv)
{
  enter;

  value * val = 0;
  set * s = 0;

  fatal(xmalloc, &val, sizeof(*val));
  fatal(set_createx, &s, 0, set_value_hash, set_value_cmp, 0, 0);

  val->els = s;
  val->type = VALUE_TYPE_SET;

  fatal(list_push, parser->store->sets, s, 0);
  s = 0;

  fatal(list_push, parser->store->values, val, 0);
  *rv = val;
  val = 0;

finally:
  wfree(val);
  fatal(set_xfree, s);
coda;
}

//
// api
//

xapi API value_store_create(value_store ** const restrict stor)
{
  enter;

  fatal(xmalloc, stor, sizeof(**stor));
  fatal(list_createx, &(*stor)->values, 0, 0, wfree, 0);
  fatal(list_createx, &(*stor)->lists, 0, 0, 0, list_xfree);
  fatal(list_createx, &(*stor)->strings, 0, 0, psfree, 0);
  fatal(list_createx, &(*stor)->sets, 0, 0, 0, hashtable_xfree);

  finally : coda;
}

xapi API value_store_xfree(value_store * const restrict stor)
{
  enter;

  if(stor)
  {
    fatal(list_xfree, stor->values);
    fatal(list_xfree, stor->lists);
    fatal(list_xfree, stor->strings);
    fatal(list_xfree, stor->sets);
  }

  wfree(stor);

  finally : coda;
}

xapi API value_store_ixfree(value_store ** const stor)
{
  enter;

  fatal(value_store_xfree, *stor);
  *stor = 0;

  finally : coda;
}

xapi API value_store_recycle(value_store * const restrict stor)
{
  enter;

  fatal(list_recycle, stor->values);
  fatal(list_recycle, stor->lists);
  fatal(list_recycle, stor->strings);
  fatal(list_recycle, stor->sets);

  finally : coda;
}
