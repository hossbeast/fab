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

#include <stdlib.h>
#include <stdint.h>

#include "internal.h"
#include "error.h"
#include "errtab.h"

//
// api
//

API const char * xapi_errname(const int exit)
{
	int16_t rt = exit >> 16;			// table id
	int16_t rc = exit & 0xFFFF;		// error code

  if(rt < 1 || rt > tabl)
    return 0;

  if(rc < tab[rt - 1]->min || rc > tab[rt - 1]->max)
    return 0;

	return tab[rt - 1]->v[rc + (tab[rt - 1]->min * -1)].name;
}

API const char * xapi_errdesc(const int exit)
{
	int16_t rt = exit >> 16;			// table id
	int16_t rc = exit & 0xFFFF;		// error code

  if(rt < 1 || rt > tabl)
    return 0;

  if(rc < tab[rt - 1]->min || rc > tab[rt - 1]->max)
    return 0;

	return tab[rt - 1]->v[rc + (tab[rt - 1]->min * -1)].desc;
}

API const char * xapi_errstr(const int exit)
{
	int16_t rt = exit >> 16;			// table id
	int16_t rc = exit & 0xFFFF;		// error code

  if(rt < 1 || rt > tabl)
    return 0;

  if(rc < tab[rt - 1]->min || rc > tab[rt - 1]->max)
    return 0;

	return tab[rt - 1]->v[rc + (tab[rt - 1]->min * -1)].str;
}

API const etable * xapi_errtab(const int exit)
{
	int16_t rt = exit >> 16;			// table index

	if(rt < 1 || rt > tabl)
		return 0;

	return tab[rt - 1];
}

API int xapi_errcode(const int exit)
{
	int16_t rt = exit >> 16;			// table id
	int16_t rc = exit & 0xFFFF;		// error code

  if(rt < 1 || rt > tabl)
    return 0;

  if(rc < tab[rt - 1]->min || rc > tab[rt - 1]->max)
    return 0;

	return rc;
}

API const etable * xapi_errtab_byid(const int id)
{
	if(id < 1 || id > tabl)
    return 0;

	return tab[id - 1];
}
