#include <stdio.h>

#include "xapi.h"
#include "narrate.h"
#include "narrate/fixed.h"

int foo()
{
  tfail(perrtab_SYS, ENXIO);

  finally : coda;
}

int main()
{
  char space[64] = { [0] = 0 };
  fatal(foo);

finally:
  if(XAPI_UNWINDING)
  {
    narrationw(space, sizeof(space));
    fsayf("%d", 40);
    fsayf(" %d", 41);
    fsayf(" %d", 42);
    fsayf(" %d", 43);
    fsayf(" %d", 44);
    printf("A%sB\n", space);

    xapi_backtrace();
  }
coda;
}
