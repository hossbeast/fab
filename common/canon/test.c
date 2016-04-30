#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "canon.h"

struct canon_test;
#define TEST_TYPE struct canon_test
#include "xunit.h"

xapi canon_test_entry(const struct canon_test*)
  __attribute__((nonnull));

typedef struct canon_test
{
  xunit_test;
  char *    path;
  char *    base;
  uint32_t  opts;
  char *    final;
} canon_test;

xapi canon_test_entry(const canon_test * test)
{
  enter;

  char space[512];

  // log the inset
  logs(L_XUNIT | L_INSET, test->path);

  // transform
  size_t z;
  fatal(canon, test->path, 0, test->base, 0, space, sizeof(space), &z, test->opts);

  // log the outset
  logs(L_XUNIT | L_OUTSET, space);

  if(strcmp(test->final, space))
  {
    failf(XUNIT_FAIL, "expected %s, actual %s", test->final, space);
  }

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
