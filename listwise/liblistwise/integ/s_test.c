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

/* s : SELECTION_STAGE | WINDOWS_STAGE | MODIFIERS_CANHAVE | ARGS_CANHAVE | OPERATION_INPLACE */

    // remove part of the row, select/leave the remainder
      (listwise_test[]){{
          .init = (char*[]) { "1-DFOO", 0 }
        , .xsfm = "s/^1// sy"
        , .final = (char*[]) { "-DFOO", 0 }
      }}
    , 0
  }
};
