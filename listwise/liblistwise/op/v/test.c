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

/* sv : SELECTION_ACTIVATE */
		    (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
				, .xsfm = "l/. y sv"
				, .final = (char*[]) { "foo", 0 }
		  }}

/* wv : WINDOWS_ACTIVATE */
		  , (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
				, .xsfm = "l/./g y wv"
				, .final = (char*[]) { "fooa", "fooab", "fooabc", 0 }
		  }}

/* v : WINDOWS_ACTIVATE | SELECTION_ACTIVATE */
		  , (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
				, .xsfm = "l/. y v"
				, .final = (char*[]) { "foo", 0 }
		  }}

		, 0
	}
};
