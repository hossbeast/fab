#include <stdlib.h>
#include <string.h>

#include "listwise/internal.h"

#include "xmem.h"
#include "control.h"
#include "macros.h"

int API lstack_last_clear(lstack* const restrict ls)
{
	ls->last.sl = 0;
	ls->last.l = 0;

	return 1;
}

int API lstack_last_set(lstack* const restrict ls, int y)
{
	// reallocate if necessary
	if(ls->last.sa <= (y / 8))
	{
		fatal(xrealloc, &ls->last.s, sizeof(ls->last.s[0]), (y/8)+1, ls->last.sa);
		ls->last.sa = (y/8)+1;
	}

	// reset any already-allocated elements now visible
	int x;
	for(x = ls->last.sl; x <= (y/8); x++)
		ls->last.s[x] = 0;

	// increase length
	ls->last.sl = MAX(ls->last.sl, (y/8)+1);

	// set this particular bit
	if((ls->last.s[y/8] & (0x01 << (y%8))) == 0)
		ls->last.l++;

	ls->last.s[y/8] |= (0x01 << y%8);

	finally : coda;
}
