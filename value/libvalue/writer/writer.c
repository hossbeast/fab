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
#include "xapi/SYS.errtab.h"
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

static xapi indent(value_writer * const restrict writer)
{
  enter;

  if(writer->mapping != 0 && (writer->mapping % 2) == 0)
  {
    fatal(narrator_xsays, writer->N, " : ");
    writer->mapping -= 2;
  }
  else
  {
    if((writer->mapping % 2) == 1)
    {
      writer->mapping++;
    }

    if(writer->any)
      fatal(narrator_xsayc, writer->N, '\n');

    fatal(narrator_xsayf, writer->N, "%*s", writer->level * 2, "");
  }

  writer->any = true;

  finally : coda;
}

static xapi write_bytes(value_writer * const restrict writer, const char * const restrict s, size_t len)
{
  enter;

  int x;
  char buf[512];
  size_t z;

  if(should_quote(s, len))
  {
    fatal(narrator_xsayc, writer->N, '"');
    for(x = 0; x < len; x += sizeof(buf))
    {
      z = value_string_znloadw(buf, sizeof(buf), &s[x], MIN(sizeof(buf), len - x));
      fatal(narrator_xsayw, writer->N, buf, z);
    }
    fatal(narrator_xsayc, writer->N, '"');
  }
  else
  {
    fatal(narrator_xsayw, writer->N, s, len);
  }

  finally : coda;
}

static xapi pop(value_writer * restrict writer)
{
  enter;

  if(writer->level == 0)
    fail(SYS_INVALID);

  fatal(narrator_xsayc, writer->N, '\n');
  writer->level--;

  fatal(narrator_xsayf, writer->N, "%*s", writer->level * 2, "");

  if(writer->levels[writer->level])
    fatal(narrator_xsayc, writer->N, writer->levels[writer->level]);

  finally : coda;
}

//
// public
//

xapi writer_setup()
{
  enter;

  fatal(set_create, &keywords);

  // keywords
  fatal(set_put, keywords, MMS("true"));
  fatal(set_put, keywords, MMS("false"));

  finally : coda;
}

xapi writer_cleanup()
{
  enter;

  fatal(set_xfree, keywords);

  finally : coda;
}

//
// api
//

xapi API value_writer_create(value_writer ** restrict writer)
{
  enter;

  fatal(xmalloc, writer, sizeof(**writer));

  finally : coda;
}

xapi API value_writer_xfree(value_writer * restrict writer)
{
  enter;

  if(writer)
  {
    fatal(value_writer_close, writer);
  }
  wfree(writer);

  finally : coda;
}

xapi API value_writer_ixfree(value_writer ** restrict writer)
{
  enter;

  fatal(value_writer_xfree, *writer);
  *writer = 0;

  finally : coda;
}

void API value_writer_init(value_writer * restrict writer)
{
  memset(writer, 0, sizeof(*writer));
}

xapi API value_writer_destroy(value_writer * restrict writer)
{
  enter;

  fatal(value_writer_close, writer);

  finally : coda;
}

xapi API value_writer_open(value_writer * restrict writer, struct narrator * restrict N)
{
  enter;

  writer->N = N;

  finally : coda;
}

