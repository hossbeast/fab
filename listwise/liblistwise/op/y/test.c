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

/* sx : WINDOWS_STAGE | SELECTION_STAGE */

			/* sy : selections */
		  (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
				, .xsfm = "sx/z sy"
				, .final = (char*[]) { "foo.z", "foo.a.z", "foo.a.b.z", 0 }
		  }}

			/* wy : windows */
		, (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
				, .xsfm = "sx/z wy"
				, .final = (char*[]) { "foo", "z", "z", "z", 0 }
		  }}

			/* selections and windows */
		, (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
				, .xsfm = "sx/z y"
				, .final = (char*[]) { "z", "z", "z", 0 }
		  }}

/* sy WINDOWS_ACTIVATE | SELECTION_STAGE */
/* sy WINDOWS_STAGE | SELECTION_ACTIVATE */

/*
		, (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
				, .xsfm = "sx/b/z"
				, .final = (char*[]) { "foo", "foo.a", "foo.a.z", "foo.a.b.c", 0 }
		  }}
		, (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
				, .xsfm = "sxf/z"
				, .final = (char*[]) { "foo", "foo.z", "foo.z", "foo.z", 0 }
		  }}
		, (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", "foo.a.b.c.d", "foo.b.c", 0 }
				, .xsfm = "sxf/b.c/z"
				, .final = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", "foo.a.b.c.d", "foo.z", 0 }
		  }}
		, (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", "foo.a.b.c.d", "foo.b.c", 0 }
				, .xsfm = "sxf/b.c/z"
				, .final = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", "foo.a.b.c.d", "foo.z", 0 }
		  }}
*/
		, 0
	}
};
