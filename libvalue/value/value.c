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

#include <inttypes.h>

#include "valyria/list.h"
#include "valyria/pstring.h"
#include "narrator.h"

#include "internal.h"
#include "store.internal.h"

//
// static
//

static xapi __attribute__((nonnull)) say_value_verbose(const value * const restrict val, int level, narrator * const restrict N);
static xapi __attribute__((nonnull)) say_value_verbose(const value * const restrict val, int level, narrator * const restrict N)
{
  enter;

  int x;
  if(val->type == VALUE_TYPE_LIST)
  {
    sayf("%s [", VALUE_TYPE_STRING(val->type));
    for(x = 0; x < val->els->l; x++)
    {
      sayf("\n%*s", (level + 1) * 2, "");
      fatal(say_value_verbose, list_get(val->els, x), level + 1, N);
    }
    sayf("\n%*s]", level * 2, "");
  }

  else if(val->type == VALUE_TYPE_MAP)
  {
    sayf("%s {", VALUE_TYPE_STRING(val->type));
    for(x = 0; x < val->keys->l; x++)
    {
      sayf("\n%*s", (level + 1) * 2, "");
      fatal(say_value_verbose, list_get(val->keys, x), level + 1, N);
      sayf(" : ");
      fatal(say_value_verbose, list_get(val->vals, x), level + 1, N);
    }
    sayf("\n%*s}", level * 2, "");
  }

  else if(val->type == VALUE_TYPE_STRING)
  {
    sayf("%s %s", VALUE_TYPE_STRING(val->type), val->s->s);
  }

  else if(val->type == VALUE_TYPE_FLOAT)
  {
    sayf("%s %f", VALUE_TYPE_STRING(val->type), val->f);
  }

  else if(val->type == VALUE_TYPE_BOOLEAN)
  {
    sayf("%s %s", VALUE_TYPE_STRING(val->type), val->b ? "true" : "false");
  }

  finally : coda;
}

static xapi __attribute__((nonnull)) say_value_canon(const value * const restrict val, int level, narrator * const restrict N)
{
  enter;

  int x;
  if(val->type == VALUE_TYPE_LIST)
  {
    says("[");
    for(x = 0; x < val->els->l; x++)
    {
      sayf("\n%*s", (level + 1) * 2, "");
      fatal(say_value_canon, list_get(val->els, x), level + 1, N);
    }
    sayf("\n%*s]", level * 2, "");
  }

  else if(val->type == VALUE_TYPE_MAP)
  {
    says("{");
    for(x = 0; x < val->keys->l; x++)
    {
      sayf("\n%*s", (level + 1) * 2, "");
      fatal(say_value_canon, list_get(val->keys, x), level + 1, N);
      sayf(" ");
      fatal(say_value_canon, list_get(val->vals, x), level + 1, N);
    }
    sayf("\n%*s}", level * 2, "");
  }

  else if(val->type == VALUE_TYPE_STRING)
  {
    sayf("%s", val->s->s);
  }

  else if(val->type == VALUE_TYPE_FLOAT)
  {
    sayf("%.2f", val->f);
  }

  else if(val->type == VALUE_TYPE_BOOLEAN)
  {
    sayf("%s", val->b ? "true" : "false");
  }

  else if(val->type == VALUE_TYPE_INTEGER)
  {
    sayf("%"PRId64, val->i);
  }

  finally : coda;
}

//
// api
//

API xapi value_say(const value * const restrict val, narrator * const restrict N)
{
  enter;

  if(val == 0)
  {
    says("(empty)");
  }
  else
  {
    fatal(say_value_canon, val, 0, N);
  }

  finally : coda;
}

API xapi value_say_verbose(const value * const restrict val, narrator * const restrict N)
{
  enter;

  if(val == 0)
  {
    says("(empty)");
  }
  else
  {
    fatal(say_value_verbose, val, 0, N);
  }

  finally : coda;
}

API int value_cmp(const value * const restrict A, const value * const restrict B)
{
  int d;

  if(A == 0 || B == 0)
    return (A == 0) - (B == 0);

  else if((d = A->type - B->type))
    return d;

  else if(A->type == VALUE_TYPE_STRING)
    return pscmp(A->s, B->s);

  else if(A->type == VALUE_TYPE_FLOAT)
    return A->f > B->f ? 1 : A->f < B->f ? -1 : 0;

  else if(A->type == VALUE_TYPE_BOOLEAN)
    return (int)(A->b - B->b);

  else if(A->type == VALUE_TYPE_INTEGER)
    return A->i > B->i ? 1 : A->i < B->i ? -1 : 0;

  else if(A->type == VALUE_TYPE_LIST)
  {
    if((d = A->els->l - B->els->l))
      return d;

    int x;
    for(x = 0; x < A->els->l; x++)
    {
      if((d = value_cmp(list_get(A->els, x), list_get(B->els, x))))
        return d;
    }
  }

  else if(A->type == VALUE_TYPE_MAP)
  {
    if((d = A->keys->l - B->keys->l))
      return d;

    int x;
    for(x = 0; x < A->keys->l; x++)
    {
      if((d = value_cmp(list_get(A->keys, x), list_get(B->keys, x))))
        return d;

      if((d = value_cmp(list_get(A->vals, x), list_get(B->vals, x))))
        return d;
    }
  }

  return 0;
}
