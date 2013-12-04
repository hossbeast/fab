#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "xapi.h"

#define perrtab perrtab_SYS

int beta(int num)
{
printf("-> beta\n");
	if(num == 25)
		fail(ERESTART, "restarting");

finally:
	XAPI_INFO(0, "betanum", "%d", num);
printf("<- beta\n");

/*
	if(num == 25)
	{
		fatalize_sys(close, -1);
	}
*/

coda;
}

int alpha(int num)
{
printf("-> alpha\n");
	fatal(beta, num);

finally:
	XAPI_INFO(1, "alphanum", "%d", num);
printf("<- alpha\n");
coda;
}

int main()
{
	int r = alpha(25);
	printf("alpha(25)=%d\n", r);
	fatal(alpha, 25);
	fatal(alpha, 13);
	fatal(alpha, 0);

finally :
if(XAPI_FAILING)
{
	printf("backtrace: \n");
		xapi_backtrace();

	printf("pithytrace: \n");
		xapi_pithytrace();
}
coda;
}
