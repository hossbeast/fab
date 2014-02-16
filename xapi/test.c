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
	//fails(ERESTART, "restarting");

finally:
	XAPI_INFOF("foxtrotnum", "%d", num);

	fatal(xclose, 0);
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

	fatal(xclose, -1);
coda;
}

int charlie(int num)
{
	fatal(delta, num);

finally:
	XAPI_INFOF("charlienum", "%d", num);
coda;
}

int bravo(int num)
{
	fatal(charlie, num);

finally:
	XAPI_INFOF("bravonum", "%d", num);
coda;
}

int alpha(int num)
{
	fatal(bravo, num);
finally :
	fatal(bravo, num);
	XAPI_INFOF("alphanum", "%d", num);
coda;
}

int main()
{
	fatal(alpha, 25);

finally :
if(XAPI_UNWINDING)
{
	printf("backtrace: \n");
		xapi_backtrace();

	printf("pithytrace: \n");
		xapi_pithytrace();
}
coda;
}
