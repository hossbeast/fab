/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#include <inttypes.h>

#include "narrator.h"
#include "valyria/pstring.h"
#include "valyria/hashtable.h"
#include "xlinux/xstdlib.h"

#include "value.h"
#include "writer.h"
#include "keywords.h"

#include "zcat.h"

//
// static
//

// enum
// keyword
// non-printable
static bool should_quote(const char * const restrict s, size_t len)
{
  int x;

  if(hashtable_contains(&keywords, (struct keyword[]) {{ s : s, len : len }})) {
    return true;
  }

  /* parity with the unquoted STR rule in parser/value.l */
  for(x = 0; x < len; x++)
  {
    if(s[x] >= 'a' && s[x] <= 'z')
      continue;
    if(s[x] >= 'A' && s[x] <= 'Z')
      continue;
    if(s[x] == '.' || s[x] == '/' || s[x] == '_')
      continue;
    if(s[x] == '+' || s[x] == '-' || s[x] == '=')
      continue;
    if(x == 0)
      break;
    if(s[x] >= '0' && s[x] <= '9')
      continue;

    break;
  }

  return x < len;
}

static void indent(bacon_writer * const restrict writer)
{
  if(writer->mapping != 0 && (writer->mapping % 2) == 0)
  {
    writer->nl_off += narrator_says(writer->N, " : ");
    writer->mapping -= 2;
  }
  else
  {
    if((writer->mapping % 2) == 1)
    {
      writer->mapping++;
    }

    if(writer->any) {
      if(writer->singleline) {
        writer->nl_off += narrator_sayc(writer->N, ' ');
      } else {
        narrator_sayc(writer->N, '\n');
        writer->nl_off = 0;
      }
    }

    if(!writer->singleline) {
      writer->nl_off += narrator_sayf(writer->N, "%*s", writer->level * 2, "");
    }
  }

  writer->any = true;
}

static void write_string(bacon_writer * const restrict writer, const char * const restrict s, size_t len)
{
  int x;
  char buf[512];
  size_t z;

  if(should_quote(s, len))
  {
    writer->nl_off += narrator_sayc(writer->N, '"');
    for(x = 0; x < len; x += (sizeof(buf) / 4))
    {
      z = bacon_value_string_zcatw(buf, sizeof(buf), &s[x], MIN(sizeof(buf) / 4, len - x));
      writer->nl_off += narrator_sayw(writer->N, buf, z);
    }
    writer->nl_off += narrator_sayc(writer->N, '"');
  }
  else
  {
    writer->nl_off += narrator_sayw(writer->N, s, len);
  }
}

size_t bacon_value_string_zcatw(void * restrict dst, size_t sz, const void * restrict b, size_t bz)
{
  size_t z  = 0;
  const char *src;
  int x;

  /* parity with the unquoted STR rule in parser/value.l */

  src = b;
  for(x = 0; x < bz; x++)
  {
    if(src[x] >= 0x20 && src[x] <= 0x7e && src[x] != '"') {
      z += zcatc(dst + z, sz - z, src[x]);
    } else if(src[x] == 0x7) {
      z += zcats(dst + z, sz - z, "\\a");
    } else if(src[x] == 0x8) {
      z += zcats(dst + z, sz - z, "\\b");
    } else if(src[x] == 0x9) {
      z += zcats(dst + z, sz - z, "\\t");
    } else if(src[x] == 0xa) {
      z += zcats(dst + z, sz - z, "\\n");
    } else if(src[x] == 0xb) {
      z += zcats(dst + z, sz - z, "\\v");
    } else if(src[x] == 0xc) {
      z += zcats(dst + z, sz - z, "\\f");
    } else if(src[x] == 0xd) {
      z += zcats(dst + z, sz - z, "\\r");
    } else if(src[x] == 0x20) {
      z += zcats(dst + z, sz - z, "\\s");
    } else if(src[x] == '\\') {
      z += zcats(dst + z, sz - z, "\\\\");
    } else if(src[x] == '"') {
      z += zcats(dst + z, sz - z, "\\\"");
    } else {
      z += zcatf(dst + z, sz - z, "\\x%02hhx", src[x]);
    }
  }

  return z;
}

size_t bacon_value_string_zcatvf(void * restrict dst, size_t sz, const char * restrict fmt, va_list va)
{
  return zcatvf(dst, sz, fmt, va);
}

static void pop(bacon_writer * restrict writer)
{
  RUNTIME_ASSERT(writer->level != 0);

  if(writer->singleline) {
    writer->nl_off += narrator_sayc(writer->N, ' ');
  } else {
    narrator_sayc(writer->N, '\n');
    writer->nl_off = 0;
  }
  writer->level--;

  if(!writer->singleline) {
    writer->nl_off += narrator_sayf(writer->N, "%*s", writer->level * 2, "");
  }

  if(writer->levels[writer->level]) {
    writer->nl_off += narrator_sayc(writer->N, writer->levels[writer->level]);
  }
}

//
// api
//

