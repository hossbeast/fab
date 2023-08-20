/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct narrator;
struct value;

#define VALUE_WRITER_MAX_DEPTH 256

typedef struct value_writer {
  struct narrator * N;

  bool any;         // whether anything has been written
  int mapping;      // true while a map entry is being written across calls

  uint8_t level;
  char levels[VALUE_WRITER_MAX_DEPTH];
} value_writer;

//
// create/free
//

void value_writer_create(value_writer ** restrict writer)
  __attribute__((nonnull));

void value_writer_xfree(value_writer * restrict writer);
void value_writer_ixfree(value_writer ** restrict writer)
  __attribute__((nonnull));

//
// init/destroy
//

void value_writer_init(value_writer * restrict writer)
  __attribute__((nonnull));

void value_writer_destroy(value_writer * restrict writer)
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
void value_writer_open(value_writer * const restrict writer, struct narrator * const restrict N)
  __attribute__((nonnull));

void value_writer_close(value_writer * const restrict writer)
  __attribute__((nonnull));

void value_writer_value(value_writer * const restrict writer, const struct value * restrict val)
  __attribute__((nonnull));

//
// writer/primitive
//

void value_writer_char(value_writer * const restrict writer, char c)
  __attribute__((nonnull));

void value_writer_int(value_writer * const restrict writer, int64_t u)
  __attribute__((nonnull));

void value_writer_uint(value_writer * const restrict writer, uint64_t u)
  __attribute__((nonnull));

void value_writer_bool(value_writer * const restrict writer, bool b)
  __attribute__((nonnull));

void value_writer_float(value_writer * const restrict writer, double f)
  __attribute__((nonnull));

void value_writer_string(value_writer * const restrict writer, const char * const restrict s)
  __attribute__((nonnull(1)));

void value_writer_bytes(value_writer * const restrict writer, const char * const restrict s, size_t len)
  __attribute__((nonnull(1)));

void value_writer_variable(value_writer * const restrict writer, const char * const restrict name, size_t len)
  __attribute__((nonnull));

//
// writer/set
//

void value_writer_push_set(value_writer * const restrict writer)
  __attribute__((nonnull));

void value_writer_pop_set(value_writer * const restrict writer)
  __attribute__((nonnull));

//
// writer/list
//

void value_writer_push_list(value_writer * const restrict writer)
  __attribute__((nonnull));

void value_writer_pop_list(value_writer * const restrict writer)
  __attribute__((nonnull));

//
// writer/mapping
//

void value_writer_push_mapping(value_writer * const restrict writer)
  __attribute__((nonnull));

void value_writer_pop_mapping(value_writer * const restrict writer)
  __attribute__((nonnull));

void value_writer_mapping_string_char(value_writer * const restrict writer, const char * const restrict k, char c)
  __attribute__((nonnull));

void value_writer_mapping_string_int(value_writer * const restrict writer, const char * const restrict k, int64_t i)
  __attribute__((nonnull));

void value_writer_mapping_string_uint(value_writer * const restrict writer, const char * const restrict k, uint64_t u)
  __attribute__((nonnull));

void value_writer_mapping_string_bool(value_writer * const restrict writer, const char * const restrict k, bool b)
  __attribute__((nonnull));

void value_writer_mapping_string_float(value_writer * const restrict writer, const char * const restrict k, double f)
  __attribute__((nonnull));

void value_writer_mapping_string_string(value_writer * const restrict writer, const char * const restrict k, const char * const restrict s)
  __attribute__((nonnull));

void value_writer_mapping_string_bytes(value_writer * const restrict writer, const char * const restrict k, const char * const restrict s, size_t len)
  __attribute__((nonnull));
