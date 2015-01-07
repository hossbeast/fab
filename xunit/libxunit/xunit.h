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

#ifndef TEST_TYPE
# define TEST_TYPE struct xunit_test
#endif
#ifndef UNIT_TYPE
# define UNIT_TYPE struct xunit_unit
#endif

struct xunit_unit;
struct xunit_test;

// xapi
typedef xapi (*xunit_unit_setup)(const UNIT_TYPE*);
typedef xapi (*xunit_unit_teardown)(const UNIT_TYPE*);
typedef xapi (*xunit_test_entry)(const TEST_TYPE*);

typedef struct xunit_test
{
	struct xunit_unit *	unit;
	char *							name;
	xunit_test_entry		entry;
} xunit_test;

typedef struct xunit_unit
{
	xunit_unit_setup		setup;
	xunit_unit_teardown	teardown;
	xunit_test **				tests;		// sentinel-terminated list of pointers to tests
} xunit_unit;

#include "xunit/XUNIT.errtab.h"
#undef perrtab
#define perrtab perrtab_XUNIT

#include "xunit/logs.h"

#endif
