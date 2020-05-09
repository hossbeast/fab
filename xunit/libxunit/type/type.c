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

#include <stddef.h>
#include <stdarg.h>
#include <inttypes.h>

#include "xapi.h"
#include "type.h"

#include "internal.h"
#include "type.internal.h"

#include "zbuffer.h"

static int printable(const char * s, size_t l)
{
  int x;
  for(x = 0; x < l; x++)
  {
    if(   (s[x] < 'a' || s[x] > 'z')
       && (s[x] < 'A' || s[x] > 'Z')
       && (s[x] < '0' || s[x] > '9')
       && (s[x] < 0x20 || s[x] > 0x7e))
    {
      break;
    }
  }

  return x == l;
}

//
// types
//

static void string_unpack(va_list va, xunit_arg * a)
{
  a->p = va_arg(va, char*);
}

static int string_compare(xunit_arg * A, xunit_arg * B)
{
  if(!A->s || !B->s)
  {
    return INTCMP(A->s, B->s);
  }

  return strcmp(A->s, B->s);
}

static void string_info_push(const char * const restrict name, xunit_arg * a)
{
  if(a->s)
    xapi_info_pushs(name, a->s);
  else
    xapi_info_pushs(name, "(null)");
}

static void stringl_unpack(va_list va, xunit_arg * a)
{
  a->p = va_arg(va, void*);
  a->l = va_arg(va, size_t);
}

static int stringl_compare(xunit_arg * A, xunit_arg * B)
{
  if(A->l == 0 && B->l == 0)
  {
    return 0;
  }
  else if(A->p && B->p)
  {
    int d;
    if((d = A->l - B->l))
      return d;

    return memcmp(A->p, B->p, A->l);
  }
  else if(A->p)
  {
    return 1;
  }
  else if(B->p)
  {
    return -1;
  }

  return 0;
}

static void stringl_info_push(const char * const restrict name, xunit_arg * a)
{
  if(a->p == 0)
  {
    xapi_info_pushs(name, "(null)");
  }
  else
  {
    xapi_info_pushf(name, "%.*s", (int)a->l, (char*)a->p);
  }
}

static void buffer_unpack(va_list va, xunit_arg * a)
{
  a->p = va_arg(va, void*);
  a->l = va_arg(va, size_t);
}

static int buffer_compare(xunit_arg * A, xunit_arg * B)
{
  if(A->l == 0 && B->l == 0)
  {
    return 0;
  }
  else if(A->p && B->p)
  {
    int d;
    if((d = A->l - B->l))
      return d;

    return memcmp(A->p, B->p, A->l);
  }
  else if(A->p)
  {
    return 1;
  }
  else if(B->p)
  {
    return -1;
  }

  return 0;
}

static void buffer_info_push(const char * const restrict name, xunit_arg * a)
{
  if(a->p == 0)
  {
    xapi_info_pushs(name, "(null)");
  }
  else if(printable(a->p, a->l))
  {
    xapi_info_pushf(name, "%.*s", (int)a->l, (char*)a->p);
  }
  else
  {
    char space[128];
    char * buf = space;
    size_t bufz = sizeof(space);
    size_t bufo = 0;

    bufo += znloads(buf + bufo, bufz - bufo, "0x");
    int x;
    for(x = 0; x < a->l; x++)
      bufo += znloadf(buf + bufo, bufz - bufo, "%02hhx", ((char*)a->p)[x]);

    xapi_info_pushs(name, space);
  }
}

static void int_unpack(va_list va, xunit_arg * a)
{
  a->d = va_arg(va, int);
}

static int int_compare(xunit_arg * A, xunit_arg * B)
{
  return A->d - B->d;
}

static void int_info_push(const char * const restrict name, xunit_arg * a)
{
  xapi_info_pushf(name, "%d", a->d);
}

static void char_unpack(va_list va, xunit_arg * a)
{
  a->c = va_arg(va, int);
}

static int char_compare(xunit_arg * A, xunit_arg * B)
{
  return A->c - B->c;
}

static void char_info_push(const char * const restrict name, xunit_arg * a)
{
  if(a->c < 0x20 || a->c > 0x7e)
  {
    xapi_info_pushf(name, "0x%02hhx", a->c);
  }
  else
  {
    xapi_info_pushf(name, "'%c'", a->c);
  }
}

static void float_unpack(va_list va, xunit_arg * a)
{
  a->f = (float)va_arg(va, double);
}

static int float_compare(xunit_arg * A, xunit_arg * B)
{
  if(A->f < B->f)
    return -1;
  else if(A->f > B->f)
    return 1;

  return 0;
}

static void float_info_push(const char * const restrict name, xunit_arg * a)
{
  xapi_info_pushf(name, "%f", a->f);
}

static void int32_unpack(va_list va, xunit_arg * a)
{
  a->i32 = va_arg(va, int32_t);
}

static int int32_compare(xunit_arg * A, xunit_arg * B)
{
  return INTCMP(A->i32, B->i32);
}

static void int32_info_push(const char * const restrict name, xunit_arg * a)
{
  xapi_info_pushf(name, "%"PRIi32, a->i32);
}

static void uint32_unpack(va_list va, xunit_arg * a)
{
  a->u32 = va_arg(va, uint32_t);
}

static int uint32_compare(xunit_arg * A, xunit_arg * B)
{
  return INTCMP(A->u32, B->u32);
}

static void uint32_info_push(const char * const restrict name, xunit_arg * a)
{
  xapi_info_pushf(name, "%"PRIu32, a->u32);
}

