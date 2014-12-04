/* Copyright (c) 2012-2013 Todd Freed <todd.freed@gmail.com>

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

#include <stdint.h>

#include "global.h"

#include "xapi/XAPI.errtab.h"
#include "xlinux/XLINUX.errtab.h"
#include "listwise/LW.errtab.h"
#include "listwise/PCRE.errtab.h"

static etable * tab[6];

void error_setup()
{
	tab[0] = perrtab_SYS;				// system errors
	tab[0]->id = 0;

	tab[1] = perrtab_XAPI;			// libxapi errors
	tab[1]->id = 1;

	tab[2] = perrtab_XLINUX;		// libxlinux errors
	tab[2]->id = 2;

	tab[3] = perrtab_PCRE;			// libpcre errors
	tab[3]->id = 3;

	tab[4] = perrtab_LW;				// liblistwise errors
	tab[4]->id = 4;

	tab[5] = perrtab_FAB;				// fab errors
	tab[5]->id = 5;
}

const char * fab_errname(const int code)
{
	int16_t rt = code >> 16;			// table index
	int16_t rc = code & 0xFFFF;		// code index

	if(rt < 0 || rt > 5)
		return 0;

	return tab[rt]->v[rc + (tab[rt]->min * -1)].name;
}

const char * fab_errdesc(const int code)
{
	int16_t rt = code >> 16;			// table index
	int16_t rc = code & 0xFFFF;		// code index

	if(rt < 0 || rt > 5)
		return 0;

	return tab[rt]->v[rc + (tab[rt]->min * -1)].desc;
}

const char * fab_errstr(const int code)
{
	int16_t rt = code >> 16;			// table index
	int16_t rc = code & 0xFFFF;		// code index

	if(rt < 0 || rt > 5)
		return 0;

	return tab[rt]->v[rc + (tab[rt]->min * -1)].str;
}

const etable * fab_errtab(const int code)
{
	int16_t rt = code >> 16;			// table index

	if(rt < 0 || rt > 5)
		return 0;

	return tab[rt];
}

int fab_errcode(const int code)
{
	int16_t rt = code >> 16;			// table index
	int16_t rc = code & 0xFFFF;		// code index

	if(rt < 0 || rt > 5)
		return 0;

	return rc + (tab[rt]->min * -1);
}
