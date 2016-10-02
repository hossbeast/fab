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
  , .cleanup = listwise_test_cleanup
  , .tests = (xunit_test*[]) {

/* s : WINDOWS_STAGE | SELECTION_STAGE */
/* l : WINDOWS_STAGE | SELECTION_ACTIVATE */

      (listwise_test[]){{ .entry = listwise_test_entry
        , .init = (char*[]) { "zoo", "foo.a", "foo.a.b", "foo.a.b.c", "bar", 0 }
        , .xsfm = "sx/x o s/z/a sy"
        , .final = (char*[]) { "aoo", "foo.x", "foo.a.x", "foo.a.b.x", 0 }
      }}
      /* o : works across multiple chained o operators */
    , (listwise_test[]){{ .entry = listwise_test_entry
        , .init = (char*[]) { "zoo", "foo.a", "foo.a.b", "foo.a.b.c", "bar", 0 }
        , .xsfm = "sx/x o s/z/a o l/b sy"
        , .final = (char*[]) { "aoo", "foo.x", "foo.a.x", "foo.a.b.x", "bar", 0 }
      }}
    , (listwise_test[]){{ .entry = listwise_test_entry
        , .init = (char*[]) { "zoo", "foo.a", "foo.a.b", "foo.a.b.c", "bar", 0 }
        , .xsfm = "sx/x o l/b"
        , .final = (char*[]) { "foo.x", "foo.a.x", "foo.a.b.x", "bar", 0 }
      }}

/* sy WINDOWS_ACTIVATE | SELECTION_STAGE */
/* sy WINDOWS_STAGE | SELECTION_ACTIVATE */
    , 0
  }
};
