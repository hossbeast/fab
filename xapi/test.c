#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "xapi.h"

#define perrtab perrtab_SYS

int foxtrot(int num)
{
	if(num == 25)
		fail(ERESTART, "restarting");

finally:
	XAPI_INFO(0, "foxtrotnum", "%d", num);

	if(num == 255)
	{
		fatalize_sys(close, -1);
	}
coda;
}

int echo(int num)
{
	fatal(foxtrot, num);

finally:
	XAPI_INFO(1, "echonum", "%d", num);
coda;
}

int delta(int num)
{
	fatal(echo, num);

finally:
	XAPI_INFO(1, "deltanum", "%d", num);
coda;
}

int charlie(int num)
{
	fatal(delta, num);

finally:
	XAPI_INFO(1, "charlienum", "%d", num);
coda;
}

int bravo(int num)
{
	int fd = 0;
//	fatal(charlie, num);

finally:
	XAPI_INFO(1, "bravonum", "%d", num);

	fatalize_sys(close, fd);
coda;
}

int alpha(int num)
{
	int fd = 0;

	fatal(bravo, num);

finally:
	XAPI_INFO(1, "alphanum", "%d", num);
coda;
}

int launch()
{
	fatal(alpha, 25);

finally :
if(XAPI_UNWINDING)
{
XAPI_INFO(1, "mainnum", "%d", 0);

	printf("backtrace: \n");
		xapi_backtrace();

	printf("pithytrace: \n");
		xapi_pithytrace();
}
coda;
}

int main()
{
	int r =	launch();
	printf("launch()=%d\n", r);

	return r;
}