void bacon_writer_create(bacon_writer ** restrict writer)
{
  xmalloc(writer, sizeof(**writer));
  bacon_writer_init(*writer);
}

void bacon_writer_free(bacon_writer * restrict writer)
{
  if(!writer) {
    return;
  }

  bacon_writer_destroy(writer);
  free(writer);
}

void bacon_writer_ifree(bacon_writer ** restrict writer)
{
  bacon_writer_free(*writer);
  *writer = 0;
}

void bacon_writer_init(bacon_writer * restrict writer)
{
  memset(writer, 0, sizeof(*writer));
}

void bacon_writer_destroy(bacon_writer * restrict writer)
{
  bacon_writer_close(writer);
}

bacon_writer *bacon_writer_open(bacon_writer * restrict writer, struct narrator * restrict N)
{
  writer->N = N;
  return writer;
}

void bacon_writer_close(bacon_writer * restrict writer)
{
  if(writer->N)
  {
    RUNTIME_ASSERT(writer->level == 0);
    RUNTIME_ASSERT(writer->mapping == 0);
    RUNTIME_ASSERT(writer->string == 0);
  }
  writer->N = 0;
}

//
// primitives
//

void bacon_writer_value(bacon_writer * const restrict writer, const bacon_value * restrict val)
{
  bacon_value *v;

  switch(val->type)
  {
    case BACON_TYPE_LIST:
    {
      bacon_writer_push_list(writer);

      llist_foreach(&val->list, v, lln) {
        bacon_writer_value(writer, v);
      }

      bacon_writer_pop_list(writer);
      break;
    }
    case BACON_TYPE_SET:
    {
      bacon_writer_push_set(writer);
      rbtree_foreach(&val->set, v, rbn) {
        bacon_writer_value(writer, v);
      }
      bacon_writer_pop_set(writer);
      break;
    }
    case BACON_TYPE_MAPPING:
      bacon_writer_push_mapping(writer);
      bacon_writer_value(writer, val->key);
      bacon_writer_value(writer, val->val);
      bacon_writer_pop_mapping(writer);
      break;
    case BACON_TYPE_STRING:
      bacon_writer_bytes(writer, val->s, val->len);
      break;
    case BACON_TYPE_FLOAT:
      bacon_writer_float(writer, val->f);
      break;
    case BACON_TYPE_BOOLEAN:
      bacon_writer_bool(writer, val->b);
      break;
    case BACON_TYPE_INTEGER:
      bacon_writer_int(writer, val->i);
      break;
    case BACON_TYPE_VARIABLE:
      bacon_writer_variable(writer, val->s, val->len);
      break;
    default:
      fprintf(stderr, "type %d\n", val->type);
      RTA;
  }
}

void bacon_writer_uint(bacon_writer * const restrict writer, uint64_t u)
{
  indent(writer);
  writer->nl_off += narrator_sayf(writer->N, "%"PRIu64, u);
}

void bacon_writer_hex(bacon_writer * const restrict writer, uint64_t u)
{
  indent(writer);
  writer->nl_off += narrator_sayf(writer->N, "0x%"PRIx64, u);
}

void bacon_writer_int(bacon_writer * const restrict writer, int64_t i)
{
  indent(writer);
  writer->nl_off += narrator_sayf(writer->N, "%"PRId64, i);
}

void bacon_writer_bool(bacon_writer * const restrict writer, bool b)
{
  indent(writer);
  writer->nl_off += narrator_sayf(writer->N, "%s", b ? "true" : "false");
}

void bacon_writer_float(bacon_writer * const restrict writer, double f)
{
  indent(writer);
  writer->nl_off += narrator_sayf(writer->N, "%.6f", f);
}

void bacon_writer_bytes(bacon_writer * const restrict writer, const char * const restrict s, size_t len)
{
  if(len == 0) {
    return;
  }

  if(writer->string)
  {
    writer->nl_off += narrator_sayw(writer->N, s, len);
  }
  else
  {
    indent(writer);
    write_string(writer, s, len);
  }
}

void bacon_writer_string(bacon_writer * restrict writer, const char * restrict s)
{
  uint16_t len;

  if((!s || (len = strlen(s)) == 0) && writer->mapping) {
    s = "_";
    len = 1;
  }

  if(writer->string)
  {
    writer->nl_off += narrator_sayw(writer->N, s, len);
  }
  else
  {
    indent(writer);
    write_string(writer, s, len);
  }
}

void bacon_writef(bacon_writer * restrict writer, const char * restrict fmt, ...)
{
  char buf[512];
  va_list va;
  size_t z;

  va_start(va, fmt);
  z = vsnprintf(buf, sizeof(buf), fmt, va);
  va_end(va);

  bacon_writer_bytes(writer, buf, z);
}

void bacon_writer_char(bacon_writer * const restrict writer, char c)
{
  bacon_writer_bytes(writer, &c, 1);
}