xapi API value_writer_close(value_writer * restrict writer)
{
  enter;

  if(writer->N)
  {
/* change these to runtime asserts */
    if(writer->level != 0)
    {
      xapi_info_pushf("level", "%d", writer->level);
      fail(SYS_INVALID);
    }
    if(writer->mapping != 0)
    {
      xapi_info_pushf("mapping", "%s", writer->mapping ? "true" : "false");
      fail(SYS_INVALID);
    }

    RUNTIME_ASSERT(writer->level == 0);
    RUNTIME_ASSERT(writer->mapping == 0);

    fatal(narrator_flush, writer->N);
  }
  writer->N = 0;

  finally : coda;
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

xapi API value_writer_value(value_writer * const restrict writer, const value * restrict val)
{
  enter;

  int x;

  if(val->type == VALUE_TYPE_LIST)
  {
    fatal(value_writer_push_list, writer);

    for(x = 0; x < val->items->size; x++)
      fatal(value_writer_value, writer, list_get(val->items, x));

    fatal(value_writer_pop_list, writer);
  }
  else if(val->type == VALUE_TYPE_SET)
  {
    fatal(value_writer_push_set, writer);
    for(x = 0; x < val->els->table_size; x++)
    {
      value * elp;
      if((elp = set_table_get(val->els, x)))
      {
        fatal(value_writer_value, writer, elp);
      }
    }
    fatal(value_writer_pop_set, writer);
  }
  else if(val->type == VALUE_TYPE_MAPPING)
  {
    fatal(value_writer_push_mapping, writer);
    fatal(value_writer_value, writer, val->key);
    fatal(value_writer_value, writer, val->val);
    fatal(value_writer_pop_mapping, writer);
  }
  else if(val->type == VALUE_TYPE_STRING)
  {
    fatal(value_writer_bytes, writer, val->s->s, val->s->size);
  }
  else if(val->type == VALUE_TYPE_FLOAT)
  {
    fatal(value_writer_float, writer, val->f);
  }
  else if(val->type == VALUE_TYPE_BOOLEAN)
  {
    fatal(value_writer_bool, writer, val->b);
  }
  else if(val->type == VALUE_TYPE_POSINT)
  {
    fatal(value_writer_uint, writer, val->u);
  }
  else if(val->type == VALUE_TYPE_NEGINT)
  {
    fatal(value_writer_int, writer, val->i);
  }
  else if(val->type == VALUE_TYPE_VARIABLE)
  {
    fatal(value_writer_variable, writer, val->v.name, val->v.len);
  }

  finally : coda;
}

xapi API value_writer_uint(value_writer * const restrict writer, uint64_t u)
{
  enter;

  fatal(indent, writer);
  fatal(narrator_xsayf, writer->N, "%"PRIu64, u);

  finally : coda;
}

xapi API value_writer_int(value_writer * const restrict writer, int64_t i)
{
  enter;

  fatal(indent, writer);
  fatal(narrator_xsayf, writer->N, "%"PRId64, i);

  finally : coda;
}

xapi API value_writer_bool(value_writer * const restrict writer, bool b)
{
  enter;

  fatal(indent, writer);
  fatal(narrator_xsayf, writer->N, "%s", b ? "true" : "false");

  finally : coda;
}

xapi API value_writer_float(value_writer * const restrict writer, double f)
{
  enter;

  fatal(indent, writer);
  fatal(narrator_xsayf, writer->N, "%.2lf", f);

  finally : coda;
}

xapi API value_writer_bytes(value_writer * const restrict writer, const char * const restrict s, size_t len)
{
  enter;

  fatal(indent, writer);
  fatal(write_bytes, writer, s, len);

  finally : coda;
}

xapi API value_writer_string(value_writer * restrict writer, const char * const restrict s)
{
  enter;

  if(s) {
    fatal(value_writer_bytes, writer, s, strlen(s));
  }

  finally : coda;
}

xapi API value_writer_char(value_writer * const restrict writer, char c)
{
  enter;

  fatal(value_writer_bytes, writer, &c, 1);

  finally : coda;
}

xapi API value_writer_variable(value_writer * const restrict writer, const char * const restrict name, size_t len)
{
  enter;

  fatal(indent, writer);
  fatal(narrator_xsayc, writer->N, '$');
  fatal(narrator_xsayw, writer->N, name, len);

  finally : coda;
}

//
// set
//

xapi API value_writer_push_set(value_writer * restrict writer)
{
  enter;

  if(writer->level == (sizeof(writer->levels) / sizeof(writer->levels[0])))
    fail(SYS_INVALID);

  fatal(indent, writer);
  fatal(narrator_xsayc, writer->N, '{');
  writer->levels[writer->level++] = '}';

  finally : coda;
}

xapi API value_writer_pop_set(value_writer * restrict writer)
{
  xproxy(pop, writer);
}

//
// list
//

xapi API value_writer_push_list(value_writer * restrict writer)
{
  enter;

  if(writer->level == (sizeof(writer->levels) / sizeof(writer->levels[0])))
    fail(SYS_INVALID);

  fatal(indent, writer);
  fatal(narrator_xsayc, writer->N, '[');
  writer->levels[writer->level++] = ']';

  finally : coda;
}

xapi API value_writer_pop_list(value_writer * restrict writer)
{
  xproxy(pop, writer);
}

//
// mapping
//

xapi API value_writer_push_mapping(value_writer * const restrict writer)
{
  enter;

  RUNTIME_ASSERT(writer->mapping >= 0);
  writer->mapping++;

  finally : coda;
}

xapi API value_writer_pop_mapping(value_writer * const restrict writer)
{
  enter;

  finally : coda;
}

xapi API value_writer_mapping_string_string(value_writer * const restrict writer, const char * const restrict k, const char * const restrict s)
{
  enter;

  fatal(value_writer_push_mapping, writer);
  fatal(value_writer_string, writer, k);
  fatal(value_writer_string, writer, s);
  fatal(value_writer_pop_mapping, writer);

  finally : coda;
}

xapi API value_writer_mapping_string_bytes(value_writer * const restrict writer, const char * const restrict k, const char * const restrict s, size_t len)
{
  enter;

  fatal(value_writer_push_mapping, writer);
  fatal(value_writer_string, writer, k);
  fatal(value_writer_bytes, writer, s, len);
  fatal(value_writer_pop_mapping, writer);

  finally : coda;
}

xapi API value_writer_mapping_string_uint(value_writer * const restrict writer, const char * const restrict k, uint64_t u)
{
  enter;

  fatal(value_writer_push_mapping, writer);
  fatal(value_writer_string, writer, k);
  fatal(value_writer_uint, writer, u);
  fatal(value_writer_pop_mapping, writer);

  finally : coda;
}

xapi API value_writer_mapping_string_int(value_writer * const restrict writer, const char * const restrict k, int64_t i)
{
  enter;

  fatal(value_writer_push_mapping, writer);
  fatal(value_writer_string, writer, k);
  fatal(value_writer_int, writer, i);
  fatal(value_writer_pop_mapping, writer);

  finally : coda;
}

xapi API value_writer_mapping_string_bool(value_writer * const restrict writer, const char * const restrict k, bool b)
{
  enter;

  fatal(value_writer_push_mapping, writer);
  fatal(value_writer_string, writer, k);
  fatal(value_writer_bool, writer, b);
  fatal(value_writer_pop_mapping, writer);

  finally : coda;
}

xapi API value_writer_mapping_string_float(value_writer * const restrict writer, const char * const restrict k, double f)
{
  enter;

  fatal(value_writer_push_mapping, writer);
  fatal(value_writer_string, writer, k);
  fatal(value_writer_float, writer, f);
  fatal(value_writer_pop_mapping, writer);

  finally : coda;
}
