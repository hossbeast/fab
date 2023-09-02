/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

struct bacon_writer;

typedef struct bacon_writer_vtable {
  void (*writer_open)(struct bacon_writer * restrict w);
  void (*writer_close)(struct bacon_writer * restrict w);
  void (*write_uint)(struct bacon_writer * restrict w, uint64_t u);
  void (*write_int)(struct bacon_writer * restrict w, int64_t i);
  void (*write_bool)(struct bacon_writer * restrict w, bool b);
  void (*write_float)(struct bacon_writer * restrict w, float f);
  void (*write_bytes)(struct bacon_writer * restrict w, const char * restrict s, uint16_t len);
  void (*write_string)(struct bacon_writer * restrict w, const char * restrict s);
  void (*write_char)(struct bacon_writer * restrict w, char c);
  void (*writevf)(struct bacon_writer * restrict w, const char * restrict fmt, ...);
  void (*push_string)(struct bacon_writer * restrict w);
  void (*pop_string)(struct bacon_writer * restrict w);
  void (*push_set)(struct bacon_writer * restrict w);
  void (*pop_set)(struct bacon_writer * restrict w);
  void (*push_list)(struct bacon_writer * restrict w);
  void (*pop_list)(struct bacon_writer * restrict w);
  void (*push_mapping)(struct bacon_writer * restrict w);
  void (*pop_mapping)(struct bacon_writer * restrict w);
  void (*mapping_string_string)(struct bacon_writer * restrict w, const char * k, const char * s);
  void (*mapping_string_bytes)(struct bacon_writer * restrict w, const char * k, const char * s, uint16_t len);
  void (*mapping_string_uint)(struct bacon_writer * restrict w, const char * restrict k, uint64_t u);
  void (*mapping_string_int)(struct bacon_writer * restrict w, const char * restrict k, int64_t i);
  void (*mapping_string_bool)(struct bacon_writer * restrict w, const char * restrict k, bool b);
  void (*mapping_string_float)(struct bacon_writer * restrict w, const char * restrict k, float f);
} bacon_writer_vtable;

#define VALUE_WRITER_VTABLE {                             \
  writer_open : (void*)writer_open,                       \
  writer_close : (void*)writer_close,                     \
  write_uint : (void*)write_uint,                         \
  write_int : (void*)write_int,                           \
  write_bool : (void*)write_bool,                         \
  write_float : (void*)write_float,                       \
  write_bytes : (void*)write_bytes,                       \
  write_string : (void*)write_string,                     \
  write_char : (void*)write_char,                         \
  writevf : (void*)writevf,                               \
  push_string : (void*)push_string,                       \
  pop_string : (void*)pop_string,                         \
  push_set : (void*)push_set,                             \
  pop_set : (void*)pop_set,                               \
  push_list : (void*)push_list,                           \
  pop_list : (void*)pop_list,                             \
  push_mapping : (void*)push_mapping,                     \
  pop_mapping : (void*)pop_mapping,                       \
  mapping_string_string : (void*)mapping_string_string,   \
  mapping_string_bytes : (void*)mapping_string_bytes,     \
  mapping_string_uint : (void*)mapping_string_uint,       \
  mapping_string_int : (void*)mapping_string_int,         \
  mapping_string_bool : (void*)mapping_string_bool,       \
  mapping_string_float : (void*)mapping_string_float,     \
}