void bacon_writer_variable(bacon_writer * const restrict writer, const char * const restrict name, size_t len)
{
  indent(writer);
  writer->nl_off += narrator_sayc(writer->N, '$');
  writer->nl_off += narrator_sayw(writer->N, name, len);
}

//
// string
//

void bacon_writer_push_string(bacon_writer * restrict writer)
{
  RUNTIME_ASSERT(writer->string == 0);

  writer->string = 1;
  indent(writer);
  writer->nl_off += narrator_sayc(writer->N, '"');
}

void bacon_writer_pop_string(bacon_writer * restrict writer)
{
  writer->nl_off += narrator_sayc(writer->N, '"');
  writer->string = 0;
}

void bacon_writer_push_bytes(bacon_writer * restrict writer)
{
  RUNTIME_ASSERT(writer->string == 0);

  writer->string = 1;
  indent(writer);
}

void bacon_writer_pop_bytes(bacon_writer * restrict writer)
{
  writer->string = 0;
}

//
// set
//

void bacon_writer_push_set(bacon_writer * restrict writer)
{
  RUNTIME_ASSERT(writer->level != (sizeof(writer->levels) / sizeof(writer->levels[0])));

  indent(writer);
  writer->nl_off += narrator_sayc(writer->N, '{');
  writer->levels[writer->level++] = '}';
}

void bacon_writer_pop_set(bacon_writer * restrict writer)
{
  pop(writer);
}

//
// list
//

void bacon_writer_push_list(bacon_writer * restrict writer)
{
  RUNTIME_ASSERT(writer->level != (sizeof(writer->levels) / sizeof(writer->levels[0])));

  indent(writer);
  writer->nl_off += narrator_sayc(writer->N, '[');
  writer->levels[writer->level++] = ']';
}

void bacon_writer_pop_list(bacon_writer * restrict writer)
{
  pop(writer);
}

//
// mapping
//

void bacon_writer_push_mapping(bacon_writer * const restrict writer)
{
  RUNTIME_ASSERT(writer->mapping >= 0);
  writer->mapping++;
}

void bacon_writer_pop_mapping(bacon_writer * const restrict writer)
{
  RUNTIME_ASSERT((writer->mapping % 2) == 0);
}

void bacon_writer_mapping_string_string(bacon_writer * const restrict writer, const char * const restrict k, const char * const restrict s)
{
  bacon_writer_push_mapping(writer);
  bacon_writer_string(writer, k);
  bacon_writer_string(writer, s);
  bacon_writer_pop_mapping(writer);
}

void bacon_writer_mapping_string_bytes(bacon_writer * const restrict writer, const char * const restrict k, const char * const restrict s, size_t len)
{
  bacon_writer_push_mapping(writer);
  bacon_writer_string(writer, k);
  bacon_writer_bytes(writer, s, len);
  bacon_writer_pop_mapping(writer);
}

void bacon_writer_mapping_string_uint(bacon_writer * const restrict writer, const char * const restrict k, uint64_t u)
{
  bacon_writer_push_mapping(writer);
  bacon_writer_string(writer, k);
  bacon_writer_uint(writer, u);
  bacon_writer_pop_mapping(writer);
}

void bacon_writer_mapping_string_int(bacon_writer * const restrict writer, const char * const restrict k, int64_t i)
{
  bacon_writer_push_mapping(writer);
  bacon_writer_string(writer, k);
  bacon_writer_int(writer, i);
  bacon_writer_pop_mapping(writer);
}

void bacon_writer_mapping_string_bool(bacon_writer * const restrict writer, const char * const restrict k, bool b)
{
  bacon_writer_push_mapping(writer);
  bacon_writer_string(writer, k);
  bacon_writer_bool(writer, b);
  bacon_writer_pop_mapping(writer);
}

void bacon_writer_mapping_string_float(bacon_writer * const restrict writer, const char * const restrict k, double f)
{
  bacon_writer_push_mapping(writer);
  bacon_writer_string(writer, k);
  bacon_writer_float(writer, f);
  bacon_writer_pop_mapping(writer);
}

//
// comment
//

static void comment_spacing(bacon_writer * const restrict writer)
{
  int x;

  if(writer->nl_off > 60) {
    x = 0;
  } else {
    x = 60 - writer->nl_off;
  }

  narrator_sayf(writer->N, "%*s # ", x, "");
}

void bacon_writer_commentw(bacon_writer * const restrict writer, const char *b, size_t bz)
{
  narrator_says(writer->N, " # ");
  narrator_sayw(writer->N, b, bz);
}

void bacon_writer_comments(bacon_writer * const restrict writer, const char *s)
{
  comment_spacing(writer);
  narrator_says(writer->N, s);
}

void bacon_writer_commentf(bacon_writer * const restrict writer, const char *fmt, ...)
{
  va_list va;

  va_start(va, fmt);
  bacon_writer_commentvf(writer, fmt, va);
  va_end(va);
}

void bacon_writer_commentvf(bacon_writer * const restrict writer, const char *fmt, va_list va)
{
  comment_spacing(writer);
  narrator_sayvf(writer->N, fmt, va);
}
