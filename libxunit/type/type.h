/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

/*

MODULE
 xunit/type

SUMMARY

*/

#include "types.h"

typedef union xunit_arg
{
  struct {        // xunit_buffer
    void * p;     // xunit_pointer
    size_t l;
  };

  char c;         // xunit_char
  int d;          // xunit_int
  char * s;       // xunit_string
  int32_t i32;    // xunit_int32
  uint32_t u32;   // xunit_uint32
  int64_t i64;    // xunit_int64
  uint64_t u64;   // xunit_uint64
  size_t zu;      // xunit_size_t
  float f;        // xunit_float
  bool b;         // xunit_bool

  void * udata;
} xunit_arg;

typedef struct xunit_type
{
  void (*xu_unpack)(va_list, xunit_arg *);
  int (*xu_compare)(xunit_arg *, xunit_arg *);
  void (*xu_info_push)(const char *, xunit_arg *);
} xunit_type;
