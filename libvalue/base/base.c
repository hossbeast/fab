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

#include "xapi.h"

#include "valyria/list.h"
#include "valyria/hashtable.h"
#include "valyria/pstring.h"
#include "narrator.h"

#include "internal.h"
#include "store.internal.h"

#include "hash.h"
#include "zbuffer.h"

//
// static
//

static size_t __attribute__((nonnull)) znload_value(void * restrict dst, size_t sz, const value * const restrict val, int level)
{
  size_t z = 0;

  int x;
  if(val->type == VALUE_TYPE_LIST)
  {
    z += znloads(dst + z, sz - z, "(");
    for(x = 0; x < val->items->l; x++)
    {
      z += znloadf(dst + z, sz - z, "\n%*s", (level + 1) * 2, "");
      z += znload_value(dst + z, sz - z, list_get(val->items, x), level + 1);
    }
    if(val->items->l)
      z += znloadf(dst + z, sz - z, "\n%*s", level * 2, "");
    else
      z += znloads(dst + z, sz - z, " ");
    z += znloads(dst + z, sz - z, ")");
  }
  else if(val->type == VALUE_TYPE_SET)
  {
    z += znloads(dst + z, sz - z, "[");
    for(x = 0; x < val->els->table_size; x++)
    {
      value ** elp;
      if((elp = hashtable_table_entry(val->els, x)))
      {
        z += znloadf(dst + z, sz - z, "\n%*s ", (level + 1) * 2, "");
        z += znload_value(dst + z, sz - z, *elp, level + 1);
      }
    }
    if(val->items->l)
      z += znloadf(dst + z, sz - z, "\n%*s", level * 2, "");
    else
      z += znloads(dst + z, sz - z, " ");
    z += znloads(dst + z, sz - z, "]");
  }
  else if(val->type == VALUE_TYPE_MAP)
  {
    z += znloads(dst + z, sz - z, "{");
    for(x = 0; x < val->keys->l; x++)
    {
      z += znloadf(dst + z, sz - z, "\n%*s", (level + 1) * 2, "");
      z += znload_value(dst + z, sz - z, list_get(val->keys, x), level + 1);
      z += znloads(dst + z, sz - z, " ");
      z += znload_value(dst + z, sz - z, list_get(val->vals, x), level + 1);
    }
    if(val->keys->l)
      z += znloadf(dst + z, sz - z, "\n%*s", level * 2, "");
    else
      z += znloads(dst + z, sz - z, " ");
    z += znloads(dst + z, sz - z, "}");
  }
  else if(val->type == VALUE_TYPE_STRING)
  {
    z += znloadf(dst + z, sz - z, "%s", val->s->s);
  }
  else if(val->type == VALUE_TYPE_FLOAT)
  {
    z += znloadf(dst + z, sz - z, "%.2f", val->f);
  }
  else if(val->type == VALUE_TYPE_BOOLEAN)
  {
    z += znloadf(dst + z, sz - z, "%s", val->b ? "true" : "false");
  }
  else if(val->type == VALUE_TYPE_INTEGER)
  {
    z += znloadf(dst + z, sz - z, "%"PRId64, val->i);
  }

  return z;
}

static xapi __attribute__((nonnull)) say_value(const value * const restrict val, int level, narrator * const restrict N)
{
  enter;

  int x;
  if(val->type == VALUE_TYPE_LIST)
  {
    xsays("(");
    for(x = 0; x < val->items->l; x++)
    {
      xsayf("\n%*s", (level + 1) * 2, "");
      fatal(say_value, list_get(val->items, x), level + 1, N);
    }
    if(val->items->l)
      xsayf("\n%*s", level * 2, "");
    else
      xsays(" ");
    xsays(")");
  }
  else if(val->type == VALUE_TYPE_SET)
  {
    xsays("[");
    for(x = 0; x < val->els->table_size; x++)
    {
      value ** elp;
      if((elp = hashtable_table_entry(val->els, x)))
      {
        xsayf("\n%*s ", (level + 1) * 2, "");
        fatal(say_value, *elp, level + 1, N);
      }
    }
    if(val->items->l)
      xsayf("\n%*s", level * 2, "");
    else
      xsays(" ");
    xsays("]");
  }
  else if(val->type == VALUE_TYPE_MAP)
  {
    xsays("{");
    for(x = 0; x < val->keys->l; x++)
    {
      xsayf("\n%*s", (level + 1) * 2, "");
      fatal(say_value, list_get(val->keys, x), level + 1, N);
      xsayf(" ");
      fatal(say_value, list_get(val->vals, x), level + 1, N);
    }
    if(val->keys->l)
      xsayf("\n%*s", level * 2, "");
    else
      xsays(" ");
    xsays("}");
  }
  else if(val->type == VALUE_TYPE_STRING)
  {
    xsayf("%s", val->s->s);
  }
  else if(val->type == VALUE_TYPE_FLOAT)
  {
    xsayf("%.2f", val->f);
  }
  else if(val->type == VALUE_TYPE_BOOLEAN)
  {
    xsayf("%s", val->b ? "true" : "false");
  }
  else if(val->type == VALUE_TYPE_INTEGER)
  {
    xsayf("%"PRId64, val->i);
  }

  finally : coda;
}


