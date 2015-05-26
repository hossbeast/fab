/* Copyright (c) 2012-2015 Todd Freed <todd.freed@gmail.com>

   This file is part of fab.
   
   fab is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   fab is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with fab.  If not, see <http://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <string.h>

#include "xapi.h"
#include "xlinux.h"

#include "selector.def.h"
#include "args.def.h"
#include "memblk.def.h"
#include "FAB.errtab.h"

#define restrict __restrict

//
// public
//

char * selector_string(const selector * const restrict s, char * const restrict dst, const size_t z)
{
	int l = 0;
	l += snprintf(dst + l, z - l, "{ %c", s->mode);

	uint32_t x = 1;
	while(x)
	{
		if(s->lists & x)
		{
			if(l > 3)
				l += snprintf(dst + l, z - l, "|");
			l += snprintf(dst + l, z - l, "%s", SELECTOR_STR(x));
		}
		x <<= 1;
	}

	l += snprintf(dst + l, z - l, ",%s : %s }", SELECTOR_BASE_STR(s->base), s->s);
	return dst;
}

int selector_parse(char * const s, uint32_t * const lists, uint8_t * const base, int * const mode)
{
	if(s)
	{
		if(s[0] == '-' || s[0] == '+')
		{
			if((s[1] >= 'a' && s[1] <= 'z') || (s[1] >= 'A' && s[1] <= 'Z'))
			{
				(*mode) = s[0];
				(*lists) = 0;
				(*base) = SELECTOR_BASE_CWD;
				
				if(strchr(s, 'd'))
					(*lists) |= SELECTOR_DISCOVERY;
				if(strchr(s, 'i'))
					(*lists) |= SELECTOR_INSPECT;
				if(strchr(s, 't'))
					(*lists) |= SELECTOR_FABRICATE;
				if(strchr(s, 'x'))
					(*lists) |= SELECTOR_FABRICATEX;
				if(strchr(s, 'n'))
					(*lists) |= SELECTOR_FABRICATEN;
				if(strchr(s, 'b'))
					(*lists) |= SELECTOR_INVALIDATE;
				if(strchr(s, 'q'))
					(*lists) |= SELECTOR_QUERY;

				if(strchr(s, 'C'))
					(*base) = SELECTOR_BASE_CWD;
				else if(strchr(s, 'F'))
					(*base) = SELECTOR_BASE_FABFILE_DIR;
			}
			else
			{
				failf(FAB_BADARGS, "unknown : %s", s);
			}
		}
		else
		{
			if(g_args->selectorsl == g_args->selectorsa)
			{
				int newa = g_args->selectorsa ?: 3;
				newa = newa * 2 + newa / 2;
				fatal(xrealloc, &g_args->selectors, sizeof(g_args->selectors[0]), newa, g_args->selectorsa);
				g_args->selectorsa = newa;
			}

			if((*lists) & SELECTOR_QUERY)
				g_args->selectors_arequery = 1;
			else if((*lists) & SELECTOR_INSPECT)
				g_args->selectors_areinspections = 1;
			else if((*lists) & SELECTOR_DISCOVERY)
				g_args->selectors_arediscovery = 1;

			g_args->selectors[g_args->selectorsl] = (selector){
					.mode = (*mode)
				, .lists = (*lists)
				, .base = (*base)
				, .sl = strlen(s)
			};

			fatal(ixstrdup, &g_args->selectors[g_args->selectorsl].s, s);
			g_args->selectorsl++;
		}
	}

	finally : coda;
}

void selector_freeze(memblk * const restrict mb, selector * restrict s)
{
	memblk_freeze(mb, &s->s);
}

void selector_thaw(char * const restrict mb, selector * restrict s)
{
	memblk_thaw(mb, &s->s);
}
