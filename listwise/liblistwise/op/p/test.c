/* Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>

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

/* p : WINDOWS_ACTIVATE | SELECTION_STAGE */

			(listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
				, .xsfm = "l/./g y p/-3"
				, .final = (char*[]) { ".", "foo.a.b", "a.b.c", 0 }
		  }}
		, (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
				, .xsfm = "l/./g y p/-3 y"
				, .final = (char*[]) { "foo.a.b", "a.b.c", 0 }
		  }}
		, (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
				, .xsfm = "l/./g y p/0"
				, .final = (char*[]) { "foo.a", "foo.a.b", "foo.a.b.c", 0 }
		  }}
		, (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
				, .xsfm = "l/./g y p/-1"
				, .final = (char*[]) { "a", "b", "c", 0 }
		  }}
		, 0
	}
};
