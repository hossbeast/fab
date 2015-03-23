#if 0
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "xapi.h"
#include "xapi/callstack.h"

#include "memblk.h"

#define perrtab perrtab_SYS

int xclose(int fd)
{
printf(" >> IN XCLOSE\n");

	fatalize(errno, close, fd);

finally :
	XAPI_INFOF("fd", "%d", fd);
coda;
}

int foxtrot(int num)
{
	if(num == 425)
		fails(ERESTART, "restarting");

finally:
	XAPI_INFOF("foxtrotnum", "%d", num);
coda;
}

int echo(int num)
{
	fatal(foxtrot, num);

finally:
	XAPI_INFOF("echonum", "%d", num);
coda;
}

int delta(int num)
{
	fatal(echo, num);

finally:
	XAPI_INFOF("deltanum", "%d", num);
coda;
}

int charlie(int num)
{
	fatal(delta, num);

finally:
	XAPI_INFOF("charlienum", "%d", num);
coda;
}

int normal()
{
	printf("normal\n");

	finally : coda;
}

int bravo(int num)
{
	fatal(charlie, num);
	printf("bravo %d\n", num);

finally:
	XAPI_INFOF("bravonum", "%d", num);
coda;
}

int alpha(int num)
{
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
	fatal(alpha, 125);
	fatal(alpha, 225);
	fatal(alpha, 325);
	fatal(alpha, 425);

finally :
	fatal(alpha, 425);
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
coda;
}

int main()
{
	char tmp[4096];

	printf("FOO INVOKE\n"); foo(); printf("FOO LEAVE\n");

struct callstack * cs = xapi_callstack_thaw(space);
size_t z = xapi_callstack_trace_full(cs, tmp, sizeof(tmp));
printf(">>>\n%.*s\n<<<\n", (int)z, tmp);

	printf("FOO INVOKE\n"); foo(); printf("FOO LEAVE\n");

	return 0;
}
#endif
