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
    .xu_setup = listwise_test_setup
  , .xu_cleanup = listwise_test_cleanup
  , .xu_entry = listwise_test_entry
  , .xu_tests = (xunit_test*[]) {

/* p : WINDOWS_ACTIVATE | SELECTION_STAGE */

    // off/len of zero : entire string
      (listwise_test[]){{
          .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
        , .xsfm = "l/./g y p/0"
        , .final = (char*[]) { "foo.a", "foo.a.b", "foo.a.b.c", 0 }
      }}
    , (listwise_test[]){{
          .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
        , .xsfm = "l/./g y p/0/0"
        , .final = (char*[]) { "foo.a", "foo.a.b", "foo.a.b.c", 0 }
      }}

    // off/len : in fields
    , (listwise_test[]){{
          .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
        , .xsfm = "l/./g y p/0/1"
        , .final = (char*[]) { "foo", "foo", "foo", 0 }
      }}

    // negative offset : relative to end of string
    , (listwise_test[]){{
          .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
        , .xsfm = "l/./g y p/-3"
        , .final = (char*[]) { "foo.a.b", "a.b.c", 0 }
      }}
    , (listwise_test[]){{
          .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
        , .xsfm = "l/./g y p/-3 y"
        , .final = (char*[]) { "foo.a.b", "a.b.c", 0 }
      }}
    , (listwise_test[]){{
          .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
        , .xsfm = "l/./g y p/-1"
        , .final = (char*[]) { "a", "b", "c", 0 }
      }}
    , (listwise_test[]){{
          .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
        , .xsfm = "l/./g y p/-1"
        , .final = (char*[]) { "a", "b", "c", 0 }
      }}

    // negative length : relative to end of string
    , (listwise_test[]){{
          .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
        , .xsfm = "l/./g y p/0/-2 y"
        , .final = (char*[]) { "foo", "foo.a", 0 }
      }}
    , (listwise_test[]){{
          .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
        , .xsfm = "l/./g y p/-3/-2"
        , .final = (char*[]) { "foo", "a", 0 }
      }}

    // arguments come in pairs
    , (listwise_test[]){{
          .init = (char*[]) { "foo", "foo.a", "foo.a.b", "foo.a.b.c", 0 }
        , .xsfm = "l/./g y p/0/1/2/1"
        , .final = (char*[]) { "foo", "foob", "foob", 0 }
      }}
    , 0
  }
};