static void int64_unpack(va_list va, xunit_arg * a)
{
  a->i64 = va_arg(va, int64_t);
}

static int int64_compare(xunit_arg * A, xunit_arg * B)
{
  return INTCMP(A->i64, B->i64);
}

static void int64_info_push(const char * const restrict name, xunit_arg * a)
{
  xapi_info_pushf(name, "%"PRIi64, a->i64);
}

static void uint64_unpack(va_list va, xunit_arg * a)
{
  a->u64 = va_arg(va, uint64_t);
}

static int uint64_compare(xunit_arg * A, xunit_arg * B)
{
  return INTCMP(A->u64, B->u64);
}

static void uint64_info_push(const char * const restrict name, xunit_arg * a)
{
  xapi_info_pushf(name, "%"PRIu64, a->u64);
}

static void size_t_unpack(va_list va, xunit_arg * a)
{
  a->zu = va_arg(va, size_t);
}

static int size_t_compare(xunit_arg * A, xunit_arg * B)
{
  return INTCMP(A->zu, B->zu);
}

static void size_t_info_push(const char * const restrict name, xunit_arg * a)
{
  xapi_info_pushf(name, "%zu", a->zu);
}

static void bool_unpack(va_list va, xunit_arg * a)
{
  a->b = va_arg(va, int);
}

static int bool_compare(xunit_arg * A, xunit_arg * B)
{
  return A->b != B->b;
}

static void bool_info_push(const char * const restrict name, xunit_arg * a)
{
  xapi_info_pushs(name, a->b ? "true" : "false");
}

static void xapi_unpack(va_list va, xunit_arg * a)
{
  a->e = va_arg(va, xapi);
}

static int xapi_compare(xunit_arg * A, xunit_arg * B)
{
  return A->e - B->e;
}

static void xapi_info_push(const char * const restrict name, xunit_arg * a)
{
  xapi_info_pushf(
      name
    , "%s_%s (0x%04x+%d)"
    , xapi_exit_errtab_name(a->e)
    , xapi_exit_errname(a->e)
    , xapi_exit_errtab_tag(a->e)
    , xapi_exit_errcode(a->e)
  );
}

static void pointer_unpack(va_list va, xunit_arg * a)
{
  a->p = va_arg(va, void*);
}

static int pointer_compare(xunit_arg * A, xunit_arg * B)
{
  ptrdiff_t delta = A->p - B->p;
  if(delta > 0)
    return 1;
  if(delta < 0)
    return -1;
  return 0;
}

static void pointer_info_push(const char * const restrict name, xunit_arg * a)
{
  xapi_info_pushf(name, "%p", a->p);
}

//
// API
//

APIDATA xunit_type * xunit_string = (xunit_type[]) {{
    xu_unpack : string_unpack
  , xu_compare : string_compare
  , xu_info_push : string_info_push
}};

APIDATA xunit_type * xunit_stringl = (xunit_type[]) {{
    xu_unpack : stringl_unpack
  , xu_compare : stringl_compare
  , xu_info_push : stringl_info_push
}};

APIDATA xunit_type * xunit_buffer = (xunit_type[]) {{
    xu_unpack : buffer_unpack
  , xu_compare : buffer_compare
  , xu_info_push : buffer_info_push
}};

APIDATA xunit_type * xunit_int = (xunit_type[]) {{
    xu_unpack : int_unpack
  , xu_compare : int_compare
  , xu_info_push : int_info_push
}};

APIDATA xunit_type * xunit_char = (xunit_type[]) {{
    xu_unpack : char_unpack
  , xu_compare : char_compare
  , xu_info_push : char_info_push
}};

APIDATA xunit_type * xunit_xapi = (xunit_type[]) {{
    xu_unpack : xapi_unpack
  , xu_compare : xapi_compare
  , xu_info_push : xapi_info_push
}};

APIDATA xunit_type * xunit_int32 = (xunit_type[]) {{
    xu_unpack : int32_unpack
  , xu_compare : int32_compare
  , xu_info_push : int32_info_push
}};

APIDATA xunit_type * xunit_uint32 = (xunit_type[]) {{
    xu_unpack : uint32_unpack
  , xu_compare : uint32_compare
  , xu_info_push : uint32_info_push
}};

APIDATA xunit_type * xunit_int64 = (xunit_type[]) {{
    xu_unpack : int64_unpack
  , xu_compare : int64_compare
  , xu_info_push : int64_info_push
}};

APIDATA xunit_type * xunit_uint64 = (xunit_type[]) {{
    xu_unpack : uint64_unpack
  , xu_compare : uint64_compare
  , xu_info_push : uint64_info_push
}};

APIDATA xunit_type * xunit_size_t = (xunit_type[]) {{
    xu_unpack : size_t_unpack
  , xu_compare : size_t_compare
  , xu_info_push : size_t_info_push
}};

APIDATA xunit_type * xunit_float = (xunit_type[]) {{
    xu_unpack : float_unpack
  , xu_compare : float_compare
  , xu_info_push : float_info_push
}};

APIDATA xunit_type * xunit_bool = (xunit_type[]) {{
    xu_unpack : bool_unpack
  , xu_compare : bool_compare
  , xu_info_push : bool_info_push
}};

APIDATA xunit_type * xunit_pointer = (xunit_type[]) {{
    xu_unpack : pointer_unpack
  , xu_compare : pointer_compare
  , xu_info_push : pointer_info_push
}};
