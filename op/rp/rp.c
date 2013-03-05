#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

#include <listwise/operator.h>

#include "control.h"
#include "xmem.h"

/*

rp operator - path canonicalization (with realpath)

NO ARGUMENTS

 use current selection, ELSE
 use entire top list

OPERATION

 1. rewrite the item as a canonicalized path

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc = {
	  .optype					= LWOP_SELECTION_READ | LWOP_OPERATION_INPLACE | LWOP_OPERATION_FILESYSTEM | LWOP_OBJECT_NO
	, .op_validate	= op_validate
	, .op_exec			= op_exec
	, .desc					= "path canonicalization with realpath"
};

int op_validate(operation* o)
{
	return 1;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	char * ss = 0;
	int x;
	for(x = 0; x < ls->s[0].l; x++)
	{
		int go = 1;
		if(!ls->sel.all)
		{
			if(ls->sel.sl <= (x/8))	// could not be selected
				break;

			go = (ls->sel.s[x/8] & (0x01 << (x%8)));	// whether it is selected
		}

		if(go && ls->s[0].s[x].type == 0)
		{
			xfree(&ss);
			if((ss = realpath(ls->s[0].s[x].s, 0)))
			{
				fatal(lstack_write
					, ls
					, 0
					, x
					, ss
					, strlen(ss)
				);
				fatal(lstack_last_set, ls, x);
			}
			else
			{
				dprintf(listwise_err_fd, "realpath('%s')=[%d][%s]\n", ls->s[0].s[x].s, errno, strerror(errno));
			}
		}
	}

finally:
	free(ss);
coda;
}
