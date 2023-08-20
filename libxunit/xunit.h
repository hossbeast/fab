/* SPDX-License-Identifier: GPL-3.0 */
/* Copyright (c) 2012-2023 Todd Freed <todd.freed@gmail.com> */

#pragma once

#include "types.h"

#ifndef TEST_TYPE
# define TEST_TYPE struct xunit_test
#endif
#ifndef UNIT_TYPE
# define UNIT_TYPE struct xunit_unit
#endif

struct xunit_unit;
struct xunit_test;

typedef void (*xunit_unit_setup)(UNIT_TYPE*);
typedef void (*xunit_unit_teardown)(UNIT_TYPE*);
typedef void (*xunit_unit_cleanup)(UNIT_TYPE*);
typedef void (*xunit_test_entry)(TEST_TYPE*);

/// xunit_unit
//
// SUMMARY
//  each test module exports a single xunit_unit named xunit
//
typedef struct xunit_unit
{
  struct xunit_unit *next;
  struct xunit_unit *prev;

  xunit_unit_setup      xu_setup;
  xunit_unit_teardown   xu_teardown;
  xunit_unit_cleanup    xu_cleanup;
  xunit_test_entry      xu_entry;
  void *                xu_tests;    // sentinel-terminated list of pointers to tests of type compatible with xunit_test
} xunit_unit;

/// xunit_test
//
// SUMMARY
//  a single test
//
#define XUNITDEF {                      \
  struct xunit_unit * xu_unit;          \
  char *              xu_name;          \
  xunit_test_entry    xu_entry;         \
  uint16_t            xu_index;         \
                                        \
  /* controlling which tests to run */  \
  int                 xu_weight;        \
}

typedef struct xunit_test XUNITDEF xunit_test;
#define XUNITTEST union { xunit_test xu ; struct XUNITDEF; };

void xunit_register_unit(xunit_unit * restrict unit)
  __attribute__((nonnull));

#define XUNIT_UNIT(unit)                                \
  static void __attribute__((constructor)) xunit_register()  \
  {                                                     \
    xunit_register_unit(&unit);                         \
  }
