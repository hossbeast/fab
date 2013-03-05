#include <stdlib.h>
#include <string.h>

#include "listwise/internal.h"

#include "xmem.h"
#include "control.h"
#include "macros.h"

int API lstack_sel_none(lstack* const restrict ls)
{
	ls->sel.all = 0;
	ls->sel.sl = 0;
	ls->sel.l = 0;

	return 1;
}

int API lstack_sel_all(lstack* const restrict ls)
{
	ls->sel.all = 1;

	return 1;
}

/*
int API lstack_sel_set(lstack* const restrict ls, int y)
{
	if(!ls->sel.all)
	{
		// reallocate if necessary
		if(ls->sel.sa <= (y / 8))
		{
			fatal(xrealloc, &ls->sel.s, sizeof(ls->sel.s[0]), (y/8)+1, ls->sel.sa);
			ls->sel.sa = (y/8)+1;
		}

		// reset any already-allocated elements now visible
		int x;
		for(x = ls->sel.sl; x <= (y/8); x++)
			ls->sel.s[x] = 0;

		// increase length
		ls->sel.sl = MAX(ls->sel.sl, (y/8)+1);

		// set this particular bit
		if((ls->sel.s[y/8] & (0x01 << (y%8))) == 0)
			ls->sel.l++;

		ls->sel.s[y/8] |= (0x01 << y%8);

		if(ls->sel.l == ls->s[0].l)
			ls->sel.all = 1;
	}

	return 1;
}
*/

/*
int API lstack_sel_write(lstack* const restrict ls, uint8_t * news, int newsl)
{
	ls->sel.all = 0;

	// reallocate if necessary
	if(ls->sel.sa < newsl)
	{
		fatal(xrealloc, &ls->sel.s, sizeof(ls->sel.s[0]), newsl, ls->sel.sa);
		ls->sel.sa = newsl;	
	}

	ls->sel.sl = newsl;

	// reset
	memset(ls->sel.s, 0, sizeof(ls->sel.s[0]) * ls->sel.sl);
	ls->sel.l = 0;

	// set those bits necessary
	int x;
	for(x = 0; x < (newsl * 8); x++)
	{
		if(news[x/8] & (0x01 << (x%8)))
		{
			ls->sel.s[x/8] |= 0x01 << (x%8);
			ls->sel.l++;
		}
	}
}
*/
