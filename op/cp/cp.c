#include <stdlib.h>
#include <string.h>

#include <listwise/operator.h>
#include <listwise/lstack.h>

#include "control.h"
#include "xmem.h"

/*

cp operator - duplicate list entries

ARGUMENTS

 number of copies - default : 1

OPERATION

 use current select, ELSE
 use entire top list

 for each entry
	1. create N copies of this entry at indexes x + 1 .. N

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc = {
	  .optype				= LWOP_SELECTION_READ | LWOP_SELECTION_WRITE | LWOP_ARGS_CANHAVE
	, .op_validate	= op_validate
	, .op_exec			= op_exec
	, .desc					= "duplicate list entries"
};

int op_validate(operation* o)
{
	if(o->argsl >= 1 && o->args[0]->itype != ITYPE_I64)
		fail("cp - first argument should be i64");

	finally : coda;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	int N = 1;
	if(o->argsl)
		N = o->args[0]->i64;

	int go = 0;
	int c = 0;
	int k = ls->s[0].l;

	int x;
	LSTACK_ITERREV(ls, x, go);
	if(go)
	{
		// prepare N entries at x+1 for writing
		fatal(lstack_displace, ls, 0, x + 1, N);
		
		int y;
		for(y = 1; y <= N; y++)
		{
			if(ls->s[0].s[x].type)
			{
				fatal(lstack_obj_write_alt, ls, 0, x+y, *(void**)ls->s[0].s[x].s, ls->s[0].s[x].type);
			}
			else
			{
				fatal(lstack_write_alt, ls, 0, x+y, ls->s[0].s[x].s, ls->s[0].s[x].l);
			}
		}
	}
	LSTACK_ITEREND;

	LSTACK_ITERATE(ls, x, go);
	if(x < k)
	{
		if(go)
		{
			int y;
			for(y = 0; y <= N; y++)
				fatal(lstack_last_set, ls, x + y + c);

			c += N;
		}
	}
	LSTACK_ITEREND;

	finally : coda;
}
