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

/* z : WINDOWS_RESET | SELECTION_RESET */

			/* selection is reset */
			/*  l : {SELECTION,WINDOWS}_RESET */
		    (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
				, .xsfm = "l/. y sz"
				, .final = (char*[]) { "foo", ".", ".", ".", 0 }
		  }}
			, (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
				, .xsfm = "l/. y wz"
				, .final = (char*[]) { "foo.a", "foo.a.b", "foo.a.b.c", 0 }
		  }}
			, (listwise_test[]){{ .entry = listwise_test_entry
				, .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
				, .xsfm = "l/. y z"
				, .final = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
		  }}

			/* windows are reset */

/* sz SELECTION_RESET */
/* wz WINDOWS_RESET */
		, 0
	}
};
