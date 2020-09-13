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

#include "build.h"
#include "common/attrs.h"

attrs16 * APIDATA fab_build_state_attrs = (attrs16[]) {{
#undef DEF
#define DEF(x, s) + 1
    num : 0
      FAB_BUILD_STATE_TABLE
  , members : (member16[]) {
#undef DEF
#define DEF(x, s) { name : s, value : x, range : 0xff },
FAB_BUILD_STATE_TABLE
#undef DEF
  }
}};

static void __attribute__((constructor)) init()
{
  attrs16_init(fab_build_state_attrs);
}
