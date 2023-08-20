/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <inttypes.h>

#include "narrator.h"
#include "valyria/list.h"
#include "valyria/pstring.h"
#include "valyria/set.h"

#include "value.h"
#include "say.internal.h"
#include "writer.internal.h"
#include "zbuffer.h"

#include "macros.h"

static set * keywords;

//
// static
//

// enum
// keyword
// non-printable
static bool should_escape(char * const restrict s, size_t len)
{
  int x;

  if(set_contains(keywords, s, len))
    return true;

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

void say_setup()
{
  set_create(&keywords);

  // keywords
  set_put(keywords, MMS("true"));
  set_put(keywords, MMS("false"));
}

void say_cleanup()
{
  set_xfree(keywords);
}

//
// api
//

size_t API value_znload(void * restrict dst, size_t sz, const value * const restrict val)
{
  return znload_value(dst, sz, val, 0);
}

void API value_say(const value * const restrict val, narrator * const restrict N)
{
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
    value_writer_open(writer, N);
    value_writer_value(writer, val);
    value_writer_close(writer);
  }

  value_writer_destroy(writer);
}

value * API value_lookupw(const value * restrict set, const char * restrict key, uint16_t key_len)
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

void API value_render(const value * const restrict v, narrator * const restrict N)
{
  int x;
  value * elp;

  if(v->type == VALUE_TYPE_LIST)
  {
    narrator_says(N, "[");

    for(x = 0; x < v->items->size; x++)
    {
      narrator_says(N, " ");
      value_render(list_get(v->items, x), N);
    }

    if(v->items->size)
      narrator_says(N, " ");

    narrator_says(N, "]");
  }
  else if(v->type == VALUE_TYPE_SET)
  {
    narrator_says(N, "{");

    for(x = 0; x < v->els->table_size; x++)
    {
      if(!(elp = set_table_get(v->els, x)))
        continue;

      narrator_says(N, " ");
      value_render(elp, N);
    }

    if(v->els->size)
      narrator_says(N, " ");

    narrator_says(N, "}");
  }
  else if(v->type == VALUE_TYPE_MAPPING)
  {
    value_render(v->key, N);
    narrator_says(N, " : ");
    value_render(v->val, N);
  }
  else if(v->type == VALUE_TYPE_STRING)
  {
    narrator_sayw(N, v->s->s, v->s->size);
  }
  else if(v->type == VALUE_TYPE_FLOAT)
  {
    narrator_sayf(N, "%.2lf", v->f);
  }
  else if(v->type == VALUE_TYPE_BOOLEAN)
  {
    narrator_sayf(N, "%s", v->b ? "true" : "false");
  }
  else if(v->type == VALUE_TYPE_POSINT)
  {
    narrator_sayf(N, "%"PRIu64, v->u);
  }
  else if(v->type == VALUE_TYPE_NEGINT)
  {
    narrator_sayf(N, "%"PRId64, v->i);
  }
}
