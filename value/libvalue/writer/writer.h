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

#ifndef VALUE_WRITER_H
#define VALUE_WRITER_H

#include "xapi.h"
#include "types.h"

struct narrator;
struct value;

#define VALUE_WRITER_MAX_DEPTH 256

typedef struct value_writer {
  struct narrator * N;

  int scalar;       // nesting of scalar calls
  bool noquote;
  bool any;         // whether anything has been written
  int mapping;      // true while a map entry is being written across calls

  uint8_t level;
  char levels[VALUE_WRITER_MAX_DEPTH];
} value_writer;

//
// create/free
//

xapi value_writer_create(value_writer ** restrict writer)
  __attribute__((nonnull));

xapi value_writer_xfree(value_writer * restrict writer);
xapi value_writer_ixfree(value_writer ** restrict writer)
  __attribute__((nonnull));

//
// init/destroy
//

void value_writer_init(value_writer * restrict writer)
  __attribute__((nonnull));

xapi value_writer_destroy(value_writer * restrict writer)
  __attribute__((nonnull));

/// value_writer_open
//
// SUMMARY
//  prepare to write a value
//
// PARAMETERS
//  writer    - value writer
//  N         - narrator to write to
//
xapi value_writer_open(value_writer * const restrict writer, struct narrator * const restrict N)
  __attribute__((nonnull));

xapi value_writer_close(value_writer * const restrict writer)
  __attribute__((nonnull));

xapi value_writer_value(value_writer * const restrict writer, const struct value * restrict val)
  __attribute__((nonnull));

//
// writer/primitive
//

xapi value_writer_char(value_writer * const restrict writer, char c)
  __attribute__((nonnull));

xapi value_writer_int(value_writer * const restrict writer, int64_t u)
  __attribute__((nonnull));

xapi value_writer_uint(value_writer * const restrict writer, uint64_t u)
  __attribute__((nonnull));

xapi value_writer_bool(value_writer * const restrict writer, bool b)
  __attribute__((nonnull));

xapi value_writer_float(value_writer * const restrict writer, double f)
  __attribute__((nonnull));

xapi value_writer_string(value_writer * const restrict writer, const char * const restrict s)
  __attribute__((nonnull(1)));

xapi value_writer_variable(value_writer * const restrict writer, const char * const restrict name, size_t len)
  __attribute__((nonnull));

xapi value_writer_bytes(value_writer * const restrict writer, const char * const restrict s, size_t len)
  __attribute__((nonnull(1)));

xapi value_writer_bytes_start(value_writer * const restrict writer, struct narrator ** restrict N)
  __attribute__((nonnull));

xapi value_writer_bytes_finish(value_writer * const restrict writer)
  __attribute__((nonnull));

//
// writer/set
//

xapi value_writer_push_set(value_writer * const restrict writer)
  __attribute__((nonnull));

xapi value_writer_pop_set(value_writer * const restrict writer)
  __attribute__((nonnull));

//
// writer/list
//

xapi value_writer_push_list(value_writer * const restrict writer)
  __attribute__((nonnull));

xapi value_writer_pop_list(value_writer * const restrict writer)
  __attribute__((nonnull));

//
// writer/mapping
//

xapi value_writer_push_mapping(value_writer * const restrict writer)
  __attribute__((nonnull));

xapi value_writer_pop_mapping(value_writer * const restrict writer)
  __attribute__((nonnull));

xapi value_writer_mapping_string_char(value_writer * const restrict writer, const char * const restrict k, char c)
  __attribute__((nonnull));

xapi value_writer_mapping_string_int(value_writer * const restrict writer, const char * const restrict k, int64_t i)
  __attribute__((nonnull));

xapi value_writer_mapping_string_uint(value_writer * const restrict writer, const char * const restrict k, uint64_t u)
  __attribute__((nonnull));

xapi value_writer_mapping_string_bool(value_writer * const restrict writer, const char * const restrict k, bool b)
  __attribute__((nonnull));

xapi value_writer_mapping_string_float(value_writer * const restrict writer, const char * const restrict k, double f)
  __attribute__((nonnull));

xapi value_writer_mapping_string_string(value_writer * const restrict writer, const char * const restrict k, const char * const restrict s)
  __attribute__((nonnull));

xapi value_writer_mapping_string_bytes(value_writer * const restrict writer, const char * const restrict k, const char * const restrict s, size_t len)
  __attribute__((nonnull));

#endif
