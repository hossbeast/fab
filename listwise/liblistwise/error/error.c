/* Copyright (c) 2012-2014 Todd Freed <todd.freed@gmail.com>

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
#include "listwise/PCRE.errtab.h"

static etable * tab[3];

void __attribute__((constructor)) init()
{
	tab[0] = perrtab_SYS;
	tab[0]->id = 0;

	tab[1] = perrtab_PCRE;
	tab[1]->id = 1;

	tab[2] = perrtab_LW;	
	tab[2]->id = 2;
}

const char * listwise_errname(const int code)
{
	int16_t rt = code >> 16;			// table index
	int16_t rc = code & 0xFFFF;		// code index

	if(rt < 0 || rt > 2)
		return 0;

	return tab[rt]->v[rc + (tab[rt]->min * -1)].name;
}

const char * listwise_errdesc(const int code)
{
	int16_t rt = code >> 16;			// table index
	int16_t rc = code & 0xFFFF;		// code index

	if(rt < 0 || rt > 2)
		return 0;

	return tab[rt]->v[rc + (tab[rt]->min * -1)].desc;
}

const char * listwise_errstr(const int code)
{
	int16_t rt = code >> 16;			// table index
	int16_t rc = code & 0xFFFF;		// code index

	if(rt < 0 || rt > 2)
		return 0;

	return tab[rt]->v[rc + (tab[rt]->min * -1)].str;
}

const etable * listwise_errtab(const int code)
{
  int16_t rt = code >> 16;      // table index

  if(rt < 0 || rt > 2)
    return 0;

  return tab[rt];
}

int listwise_errcode(const int code)
{
  int16_t rt = code >> 16;      // table index
  int16_t rc = code & 0xFFFF;   // code index

  if(rt < 0 || rt > 2)
    return 0;

  return rc + (tab[rt]->min * -1);
}
