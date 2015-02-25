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

#include "listwise_test.h"

/* listwise expressions that are used in fabfiles */

static char ** liblistwise_listing = (char*[]) {
	  "listwise/liblistwise/exec"
	, "listwise/liblistwise/logging"
	, "listwise/liblistwise/error"
	, "listwise/liblistwise/exports"
	, "listwise/liblistwise/iterate"
	, "listwise/liblistwise/IDEAS"
	, "listwise/liblistwise/object"
	, "listwise/liblistwise/op"
	, "listwise/liblistwise/describe"
	, "listwise/liblistwise/test"
	, "listwise/liblistwise/fabfile"
	, "listwise/liblistwise/lwx"
	, "listwise/liblistwise/TODO"
	, "listwise/liblistwise/re"
	, "listwise/liblistwise/internal"
	, "listwise/liblistwise/lstack"
	, "listwise/liblistwise/listwise.h"
	, "listwise/liblistwise/listwise"
	, "listwise/liblistwise/fs"
	, "listwise/liblistwise/selection"
	, "listwise/liblistwise/operators"
	, "listwise/liblistwise/operator"
	, "listwise/liblistwise/sanity"
	, "listwise/liblistwise/liblistwise.final.so"
	, "listwise/liblistwise/listwise_test"
	, "listwise/liblistwise/generator"
	, "listwise/liblistwise/script"
	, "listwise/liblistwise/liblistwise.debug.so"
	, "listwise/liblistwise/liblistwise.devel.so"
	, "listwise/liblistwise/window"
	, "listwise/liblistwise/valgrind.supp"
	, "listwise/liblistwise/tune"
	, 0
};

static xapi setup(const xunit_unit * unit)
{
	listwise_test ** T = (void*)unit->tests;
	while(*T)
	{
		if((*T)->init == 0)
			(*T)->init = liblistwise_listing;

		T++;
	}

	finally : coda;
}

xunit_unit xunit = {
	  .setup = setup
	, .tests = (xunit_test*[]) {
		  (listwise_test[]){{ .entry = listwise_test_entry
				, .xsfm = "?{ cons{?, l,/test.c v}"
				, .final = (char*[]) { "?, l,/test.c v", 0 }
		  }}
		, (listwise_test[]){{ .entry = listwise_test_entry
				, .xsfm = "m/op$ o l/test o l/fab"
				, .final = (char*[]) { "listwise/liblistwise/op", "listwise/liblistwise/test", "listwise/liblistwise/fabfile", "listwise/liblistwise/listwise_test", 0 }
		  }}
		, (listwise_test[]){{ .entry = listwise_test_entry
				, .xsfm = "sxf/final.pic.o sy uu"
				, .final = (char*[]) { "listwise/liblistwise/listwise.final.pic.o", "listwise/liblistwise/liblistwise.final.pic.o", "listwise/liblistwise/valgrind.final.pic.o", 0 }
		  }}
		, 0
	}
};
