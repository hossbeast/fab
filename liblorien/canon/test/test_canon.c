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

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "canon.h"

struct canon_test;
#define TEST_TYPE struct canon_test
#include "xunit.h"
#include "xunit/assert.h"

#include "logger.h"

typedef struct canon_test
{
  xunit_test;
  char *    path;
  char *    base;
  uint32_t  opts;
  char *    final;
} canon_test;

xapi __attribute__((nonnull)) canon_test_entry(canon_test * const test)
{
  enter;

  char space[512];

  // transform
  size_t z;
  fatal(canon, test->path, 0, test->base, 0, space, sizeof(space), &z, test->opts);

  assertf(strcmp(test->final, space) == 0, "%s", "%s", test->final, space);

  finally : coda;
}

xunit_unit xunit = {
    .tests = (xunit_test*[]) {
        (canon_test[]){{ .entry = canon_test_entry
          , .path   = "."
          , .base   = "/home/todd/fab"
          , .opts   = CAN_REALPATH
          , .final  = "/home/todd/fab"
        }}
      , (canon_test[]){{ .entry = canon_test_entry
          , .path = "/home/todd/fab/fab/fab/fab.devel/.."
          , .base = "/home/todd/fab/listwise/listwise"
          , .opts = CAN_REALPATH
          , .final = "/home/todd/fab/fab/fab"
        }}
      , (canon_test[]){{ .entry = canon_test_entry
          , .path = "./fabfile"
          , .base = "/home/todd/fab/listwise/listwise"
          , .opts = CAN_REALPATH
          , .final = "/home/todd/fab/listwise/listwise/fabfile"
        }}
      , (canon_test[]){{ .entry = canon_test_entry
          , .path = "./fabfile"
          , .base = "/home/todd/fab/listwise/listwise"
          , .opts = CAN_INIT_DOT | CAN_NEXT_DOT | CAN_FORCE_DOT | CAN_NEXT_SYM
          , .final = "/home/todd/fab/listwise/listwise/fabfile"
        }}
      , (canon_test[]) {{ .entry = canon_test_entry
          , .path = "/home/todd/fab/devel/var/tmp/fab/pid/638/fml/1156/out"
          , .base = "/../FABSYS/dscv"
          , .opts = CAN_INIT_DOT | CAN_NEXT_DOT | CAN_FORCE_DOT | CAN_NEXT_SYM
          , .final = "/home/todd/fab/devel/var/tmp/fab/pid/638/fml/1156/out"
      }}
      , (canon_test[]){{ .entry = canon_test_entry
          , .path = "../../fab/fabd/ts/ts.debug.o"
          , .base = "/home/todd/fab/fab/fabd"
          , .opts = CAN_REALPATH
          , .final = "/home/todd/fab/fab/fabd/ts/ts.debug.o"
      }}
      , (canon_test[]) {{ .entry = canon_test_entry
          , .path = "../../fab/fabd/strstack/strstack.debug.o"
          , .base = "/home/todd/fab/fab/fabd"
          , .opts = CAN_REALPATH
          , .final = "/home/todd/fab/fab/fabd/strstack/strstack.debug.o"
      }}
      , (canon_test[]) {{ .entry = canon_test_entry
          , .path = "../../fab/fabd/traverse/traverse.debug.o"
          , .base = "/home/todd/fab/fab/fabd"
          , .opts = CAN_REALPATH
          , .final = "/home/todd/fab/fab/fabd/traverse/traverse.debug.o"
      }}
      , (canon_test[]) {{ .entry = canon_test_entry
          , .path = "../../fab/common/dirutil/dirutil.debug.o"
          , .base = "/home/todd/fab/fab/fabd"
          , .opts = CAN_REALPATH
          , .final = "/home/todd/fab/fab/common/dirutil/dirutil.debug.o"
      }}
      , 0
  }
};
