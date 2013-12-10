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

#include "listwise/internal.h"

static etable * tab[2] = {
	  [ 0 ] = perrtab_SYS
	, [ 1 ] = perrtab_PCRE
	, [ 2 ] = perrtab_LW
};

void __attribute__((constructor)) init()
{
	perrtab_SYS->id = 0;
	perrtab_PCRE->id = 1;
	perrtab_LW->id = 2;
}

const char * API listwise_errname(const int code)
{
	int16_t rt = code >> 16;			// table index
	int16_t rc = code & 0xFFFF;		// code index

	if(rt < 0 || rt > 2)
		return 0;

	return tab[rt]->v[rc + tab[rt]->jump]->name;
}

const char * API listwise_errdesc(const int code)
{
	int16_t rt = code >> 16;			// table index
	int16_t rc = code & 0xFFFF;		// code index

	if(rt < 0 || rt > 2)
		return 0;

	return tab[rt]->v[rc + tab[rt]->jump]->desc;
}

const char * API listwise_errstr(const int code)
{
	int16_t rt = code >> 16;			// table index
	int16_t rc = code & 0xFFFF;		// code index

	if(rt < 0 || rt > 2)
		return 0;

	return tab[rt]->v[rc + tab[rt]->jump]->str;
}
