#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

#include <listwise/operator.h>

#include "xstring.h"

/*

ss operator - sort strings (in ascending order)

NO ARGUMENTS

 use current selection, ELSE
 use entire top list

OPERATION

 1. sort the items amongst themselves

*/

static int op_validate(operation* o);
static int op_exec(operation*, lstack*, int**, int*);

operator op_desc = {
	  .optype					= LWOP_SELECTION_READ
	, .op_validate	= op_validate
	, .op_exec			= op_exec
	, .desc					= "sort stringwise"
};

int op_validate(operation* o)
{
	return 1;
}

int op_exec(operation* o, lstack* ls, int** ovec, int* ovec_len)
{
	int * mema = alloca((ls->sel.l ?: ls->s[0].l) * sizeof(mema[0]));
	int * memb = 0;

	int x;
	int i = 0;
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
			mema[i++] = x;
	}	

	memb = alloca(i * sizeof(memb[0]));
	memcpy(memb, mema, i * sizeof(memb[0]));

	int compar(const void * A, const void * B)
	{
		char * As;
		int Asl;
		char * Bs;
		int Bsl;

		lstack_string(ls, 0, *(int*)A, &As, &Asl);
		lstack_string(ls, 0, *(int*)B, &Bs, &Bsl);

		return xstrcmp(As, Asl, Bs, Bsl, 0);
	}

	qsort(mema, i, sizeof(mema[0]), compar);

	typeof(ls->s[0].s[0]) * T = alloca(ls->s[0].l * sizeof(T[0]));
	memcpy(T, ls->s[0].s, ls->s[0].l * sizeof(T[0]));

	for(x = 0; x < i; x++)
		ls->s[0].s[memb[x]] = T[mema[x]];

	return 1;
}
