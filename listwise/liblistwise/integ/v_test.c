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
    .setup = listwise_test_setup
  , .release = listwise_test_release
	, .tests = (xunit_test*[]) {

/* s : SELECTION_ACTIVATE */
		    (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
				, .xsfm = "l/. y v"
				, .final = (char*[]) { "foo", 0 }
		  }}

/* wv : WINDOWS_ACTIVATE */
		  , (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
				, .xsfm = "l/./g y wv"
				, .final = (char*[]) { "fooa", "fooab", "fooabc", 0 }
		  }}
		  , (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
				, .xsfm = "l/./g y wv sz"
				, .final = (char*[]) { "foo", "fooa", "fooab", "fooabc", 0 }
		  }}
		  , (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
				, .xsfm = "l/. y p/-1 wv"
				, .final = (char*[]) { "foo.", "foo.", "foo.", 0 }
		  }}
		  , (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
				, .xsfm = "l/. y p/0/1 wv"
				, .final = (char*[]) { ".a", ".a.b", ".a.b.c", 0 }
		  }}

		  , (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
				, .xsfm = "wv"		// { all } -> none
				, .final = (char*[]) { 0 }
		  }}
		  , (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
				, .xsfm = "wz wv"	// { all } -> all -> none 
				, .final = (char*[]) { 0 }
		  }}
		  , (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
				, .xsfm = "wz wv wv"	// { all } -> all -> none -> all
				, .final = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
		  }}
		  , (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
				, .xsfm = "wz wv wv wv"	// { all } -> all -> none -> all -> none
				, .final = (char*[]) { 0 }
		  }}
		, 0
	}
};
