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

#include "valyria/set.h"
#include "valyria/list.h"
#include "valyria/pstring.h"
#include "narrator.h"

#include "internal.h"
#include "value.h"
#include "say.internal.h"
#include "parser.internal.h"
#include "store.internal.h"
#include "writer.internal.h"

#include "hash.h"
#include "zbuffer.h"
#include "attrs.h"
#include "stresc.h"

static set * keywords;

//
// static
//

// enum
// keyword
// non-printable
static bool should_escape(char * const restrict s, size_t len)
{
  if(set_contains(keywords, s, len))
    return true;

  int x;
  for(x = 0; x < len; x++)
  {
    if(s[x] <= 0x20 || s[x] >= 0x7e)
      return true;
  }

  return false;
}

static size_t escape(void * restrict dst, size_t sz, const char * const restrict src, size_t len)
{
  size_t z  = 0;

  int x;
  for(x = 0; x < len; x++)
  {
    if(src[x] >= 0x20 && src[x] <= 0x7e)
      z += znloadc(dst + z, sz - z, src[x]);
    else
      z += znloadf(dst + z, sz - z, "\\x%02hhx", src[x]);
  }

  return z;
}

static size_t __attribute__((nonnull)) znload_value(void * restrict dst, size_t sz, const value * const restrict val, int level)
{
  size_t z = 0;

  int x;
  if(val->type == VALUE_TYPE_LIST)
  {
    z += znloads(dst + z, sz - z, "(");
    for(x = 0; x < val->items->size; x++)
    {
      z += znloadf(dst + z, sz - z, "\n%*s", (level + 1) * 2, "");
      z += znload_value(dst + z, sz - z, list_get(val->items, x), level + 1);
    }
    if(val->items->size)
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
      value * elp;
      if((elp = set_table_get(val->els, x)))
      {
        z += znloadf(dst + z, sz - z, "\n%*s ", (level + 1) * 2, "");
        z += znload_value(dst + z, sz - z, elp, level + 1);
      }
    }
    if(val->items->size)
      z += znloadf(dst + z, sz - z, "\n%*s", level * 2, "");
    else
      z += znloads(dst + z, sz - z, " ");
    z += znloads(dst + z, sz - z, "]");
  }
  else if(val->type == VALUE_TYPE_STRING)
  {
    if(should_escape(val->s->s, val->s->size))
    {
      z += znloadc(dst + z, sz - z, '"');
      z += escape(dst + z, sz - z, val->s->s, val->s->size);
      z += znloadc(dst + z, sz - z, '"');
    }
    else
    {
      z += znloadw(dst + z, sz - z, val->s->s, val->s->size);
    }
  }
  else if(val->type == VALUE_TYPE_FLOAT)
  {
    z += znloadf(dst + z, sz - z, "%.2f", val->f);
  }
  else if(val->type == VALUE_TYPE_BOOLEAN)
  {
    z += znloadf(dst + z, sz - z, "%s", val->b ? "true" : "false");
  }
  else if(val->type == VALUE_TYPE_POSINT)
  {
    z += znloadf(dst + z, sz - z, "%"PRIu64, val->u);
  }
  else if(val->type == VALUE_TYPE_NEGINT)
  {
    z += znloadf(dst + z, sz - z, "%"PRId64, val->i);
  }

  return z;
}

//
// public
//

xapi say_setup()
{
  enter;

  fatal(set_create, &keywords);

  // keywords
  fatal(set_put, keywords, MMS("true"));
  fatal(set_put, keywords, MMS("false"));

  finally : coda;
}

xapi say_cleanup()
{
  enter;

  fatal(set_xfree, keywords);

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

  value_writer lwriter;
  value_writer * writer = 0;

  if(val == 0)
  {
    xsays("-none-");
  }
  else
  {
    value_writer_init(&lwriter);
    writer = &lwriter;
    fatal(value_writer_open, writer, N);
    fatal(value_writer_value, writer, val);
    fatal(value_writer_close, writer);
  }

finally:
  fatal(value_writer_destroy, writer);
coda;
}

API value * value_lookupw(const value * restrict set, const char * restrict key, uint16_t key_len)
{
  RUNTIME_ASSERT(set->type == VALUE_TYPE_SET);

  value keyval;
  value *mapping;

  keyval = (typeof(keyval)) {
      type : VALUE_TYPE_MAPPING
    , key : (value[]) {{
          type : VALUE_TYPE_STRING
      , s : (pstring[]) {{
          s : (char*)key
        , size : key_len
      }}
    }}
  };

  if((mapping = set_get(set->els, &keyval, 0)))
    return mapping->val;

  return 0;
}

API xapi value_render(const value * const restrict v, narrator * const restrict N)
{
  enter;

  int x;
  value * elp;

  if(v->type == VALUE_TYPE_LIST)
  {
    fatal(narrator_xsays, N, "[");

    for(x = 0; x < v->items->size; x++)
    {
      fatal(narrator_xsays, N, " ");
      fatal(value_render, list_get(v->items, x), N);
    }

    if(v->items->size)
      fatal(narrator_xsays, N, " ");

    fatal(narrator_xsays, N, "]");
  }
  else if(v->type == VALUE_TYPE_SET)
  {
    fatal(narrator_xsays, N, "{");

    for(x = 0; x < v->els->table_size; x++)
    {
      if(!(elp = set_table_get(v->els, x)))
        continue;

      fatal(narrator_xsays, N, " ");
      fatal(value_render, elp, N);
    }

    if(v->els->size)
      fatal(narrator_xsays, N, " ");

    fatal(narrator_xsays, N, "}");
  }
  else if(v->type == VALUE_TYPE_MAPPING)
  {
    fatal(value_render, v->key, N);
    fatal(narrator_xsays, N, " : ");
    fatal(value_render, v->val, N);
  }
  else if(v->type == VALUE_TYPE_STRING)
  {
    fatal(narrator_xsayw, N, v->s->s, v->s->size);
  }
  else if(v->type == VALUE_TYPE_FLOAT)
  {
    fatal(narrator_xsayf, N, "%.2lf", v->f);
  }
  else if(v->type == VALUE_TYPE_BOOLEAN)
  {
    fatal(narrator_xsayf, N, "%s", v->b ? "true" : "false");
  }
  else if(v->type == VALUE_TYPE_POSINT)
  {
    fatal(narrator_xsayf, N, "%"PRIu64, v->u);
  }
  else if(v->type == VALUE_TYPE_NEGINT)
  {
    fatal(narrator_xsayf, N, "%"PRId64, v->i);
  }

  finally : coda;
}
