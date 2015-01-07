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

xunit_unit xunit = {
	.tests = (xunit_test*[]) {

/* p : SELECTION_ACTIVATE | WINDOWS_STAGE */

		// off/len of zero : entire string
		  (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.b.c", "foo.a.b.c", 0 }
				, .xsfm = "lx/a"
				, .final = (char*[]) { "foo.a", 0 }
			}}
		, (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.b.c", "foo.a.b.c", 0 }
				, .xsfm = "lx/b.c"
				, .final = (char*[]) { "foo.b.c", "foo.a.b.c", 0 }
		  }}
		, (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.b.c", "foo.a.b.c", 0 }
				, .xsfm = "lxf/b.c"
				, .final = (char*[]) { "foo.b.c", 0 }
		  }}
		, 0
	}
};
