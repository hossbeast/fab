#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "xapi.h"

#define perrtab perrtab_SYS

int foxtrot(int num)
{
  enter;

	if(num == 425)
		fails(ERESTART, "restarting");

finally:
	XAPI_INFOF("foxtrotnum", "%d", num);
coda;
}

int echo(int num)
{
  enter;

	fatal(foxtrot, num);

finally:
	XAPI_INFOF("echonum", "%d", num);
coda;
}

int delta(int num)
{
  enter;

	fatal(echo, num);

finally:
	XAPI_INFOF("deltanum", "%d", num);
coda;
}

int charlie(int num)
{
  enter;

	fatal(delta, num);

finally:
	XAPI_INFOF("charlienum", "%d", num);
coda;
}

int normal()
{
  enter;

	printf("normal\n");

	finally : coda;
}

int bravo(int num)
{
  enter;

	fatal(charlie, num);

finally:
	XAPI_INFOF("bravonum", "%d", num);
coda;
}

int alpha(int num)
{
  enter;

printf("bravo %d\n", num);

	fatal(bravo, num);
//	bravo(17);
//	fatal(normal);

finally :
//	fatal(bravo, num);
	XAPI_INFOF("alphanum", "%d", num);
coda;
}

char space[4096 << 2];

int foo()
{
  enter;

	fatal(alpha, 125);
	fatal(alpha, 225);
	fatal(alpha, 325);
	fatal(alpha, 425);

finally :
	fatal(alpha, 425);
#if 0
if(XAPI_UNWINDING)
{
	printf("backtrace: \n");
		xapi_backtrace();

	printf("pithytrace: \n");
		xapi_pithytrace();

	memblk * mb = xapi_callstack_freeze();
	memblk_copyto(mb, space, sizeof(space));
	xapi_callstack_unfreeze();
}
#endif
coda;
}

int main()
{
	printf("FOO INVOKE\n"); foo(); printf("FOO LEAVE\n");

#if 0
char tmp[4096];

struct callstack * cs = xapi_callstack_thaw(space);
size_t z = xapi_callstack_trace_full(cs, tmp, sizeof(tmp));
printf(">>>\n%.*s\n<<<\n", (int)z, tmp);
#endif

	printf("FOO INVOKE\n"); foo(); printf("FOO LEAVE\n");

	return 0;
}
