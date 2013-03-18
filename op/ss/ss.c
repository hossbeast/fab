#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

#include <listwise/operator.h>
#include <listwise/lstack.h>

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
	size_t num = ls->sel.all ? ls->s[0].l : ls->sel.l;

	// indexes to be sorted
	int * mema = calloc(num, sizeof(*mema));

	// copy of indexes
	int * memb = calloc(num, sizeof(*memb));

	// copies of entries to be swapped
	typeof(ls->s[0].s[0]) * T = calloc(ls->s[0].l, sizeof(*T));
	memcpy(T, ls->s[0].s, ls->s[0].l * sizeof(*T));

	int i = 0;
	int x;
	LSTACK_ITERATE(ls, x, go)
	if(go)
	{
		mema[i] = x;
		memb[i] = x;
		i++;
	}
	LSTACK_ITEREND;

	int compar(const void * A, const void * B)
	{
		char *	As = 0;
		int			Asl = 0;
		char *	Bs = 0;
		int			Bsl = 0;

		lstack_string(ls, 0, *(int*)A, &As, &Asl);
		lstack_string(ls, 0, *(int*)B, &Bs, &Bsl);

		return xstrcmp(As, Asl, Bs, Bsl, 0);
	}

	qsort(mema, i, sizeof(*mema), compar);

	for(x = 0; x < i; x++)
	{
		ls->s[0].s[memb[x]] = T[mema[x]];
	}

	free(mema);
	free(memb);
	free(T);

	return 1;
}
