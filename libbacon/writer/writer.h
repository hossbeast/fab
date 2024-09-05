/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct narrator;
struct bacon_value;

#define VALUE_WRITER_MAX_DEPTH 256

typedef struct bacon_writer {
  struct narrator * N;
  bool singleline;

  bool any;         // whether anything has been written
  int mapping;      // true while a map entry is being written across calls
  int string;       // true while a string entry is being written across calls;

  off_t nl_off;     // offset of the most recent newline

  uint8_t level;
  char levels[VALUE_WRITER_MAX_DEPTH];
} bacon_writer;

//
// create/free
//

void bacon_writer_create(bacon_writer ** restrict writer)
  __attribute__((nonnull));

void bacon_writer_free(bacon_writer * restrict writer);
void bacon_writer_ifree(bacon_writer ** restrict writer)
  __attribute__((nonnull));

//
// init/destroy
//

void bacon_writer_init(bacon_writer * restrict writer)
  __attribute__((nonnull));

void bacon_writer_destroy(bacon_writer * restrict writer)
  __attribute__((nonnull));

/// bacon_writer_open
//
// SUMMARY
//  prepare to write a value
//
// PARAMETERS
//  writer    - value writer
//  N         - narrator to write to
//
bacon_writer *bacon_writer_open(bacon_writer * restrict writer, struct narrator * restrict N)
  __attribute__((nonnull));

void bacon_writer_close(bacon_writer * restrict writer)
  __attribute__((nonnull));

void bacon_writer_value(bacon_writer * const restrict writer, const struct bacon_value * restrict val)
  __attribute__((nonnull));

//
// writer/primitive
//

void bacon_writer_char(bacon_writer * const restrict writer, char c)
  __attribute__((nonnull));

void bacon_writer_int(bacon_writer * const restrict writer, int64_t u)
  __attribute__((nonnull));

void bacon_writer_uint(bacon_writer * const restrict writer, uint64_t u)
  __attribute__((nonnull));

void bacon_writer_hex(bacon_writer * const restrict writer, uint64_t u)
  __attribute__((nonnull));

void bacon_writer_bool(bacon_writer * const restrict writer, bool b)
  __attribute__((nonnull));

void bacon_writer_float(bacon_writer * const restrict writer, double f)
  __attribute__((nonnull));

void bacon_writer_string(bacon_writer * const restrict writer, const char * restrict s)
  __attribute__((nonnull(1)));

void bacon_writer_bytes(bacon_writer * const restrict writer, const char * restrict s, size_t len)
  __attribute__((nonnull(1)));

void bacon_writef(bacon_writer * restrict writer, const char * restrict fmt, ...)
  __attribute__((nonnull(1, 2)))
  __attribute__((format(printf, 2, 3)));

void bacon_writer_variable(bacon_writer * restrict writer, const char * restrict name, size_t len)
  __attribute__((nonnull));

//
// writer/string
//

void bacon_writer_push_string(bacon_writer * const restrict writer)
  __attribute__((nonnull));

void bacon_writer_pop_string(bacon_writer * const restrict writer)
  __attribute__((nonnull));

void bacon_writer_push_bytes(bacon_writer * const restrict writer)
  __attribute__((nonnull));

void bacon_writer_pop_bytes(bacon_writer * const restrict writer)
  __attribute__((nonnull));


//
// writer/set
//

void bacon_writer_push_set(bacon_writer * const restrict writer)
  __attribute__((nonnull));

void bacon_writer_pop_set(bacon_writer * const restrict writer)
  __attribute__((nonnull));

//
// writer/list
//

void bacon_writer_push_list(bacon_writer * const restrict writer)
  __attribute__((nonnull));

void bacon_writer_pop_list(bacon_writer * const restrict writer)
  __attribute__((nonnull));

//
// writer/mapping
//

void bacon_writer_push_mapping(bacon_writer * const restrict writer)
  __attribute__((nonnull));

void bacon_writer_pop_mapping(bacon_writer * const restrict writer)
  __attribute__((nonnull));

void bacon_writer_mapping_string_char(bacon_writer * const restrict writer, const char * const restrict k, char c)
  __attribute__((nonnull));

void bacon_writer_mapping_string_int(bacon_writer * const restrict writer, const char * const restrict k, int64_t i)
  __attribute__((nonnull));

void bacon_writer_mapping_string_uint(bacon_writer * const restrict writer, const char * const restrict k, uint64_t u)
  __attribute__((nonnull));

void bacon_writer_mapping_string_bool(bacon_writer * const restrict writer, const char * const restrict k, bool b)
  __attribute__((nonnull));

void bacon_writer_mapping_string_float(bacon_writer * const restrict writer, const char * const restrict k, double f)
  __attribute__((nonnull));

void bacon_writer_mapping_string_string(bacon_writer * const restrict writer, const char * const restrict k, const char * const restrict s)
  __attribute__((nonnull));

void bacon_writer_mapping_string_bytes(bacon_writer * const restrict writer, const char * const restrict k, const char * const restrict s, size_t len)
  __attribute__((nonnull));

/* write value-string escaped text */
size_t bacon_value_string_zcatw(void * restrict dst, size_t sz, const void * restrict b, size_t bz)
  __attribute__((nonnull));

#define bacon_value_string_zcats(dst, sz, s) bacon_value_string_zcatw(dst, sz, s, strlen(s))
#define bacon_value_string_zcatc(dst, sz, c) bacon_value_string_zcatw(dst, sz, (char[]) { c }, 1)

size_t bacon_value_string_zcatvf(void * restrict dst, size_t sz, const char * restrict fmt, va_list va)
  __attribute__((nonnull));

//
// comments
//

void bacon_writer_commentf(bacon_writer * const restrict writer, const char *fmt, ...)
  __attribute__((nonnull(1, 2)))
  __attribute__((format(printf, 2, 3)));

void bacon_writer_commentvf(bacon_writer * const restrict writer, const char *fmt, va_list va)
  __attribute__((nonnull));

void bacon_writer_comments(bacon_writer * const restrict writer, const char *s)
  __attribute__((nonnull));

void bacon_writer_commentw(bacon_writer * const restrict writer, const char *b, size_t bz)
  __attribute__((nonnull(1)));

//
// fixed-length
//

void bacon_writer_hex64(bacon_writer * const restrict writer, uint64_t u)
  __attribute__((nonnull));
