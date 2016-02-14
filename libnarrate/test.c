#include <stdio.h>

#include "xapi.h"
#include "xlinux/SYS.errtab.h"
#include "narrate.h"

xapi foo()
{
  enter;

  tfail(perrtab_SYS, ENXIO);

  finally : coda;
}

xapi delta()
{
  enter;

  char space[64] = { [0] = 0 };
  narrationw(space, sizeof(space));

  sayf("%d", 40);
  sayf(" %d", 41);
  sayf(" %d", 42);
  sayf(" %d", 43);
  sayf(" %d", 44);
  printf("A%sB\n", space);

  finally : coda;
}

xapi alpha()
{
  enter;

  fatal(foo);

finally:
  fatal(delta);
coda;
}

xapi zeta()
{
  enter;

  fatal(alpha);

finally:
  if(XAPI_UNWINDING)
  {
    xapi_backtrace();
  }
coda;
}

int main()
{
  zeta();
}
