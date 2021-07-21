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

#include "narrator.h"
#include "valyria/list.h"
#include "valyria/pstring.h"
#include "valyria/set.h"
#include "xlinux/xstdlib.h"

#include "value.h"
#include "writer.internal.h"

#include "zbuffer.h"

static set * keywords;

//
// static
//

// enum
// keyword
// non-printable
static bool should_quote(const char * const restrict s, size_t len)
{
  int x;

  if(set_contains(keywords, s, len))
    return true;

  for(x = 0; x < len; x++)
  {
    if((s[x] >= 'a' && s[x] <= 'z') || (s[x] >= 'A' && s[x] <= 'Z') || (s[x] >= '0' && s[x] <= '9') || s[x] == '_' || s[x] == '/' || s[x] == '.')
      continue;

    if(x > 0 && s[x] == '-')
      continue;

    break;
  }

  return x < len;
}

static void indent(value_writer * const restrict writer)
{
  if(writer->mapping != 0 && (writer->mapping % 2) == 0)
  {
    narrator_xsays(writer->N, " : ");
    writer->mapping -= 2;
  }
  else
  {
    if((writer->mapping % 2) == 1)
    {
      writer->mapping++;
    }

    if(writer->any)
      narrator_xsayc(writer->N, '\n');

    narrator_xsayf(writer->N, "%*s", writer->level * 2, "");
  }

  writer->any = true;
}

static void write_bytes(value_writer * const restrict writer, const char * const restrict s, size_t len)
{
  int x;
  char buf[512];
  size_t z;

  if(should_quote(s, len))
  {
    narrator_xsayc(writer->N, '"');
    for(x = 0; x < len; x += sizeof(buf))
    {
      z = value_string_znloadw(buf, sizeof(buf), &s[x], MIN(sizeof(buf), len - x));
      narrator_xsayw(writer->N, buf, z);
    }
    narrator_xsayc(writer->N, '"');
  }
  else
  {
    narrator_xsayw(writer->N, s, len);
  }
}

static void pop(value_writer * restrict writer)
{
  RUNTIME_ASSERT(writer->level != 0);

  narrator_xsayc(writer->N, '\n');
  writer->level--;

  narrator_xsayf(writer->N, "%*s", writer->level * 2, "");

  if(writer->levels[writer->level])
    narrator_xsayc(writer->N, writer->levels[writer->level]);
}

//
// public
//

void writer_setup()
{
  set_create(&keywords);

  // keywords
  set_put(keywords, MMS("true"));
  set_put(keywords, MMS("false"));
}

void writer_cleanup()
{
  set_xfree(keywords);
}

//
// api
//

void API value_writer_create(value_writer ** restrict writer)
{
  xmalloc(writer, sizeof(**writer));
}

void API value_writer_xfree(value_writer * restrict writer)
{
  if(writer)
  {
    value_writer_close(writer);
  }
  wfree(writer);
}

void API value_writer_ixfree(value_writer ** restrict writer)
{
  value_writer_xfree(*writer);
  *writer = 0;
}

void API value_writer_init(value_writer * restrict writer)
{
  memset(writer, 0, sizeof(*writer));
}

void API value_writer_destroy(value_writer * restrict writer)
{
  value_writer_close(writer);
}

void API value_writer_open(value_writer * restrict writer, struct narrator * restrict N)
{
  writer->N = N;
}

void API value_writer_close(value_writer * restrict writer)
{
  if(writer->N)
  {
    RUNTIME_ASSERT(writer->level == 0);
    RUNTIME_ASSERT(writer->mapping == 0);

    narrator_flush(writer->N);
  }
  writer->N = 0;
}

//
// primitives
//

size_t API value_string_znloadw(void * restrict dst, size_t sz, const void * restrict b, size_t bz)
{
  size_t z  = 0;
  const char *src;

  src = b;
  int x;
  for(x = 0; x < bz; x++)
  {
    if(src[x] >= 0x20 && src[x] <= 0x7e)
      z += znloadc(dst + z, sz - z, src[x]);
    else
      z += znloadf(dst + z, sz - z, "\\x%02hhx", src[x]);
  }

  return z;
}

