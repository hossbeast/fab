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

#include "pathtree_test.h"

/*

TEST
  test_load

SUMMARY
  verifies behavior with a larger set of keys compared to the other tests

*/

xunit_unit xunit = {
    setup : pathtree_unit_setup
  , tests : (xunit_test*[]) {
      (pathtree_test[]){{
          entries : (typeof(*((pathtree_test*)0)->entries)[]) {
              { key : "/asgard" }
            , { key : "/asgard/bi/midgard" }
            , { key : "/asgard/bifrost" }
            , { key : "/asgard/bifrost/midgard" }
            , { key : "/asgard/helgrind" }
            , { key : "/asgard/helgrind/ragnarok/heimdall" }
            , { key : "/bifrost/asgard" }
            , { key : "/bifrost/asgard/bi/midgard" }
            , { key : "/bifrost/asgard/bifrost" }
            , { key : "/bifrost/asgard/bifrost/midgard" }
            , { key : "/bifrost/asgard/helgrind" }
            , { key : "/mordor/bifrost/helgrind/ragnarok/heimdall" }
            , { key : "/mordor/bifrost/helgrind/ragnarok/isengard/heimdall" }
            , { key : "/mordor/bifrost/isengard" }
            , { key : "/mordor/bifrost/midgard" }
            , { key : "/mordor/bifrost/midgard/a" }
            , { key : "/mordor/bifrost/midgard/ab" }
            , { key : "/mordor/bifrost/midgard/ab/bifrost" }
            , { key : "/mordor/bifrost/midgard/abc" }
            , { key : "/mordor/bifrost/midgard/abcd" }
            , { key : "/mordor/bifrost/midgard/bifr" }
            , { key : "/mordor/helgrind/ragnarok/bifrost" }
            , { key : "/mordor/helgrind/ragnarok/bifrost/heimdall" }
            , { key : "/mordor/helgrind/ragnarok/heimdall" }
            , { key : "/mordor/helgrind/ragnarok/isengard" }
            , { key : "/mordor/helgrind/ragnarok/isengard/heimdall" }
            , { key : "/bifrost/asgard/helgrind/ragnarok/heimdall" }
            , { key : "/bifrost/asgard/helgrind/ragnarok/heimdall" }
            , { key : "/bifrost/helgrind/ragnarok/bifrost" }
            , { key : "/bifrost/helgrind/ragnarok/isengard" }
            , { key : "/bifrost/helgrind/ragnarok/bifrost/heimdall" }
            , { key : "/bifrost/helgrind/ragnarok/heimdall" }
            , { key : "/bifrost/helgrind/ragnarok/isengard/heimdall" }
            , { key : "/bifrost/isengard" }
            , { key : "/bifrost/midgard" }
            , { key : "/bifrost/midgard/a" }
            , { key : "/bifrost/midgard/ab" }
            , { key : "/bifrost/midgard/ab/bifrost" }
            , { key : "/bifrost/midgard/abc" }
            , { key : "/bifrost/midgard/abcd" }
            , { key : "/bifrost/midgard/bifr" }
            , { key : "/helgrind/ragnarok/bifrost" }
            , { key : "/helgrind/ragnarok/bifrost/heimdall" }
            , { key : "/helgrind/ragnarok/heimdall" }
            , { key : "/helgrind/ragnarok/isengard" }
            , { key : "/helgrind/ragnarok/isengard/heimdall" }
            , { key : "/isengard" }
            , { key : "/midgard" }
            , { key : "/midgard/a" }
            , { key : "/midgard/ab" }
            , { key : "/midgard/ab/bifrost" }
            , { key : "/midgard/abc" }
            , { key : "/midgard/abcd" }
            , { key : "/midgard/bifr" }
            , { key : "/midgard/bifrost-o/a/b" }
            , { key : "/midgard/bifrost-other" }
            , { key : "/midgard/bifrost/asgard" }
            , { key : "/midgard/helgrind" }
            , { key : "/mordor/rohan/rivendell/gondor/bifrost/midgard/a" }
            , { key : "/mordor/rohan/rivendell/gondor/bifrost/midgard/ab" }
            , { key : "/mordor/rohan/rivendell/gondor/bifrost/midgard/ab/bifrost" }
            , { key : "/mordor/rohan/rivendell/gondor/bifrost/midgard/abc" }
            , { key : "/mordor/rohan/rivendell/gondor/bifrost/midgard/abcd" }
            , { key : "/mordor/rohan/rivendell/gondor/bifrost/midgard/bifr" }
            , { key : "/mordor/rohan/rivendell/gondor/helgrind/ragnarok/bifrost" }
            , { key : "/mordor/rohan/rivendell/gondor/helgrind/ragnarok/bifrost/heimdall" }
            , { key : "/mordor/rohan/rivendell/gondor/helgrind/ragnarok/heimdall" }
            , { key : "/mordor/rohan/rivendell/gondor/helgrind/ragnarok/isengard" }
            , { key : "/mordor/rohan/rivendell/gondor/helgrind/ragnarok/isengard/heimdall" }
            , { key : "/mordor/rohan/rivendell/gondor/isengard" }
            , { key : "/mordor/rohan/rivendell/gondor/midgard" }
            , { key : "/mordor/rohan/rivendell/gondor/midgard/a" }
            , { key : "/mordor/rohan/rivendell/gondor/midgard/ab" }
            , { key : "/mordor/rohan/rivendell/gondor/midgard/ab/bifrost" }
            , { key : "/mordor/rohan/rivendell/gondor/midgard/abc" }
            , { key : "/mordor/rohan/rivendell/gondor/midgard/abcd" }
            , { key : "/mordor/rohan/rivendell/gondor/midgard/bifr" }
            , { key : "/mordor/rohan/rivendell/gondor/midgard/bifrost-o/a/b" }
            , { }
          }
      }}
    , 0
  }
};
