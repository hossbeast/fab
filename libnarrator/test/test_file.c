#include <stdio.h>

#include "xapi.h"
#include "xlinux/SYS.errtab.h"

#include "narrator.h"
#include "narrator/fixed.h"
#include "narrator/growing.h"
#include "narrator/file.h"

xapi validate(narrator * const N)
{
  enter;

  sayf("%d", 40);
  sayf(" %d", 41);
  sayf(" %d", 42);
  sayf(" %d", 43);
  sayf(" %d", 44);

  finally : coda;
}

xapi test_growing()
{
  enter;

  narrator * N = 0;
  fatal(narrator_growing_create, &N);
  fatal(validate, N);
  printf("%s\n", narrator_first(N));

finally:
  narrator_free(N);
coda;
}

xapi test_fixed()
{
  enter;

  narrator * N = 0;
  fatal(narrator_fixed_create, &N, 2048);
  fatal(validate, N);
  printf("%s\n", narrator_first(N));

finally:
  narrator_free(N);
coda;
}

xapi test_file()
{
  enter;

  narrator * N = 0;
  fatal(narrator_file_create, &N, 1);
  fatal(validate, N);
  printf("\n");

finally:
  narrator_free(N);
coda;
}

int main()
{
  enter;

  xapi R = 0;
  fatal(test_growing);
  fatal(test_fixed);
  fatal(test_file);

finally:
  if(XAPI_UNWINDING)
  {
    xapi_backtrace();
  }
conclude(&R);

  return !!R;
}
