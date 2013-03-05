#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>

#include <listwise/operator.h>

#include "control.h"
#include "xstring.h"

#include "parseint.h"

/*

u operator - select stringwise-unique entries (expects an already-sorted list)

NO ARGUMENTS

 use current selection, ELSE
 use entire top list

OPERATION

 1. select entries which are stringwise NEQ to the preceeding entry

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc = {
	  .optype				= LWOP_SELECTION_READ | LWOP_SELECTION_WRITE
	, .op_validate	= op_validate
	, .op_exec			= op_exec
	, .desc					= "select unique"
};

int op_validate(operation* o)
{
	return 1;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	int p = -1;
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
			char * As = 0;
			int    Asl = 0;
			char * Bs = 0;
			int    Bsl = 0;

			if(p != -1)
			{
				lstack_string(ls, 0, p, &As, &Asl);
				lstack_string(ls, 0, x, &Bs, &Bsl);
			}

			if(p == -1 || xstrcmp(As, Asl, Bs, Bsl, 0))
			{
				fatal(lstack_last_set, ls, x);
			}

			p = x;
		}
	}

	finally : coda;
}
