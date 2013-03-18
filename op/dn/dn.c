#include <stdlib.h>
#include <string.h>
#include <alloca.h>

#include <listwise/operator.h>

#include "control.h"

/*

dn operator - dirname : replace paths with dirname (path)
    not *quite* the same as libgen/dirname

NO ARGUMENTS

OPERATION

	1. foreach selected string
		2. replace that string with dirname(itself)

*/

static int op_exec(operation*, lstack*, int**, int*);

operator op_desc = {
	  .optype				= LWOP_SELECTION_READ | LWOP_OPERATION_INPLACE | LWOP_OBJECT_NO
	, .op_exec			= op_exec
	, .desc					= "get component of filepath preceeding the filename"
};

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	int x;
	for(x = 0; x < ls->s[0].l; x++)
	{
		int go = 1;
		if(!ls->sel.all)
		{
			if(ls->sel.sl <= (x/8))
				break;

			go = (ls->sel.s[x/8] & (0x01 << (x%8)));
		}

		if(go)
		{
			if(ls->s[0].s[x].type == 0 && ls->s[0].s[x].l)
			{
				char * s = ls->s[0].s[x].s;
				char * e = ls->s[0].s[x].s + ls->s[0].s[x].l - 1;

				while(e != s && e[0] == '/')
					e--;

				if(e != s)
					e--;

				while(e != s && e[0] != '/')
					e--;

				while(e != s && e[0] == '/')
					e--;

				if(s != e || s[0] == '.')
				{
					e++;
					memmove(
							ls->s[0].s[x].s
						, s
						, e - s
					);

					ls->s[0].s[x].l = e - s;
					ls->s[0].t[x].w = 0;

					// record this index was hit
					fatal(lstack_last_set, ls, x);
				}
			}
		}
	}

	finally : coda;
}