//
// api
//

API size_t value_znload(void * restrict dst, size_t sz, const value * const restrict val)
{
  return znload_value(dst, sz, val, 0);
}

API xapi value_say(const value * const restrict val, narrator * const restrict N)
{
  enter;

  if(val == 0)
  {
    xsays("(empty)");
  }
  else
  {
    fatal(say_value, val, 0, N);
  }

  finally : coda;
}

API int value_cmp(const value * const restrict A, const value * const restrict B)
{
  int d;
  int x;

  int r = 0;
  if(A == 0 || B == 0)
  {
    r = (A == 0) - (B == 0);
  }
  else if((d = A->type - B->type))
  {
    r = d;
  }
  else if(A->type == VALUE_TYPE_STRING)
  {
    r = pscmp(A->s, B->s);
  }
  else if(A->type == VALUE_TYPE_FLOAT)
  {
    r = INTCMP(A->f, B->f);
  }
  else if(A->type == VALUE_TYPE_INTEGER)
  {
    r = INTCMP(A->i, B->i);
  }
  else if(A->type == VALUE_TYPE_BOOLEAN)
  {
    r = A->b != B->b;
  }
  else if(A->type == VALUE_TYPE_LIST)
  {
    if((d = A->items->l - B->items->l))
    {
      r = d;
    }
    else
    {
      for(x = 0; x < A->items->l; x++)
      {
        if((d = value_cmp(list_get(A->items, x), list_get(B->items, x))))
        {
          r = d;
          break;
        }
      }
    }
  }
  else if(A->type == VALUE_TYPE_SET)
  {
    r = !hashtable_equal(A->els, B->els);
  }
  else if(A->type == VALUE_TYPE_MAP)
  {
    if((d = A->keys->l - B->keys->l))
    {
      r = d;
    }
    else
    {
      for(x = 0; x < A->keys->l; x++)
      {
        if((d = value_cmp(list_get(A->keys, x), list_get(B->keys, x))))
        {
          r = d;
          break;
        }
        else if((d = value_cmp(list_get(A->vals, x), list_get(B->vals, x))))
        {
          r = d;
          break;
        }
      }
    }
  }

  return r;
}

API bool value_equal(value * const restrict A, value * const restrict B)
{
  if(A == 0 || B == 0)
    return !!A == !!B;

  if(A->type != B->type)
    return false;

  if(value_hash(0, A) != value_hash(0, B))
    return false;

  return value_cmp(A, B) == 0;
}

API uint32_t value_hash(uint32_t h, value * const restrict val)
{
  if(!val->hash)
  {
    uint32_t hash = hash32(0, &val->type, sizeof(val->type));

    int x;
    if(val->type == VALUE_TYPE_LIST)
    {
      for(x = 0; x < val->items->l; x++)
        hash = value_hash(hash, list_get(val->items, x));
    }
    else if(val->type == VALUE_TYPE_SET)
    {
      for(x = 0; x < val->els->table_size; x++)
      {
        value ** elp;
        if((elp = hashtable_table_entry(val->els, x)))
          hash = value_hash(hash, *elp);
      }
    }
    else if(val->type == VALUE_TYPE_MAP)
    {
      for(x = 0; x < val->keys->l; x++)
      {
        hash = value_hash(hash, list_get(val->keys, x));
        hash = value_hash(hash, list_get(val->vals, x));
      }
    }
    else if(val->type == VALUE_TYPE_STRING)
    {
      hash = hash32(hash, val->s->s, val->s->l);
    }
    else if(val->type == VALUE_TYPE_FLOAT)
    {
      hash = hash32(hash, &val->f, sizeof(val->f));
    }
    else if(val->type == VALUE_TYPE_BOOLEAN)
    {
      hash = hash32(hash, &val->i, sizeof(val->i));
    }
    else if(val->type == VALUE_TYPE_INTEGER)
    {
      hash = hash32(hash, &val->i, sizeof(val->i));
    }

    val->hash = hash;
  }

  return h + val->hash;
}
