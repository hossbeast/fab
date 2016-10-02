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

/* p : WINDOWS_ACTIVATE | SELECTION_STAGE */

    // off/len of zero : all fields
      (listwise_test[]){{ .entry = listwise_test_entry
        , .init = (char*[]) { "\"alpha\"", "\"alpha\" : \"beta\"", "\"alpha\" : \"beta\" : \"gamma\"", "\"alpha\" : \"beta\" : \"gamma\" : \"delta\" : \"epsilon\"", 0 }
        , .xsfm = "l/\"/g y f"
        , .final = (char*[]) { "alpha", "alphabeta", "alphabetagamma", "alphabetagammadeltaepsilon", 0 }
      }}
    , (listwise_test[]){{ .entry = listwise_test_entry
        , .init = (char*[]) { "\"alpha\"", "\"alpha\" : \"beta\"", "\"alpha\" : \"beta\" : \"gamma\"", "\"alpha\" : \"beta\" : \"gamma\" : \"delta\" : \"epsilon\"", 0 }
        , .xsfm = "l/\"/g y f/0"
        , .final = (char*[]) { "alpha", "alphabeta", "alphabetagamma", "alphabetagammadeltaepsilon", 0 }
      }}
    , (listwise_test[]){{ .entry = listwise_test_entry
        , .init = (char*[]) { "\"alpha\"", "\"alpha\" : \"beta\"", "\"alpha\" : \"beta\" : \"gamma\"", "\"alpha\" : \"beta\" : \"gamma\" : \"delta\" : \"epsilon\"", 0 }
        , .xsfm = "l/\"/g y f/0/0"
        , .final = (char*[]) { "alpha", "alphabeta", "alphabetagamma", "alphabetagammadeltaepsilon", 0 }
      }}

    // off/len : in fields
    , (listwise_test[]){{ .entry = listwise_test_entry
        , .init = (char*[]) { "\"alpha\"", "\"alpha\" : \"beta\"", "\"alpha\" : \"beta\" : \"gamma\"", "\"alpha\" : \"beta\" : \"gamma\" : \"delta\" : \"epsilon\"", 0 }
        , .xsfm = "l/\"/g y f/0/1"
        , .final = (char*[]) { "alpha", "alpha", "alpha", "alpha", 0 }
      }}
    , (listwise_test[]){{ .entry = listwise_test_entry
        , .init = (char*[]) { "\"alpha\"", "\"alpha\" : \"beta\"", "\"alpha\" : \"beta\" : \"gamma\"", "\"alpha\" : \"beta\" : \"gamma\" : \"delta\" : \"epsilon\"", 0 }
        , .xsfm = "l/\"/g y f/1/2"
        , .final = (char*[]) { "betagamma", "betagamma", 0 }
      }}

    // negative offset : relative to end of fields
    , (listwise_test[]){{ .entry = listwise_test_entry
        , .init = (char*[]) { "\"alpha\"", "\"alpha\" : \"beta\"", "\"alpha\" : \"beta\" : \"gamma\"", "\"alpha\" : \"beta\" : \"gamma\" : \"delta\" : \"epsilon\"", 0 }
        , .xsfm = "l/\"/g y f/-2/1 y"
        , .final = (char*[]) { "alpha", "beta", "delta", 0 }
      }}
    // negative length : relative to end of fields
    , (listwise_test[]){{ .entry = listwise_test_entry
        , .init = (char*[]) { "\"alpha\"", "\"alpha\" : \"beta\"", "\"alpha\" : \"beta\" : \"gamma\"", "\"alpha\" : \"beta\" : \"gamma\" : \"delta\" : \"epsilon\"", 0 }
        , .xsfm = "l/\"/g y f/-2/-1 y"
        , .final = (char*[]) { "alphabeta", "betagamma", "deltaepsilon", 0 }
      }}

    // other
    , (listwise_test[]){{ .entry = listwise_test_entry
        , .init = (char*[]) { "foo" }
        , .xsfm = "l/o/g wy f"
        , .final = (char*[]) { 0 }
      }}
    , 0
  }
};
