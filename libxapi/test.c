#if 0
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "xapi.h"

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
}
coda;
}

int main()
{
	printf("FOO INVOKE\n"); foo(); printf("FOO LEAVE\n");
	printf("FOO INVOKE\n"); foo(); printf("FOO LEAVE\n");
}
#endif
