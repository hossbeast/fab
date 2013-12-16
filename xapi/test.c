#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "xapi.h"

#define perrtab perrtab_SYS

int foxtrot(int num)
{
	if(num == 25)
	{
printf("foxtrot failing\n");
		fail(ERESTART, "restarting");
	}

finally:
	XAPI_INFO("foxtrotnum", "%d", num);

	if(num == 255)
	{
		sysfatalize(close, -1);
	}
coda;
}

int echo(int num)
{
	fatal(foxtrot, num);

finally:
	XAPI_INFO("echonum", "%d", num);
coda;
}

int delta(int num)
{
	fatal(echo, num);

finally:
	XAPI_INFO("deltanum", "%d", num);
coda;
}

int charlie(int num)
{
	fatal(delta, num);

finally:
	XAPI_INFO("charlienum", "%d", num);
coda;
}

int bravo(int num)
{
	fatal(charlie, num);

finally:
	XAPI_INFO("bravonum", "%d", num);
coda;
}

int alpha(int num)
{
	int fd = 0;

	int r = bravo(num);
printf("bravo returns %d\n", r);
	if(r != 0)
		fatality("bravo", perrtab, ENOMEM, 0);

finally:
	XAPI_INFO("alphanum", "%d", num);
coda;
}

int main()
{
	fatal(alpha, 25);

finally :
if(XAPI_UNWINDING)
{
XAPI_INFO("mainnum", "%d", 0);

	printf("backtrace: \n");
		xapi_backtrace();

	printf("pithytrace: \n");
		xapi_pithytrace();
}
coda;
}
