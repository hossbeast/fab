#include <stdio.h>
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
	fatal(alpha, 13);
	fatal(alpha, 0);
	fatal(alpha, 25);

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
