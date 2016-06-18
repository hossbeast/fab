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

#ifndef _XUNIT_H
#define _XUNIT_H

#include "xapi.h"

/// xunit_setup
//
// SUMMARY
//  initialize the library
//
xapi xunit_load();

/// xunit_unload
//
// SUMMARY
//  release the library
//
xapi xunit_unload();

#ifndef TEST_TYPE
# define TEST_TYPE struct xunit_test
#endif
#ifndef UNIT_TYPE
# define UNIT_TYPE struct xunit_unit
#endif

struct xunit_unit;
struct xunit_test;

typedef xapi (*xunit_unit_setup)(UNIT_TYPE*);
typedef void (*xunit_unit_teardown)(UNIT_TYPE*);
typedef xapi (*xunit_unit_release)(UNIT_TYPE*);
typedef xapi (*xunit_test_entry)(TEST_TYPE*);

/// xunit_unit
//
// SUMMARY
//  each test module exports a single xunit_unit named xunit
//
typedef struct xunit_unit
{
  xunit_unit_setup      setup;
  xunit_unit_teardown   teardown;
  xunit_unit_release    release;
  struct xunit_test **  tests;    // sentinel-terminated list of pointers to tests
} xunit_unit;

/// xunit_test
//
// SUMMARY
//  a single test
//
typedef struct xunit_test
{
  struct xunit_unit * unit;
  char *              name;
  xunit_test_entry    entry;
} xunit_test;

#endif
