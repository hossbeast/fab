#include <stdlib.h>
#include <alloca.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

#include <listwise/operator.h>

#include "control.h"

#include "parseint.h"

/*

-d operator - select entries whose stringvalue is a path referencing a directory

NO ARGUMENTS

 use current selection, ELSE
 use entire top list

OPERATION

 1. select entries referencing a directory

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc = {
	  .optype				= LWOP_SELECTION_READ | LWOP_SELECTION_WRITE | LWOP_OPERATION_FILESYSTEM
	, .op_validate	= op_validate
	, .op_exec			= op_exec
	, .desc					= "select directories"
};

int op_validate(operation* o)
{
	return 1;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
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

		if(go)
		{
			struct stat st;
			if(stat(lstack_getstring(ls, 0, x), &st) == 0)
			{
				if(S_ISDIR(st.st_mode))
					fatal(lstack_last_set, ls, x);
			}
			else
			{
				dprintf(listwise_err_fd, "stat('%s')=[%d][%s]\n", lstack_getstring(ls, 0, x), errno, strerror(errno));
			}
		}
	}

	finally : coda;
}