void API value_writer_value(value_writer * const restrict writer, const value * restrict val)
{
  int x;

  if(val->type == VALUE_TYPE_LIST)
  {
    value_writer_push_list(writer);

    for(x = 0; x < val->items->size; x++)
      value_writer_value(writer, list_get(val->items, x));

    value_writer_pop_list(writer);
  }
  else if(val->type == VALUE_TYPE_SET)
  {
    value_writer_push_set(writer);
    for(x = 0; x < val->els->table_size; x++)
    {
      value * elp;
      if((elp = set_table_get(val->els, x)))
      {
        value_writer_value(writer, elp);
      }
    }
    value_writer_pop_set(writer);
  }
  else if(val->type == VALUE_TYPE_MAPPING)
  {
    value_writer_push_mapping(writer);
    value_writer_value(writer, val->key);
    value_writer_value(writer, val->val);
    value_writer_pop_mapping(writer);
  }
  else if(val->type == VALUE_TYPE_STRING)
  {
    value_writer_bytes(writer, val->s->s, val->s->size);
  }
  else if(val->type == VALUE_TYPE_FLOAT)
  {
    value_writer_float(writer, val->f);
  }
  else if(val->type == VALUE_TYPE_BOOLEAN)
  {
    value_writer_bool(writer, val->b);
  }
  else if(val->type == VALUE_TYPE_POSINT)
  {
    value_writer_uint(writer, val->u);
  }
  else if(val->type == VALUE_TYPE_NEGINT)
  {
    value_writer_int(writer, val->i);
  }
  else if(val->type == VALUE_TYPE_VARIABLE)
  {
    value_writer_variable(writer, val->v.name, val->v.len);
  }
}

void API value_writer_uint(value_writer * const restrict writer, uint64_t u)
{
  indent(writer);
  narrator_xsayf(writer->N, "%"PRIu64, u);
}

void API value_writer_int(value_writer * const restrict writer, int64_t i)
{
  indent(writer);
  narrator_xsayf(writer->N, "%"PRId64, i);
}

void API value_writer_bool(value_writer * const restrict writer, bool b)
{
  indent(writer);
  narrator_xsayf(writer->N, "%s", b ? "true" : "false");
}

void API value_writer_float(value_writer * const restrict writer, double f)
{
  indent(writer);
  narrator_xsayf(writer->N, "%.2lf", f);
}

void API value_writer_bytes(value_writer * const restrict writer, const char * const restrict s, size_t len)
{
  indent(writer);
  write_bytes(writer, s, len);
}

void API value_writer_string(value_writer * restrict writer, const char * const restrict s)
{
  if(s) {
    value_writer_bytes(writer, s, strlen(s));
  }
}

void API value_writer_char(value_writer * const restrict writer, char c)
{
  value_writer_bytes(writer, &c, 1);
}

void API value_writer_variable(value_writer * const restrict writer, const char * const restrict name, size_t len)
{
  indent(writer);
  narrator_xsayc(writer->N, '$');
  narrator_xsayw(writer->N, name, len);
}

//
// set
//

void API value_writer_push_set(value_writer * restrict writer)
{
  RUNTIME_ASSERT(writer->level != (sizeof(writer->levels) / sizeof(writer->levels[0])));

  indent(writer);
  narrator_xsayc(writer->N, '{');
  writer->levels[writer->level++] = '}';
}

void API value_writer_pop_set(value_writer * restrict writer)
{
  pop(writer);
}

//
// list
//

void API value_writer_push_list(value_writer * restrict writer)
{
  RUNTIME_ASSERT(writer->level != (sizeof(writer->levels) / sizeof(writer->levels[0])));

  indent(writer);
  narrator_xsayc(writer->N, '[');
  writer->levels[writer->level++] = ']';
}

void API value_writer_pop_list(value_writer * restrict writer)
{
  pop(writer);
}

//
// mapping
//

void API value_writer_push_mapping(value_writer * const restrict writer)
{
  RUNTIME_ASSERT(writer->mapping >= 0);
  writer->mapping++;
}

void API value_writer_pop_mapping(value_writer * const restrict writer)
{
}

void API value_writer_mapping_string_string(value_writer * const restrict writer, const char * const restrict k, const char * const restrict s)
{
  value_writer_push_mapping(writer);
  value_writer_string(writer, k);
  value_writer_string(writer, s);
  value_writer_pop_mapping(writer);
}

void API value_writer_mapping_string_bytes(value_writer * const restrict writer, const char * const restrict k, const char * const restrict s, size_t len)
{
  value_writer_push_mapping(writer);
  value_writer_string(writer, k);
  value_writer_bytes(writer, s, len);
  value_writer_pop_mapping(writer);
}

void API value_writer_mapping_string_uint(value_writer * const restrict writer, const char * const restrict k, uint64_t u)
{
  value_writer_push_mapping(writer);
  value_writer_string(writer, k);
  value_writer_uint(writer, u);
  value_writer_pop_mapping(writer);
}

void API value_writer_mapping_string_int(value_writer * const restrict writer, const char * const restrict k, int64_t i)
{
  value_writer_push_mapping(writer);
  value_writer_string(writer, k);
  value_writer_int(writer, i);
  value_writer_pop_mapping(writer);
}

void API value_writer_mapping_string_bool(value_writer * const restrict writer, const char * const restrict k, bool b)
{
  value_writer_push_mapping(writer);
  value_writer_string(writer, k);
  value_writer_bool(writer, b);
  value_writer_pop_mapping(writer);
}

void API value_writer_mapping_string_float(value_writer * const restrict writer, const char * const restrict k, double f)
{
  value_writer_push_mapping(writer);
  value_writer_string(writer, k);
  value_writer_float(writer, f);
  value_writer_pop_mapping(writer);
}
