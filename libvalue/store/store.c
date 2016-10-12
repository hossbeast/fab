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

#include "xlinux/xstdlib.h"

#include "valyria/list.h"
#include "valyria/pstring.h"

#include "internal.h"
#include "store.internal.h"

#include "grow.h"
#include "assure.h"

#define restrict __restrict

//
// public
//

xapi store_string(value_store * const restrict stor, value ** rv)
{
  enter;

  value * val = 0;
  pstring * ps = 0;

  fatal(xmalloc, &val, sizeof(*val));
  fatal(pscreate, &ps);

  val->s = ps;
  val->type = VALUE_TYPE_STRING;

  fatal(list_push, stor->strings, ps);
  ps = 0;

  fatal(list_push, stor->values, val);
  *rv = val;
  val = 0;

finally:
  wfree(val);
  psfree(ps);
coda;
}

xapi store_float(value_store * const restrict stor, value ** rv)
{
  enter;

  value * val = 0;

  fatal(xmalloc, &val, sizeof(*val));
  val->type = VALUE_TYPE_FLOAT;

  fatal(list_push, stor->values, val);
  *rv = val;
  val = 0;

  finally : coda;
}

xapi store_boolean(value_store * const restrict stor, value ** rv)
{
  enter;

  value * val = 0;

  fatal(xmalloc, &val, sizeof(*val));
  val->type = VALUE_TYPE_BOOLEAN;

  fatal(list_push, stor->values, val);
  *rv = val;
  val = 0;

  finally : coda;
}

xapi store_integer(value_store * const restrict stor, value ** rv)
{
  enter;

  value * val = 0;

  fatal(xmalloc, &val, sizeof(*val));
  val->type = VALUE_TYPE_INTEGER;

  fatal(list_push, stor->values, val);
  *rv = val;
  val = 0;

  finally : coda;
}

xapi store_map(value_store * const restrict stor, value ** rv)
{
  enter;

  value * val = 0;
  list * keys = 0;
  list * vals = 0;

  fatal(xmalloc, &val, sizeof(*val));
  fatal(list_create, &keys);
  fatal(list_create, &vals);

  val->keys = keys;
  val->vals = vals;
  val->type = VALUE_TYPE_MAP;

  fatal(list_push, stor->lists, keys);
  keys = 0;

  fatal(list_push, stor->lists, vals);
  vals = 0;

  fatal(list_push, stor->values, val);
  *rv = val;
  val = 0;

finally:
  wfree(val);
  fatal(list_xfree, keys);
  fatal(list_xfree, vals);
coda;
}

xapi store_list(value_store * const restrict stor, value ** rv)
{
  enter;

  value * val = 0;
  list * li = 0;

  fatal(xmalloc, &val, sizeof(*val));
  fatal(list_create, &li);

  val->els = li;
  val->type = VALUE_TYPE_LIST;

  fatal(list_push, stor->lists, li);
  li = 0;

  fatal(list_push, stor->values, val);
  *rv = val;
  val = 0;

finally:
  wfree(val);
  fatal(list_xfree, li);
coda;
}

//
// api
//

API xapi value_store_create(value_store ** const restrict stor)
{
  enter;

  fatal(xmalloc, stor, sizeof(**stor));
  fatal(list_createx, &(*stor)->values, wfree, 0, 0);
  fatal(list_createx, &(*stor)->lists, 0, (void*)list_xfree, 0);
  fatal(list_createx, &(*stor)->strings, (void*)psfree, 0, 0);

  finally : coda;
}

API xapi value_store_xfree(value_store * const restrict stor)
{
  enter;

  if(stor)
  {
    fatal(list_xfree, stor->values);
    fatal(list_xfree, stor->lists);
    fatal(list_xfree, stor->strings);
  }

  wfree(stor);

  finally : coda;
}

API xapi value_store_ixfree(value_store ** const stor)
{
  enter;

  fatal(value_store_xfree, *stor);
  *stor = 0;

  finally : coda;
}
