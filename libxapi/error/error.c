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
#include "errtab.internal.h"

//
// api
//

API const char * xapi_errname(const xapi exit)
{
	xapi_code rt = exit >> 16;			// table id

  if(rt < 1 || rt > tabl)
    return 0;

  return xapi_errtab_errname(exit, tab[rt - 1]);
}

API const char * xapi_errdesc(const xapi exit)
{
	xapi_code rt = exit >> 16;			// table id

  if(rt < 1 || rt > tabl)
    return 0;

  return xapi_errtab_errdesc(exit, tab[rt - 1]);
}

API const char * xapi_errstr(const xapi exit)
{
	xapi_code rt = exit >> 16;			// table id

  if(rt < 1 || rt > tabl)
    return 0;

  return xapi_errtab_errstr(exit, tab[rt - 1]);
}

API const etable * xapi_errtab(const xapi exit)
{
	xapi_code rt = exit >> 16;			// table id

	if(rt < 1 || rt > tabl)
		return 0;

  return tab[rt - 1];
}

API int xapi_errcode(const xapi exit)
{
	xapi_code rt = exit >> 16;			// table id

  if(rt < 1 || rt > tabl)
    return 0;

  return xapi_errtab_errcode(exit, tab[rt - 1]);
}
