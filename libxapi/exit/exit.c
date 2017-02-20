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
#include "exit.h"
#include "errtab.internal.h"

//
// api
//

API const char * xapi_exit_errname(const xapi exit)
{
  xapi_errtab_id id = exit >> 16;     // table id

  if(id < 1 || id > tabl)
    return 0;

  return xapi_errtab_errname(tab[id - 1], exit);
}

API const char * xapi_exit_errdesc(const xapi exit)
{
  xapi_errtab_id id = exit >> 16;     // table id

  if(id < 1 || id > tabl)
    return 0;

  return xapi_errtab_errdesc(tab[id - 1], exit);
}

API const char * xapi_exit_errstr(const xapi exit)
{
  xapi_errtab_id id = exit >> 16;     // table id

  if(id < 1 || id > tabl)
    return 0;

  return xapi_errtab_errstr(tab[id - 1], exit);
}

API const errtab * xapi_exit_errtab(const xapi exit)
{
  xapi_errtab_id id = exit >> 16;     // table id

  if(id < 1 || id > tabl)
    return 0;

  return tab[id - 1];
}

API xapi_code xapi_exit_errcode(const xapi exit)
{
  xapi_errtab_id id = exit >> 16;     // table id

  if(id < 1 || id > tabl)
    return 0;

  return xapi_errtab_errcode(tab[id - 1], exit);
}

API const char * xapi_exit_errtab_name(const xapi exit)
{
  xapi_errtab_id id = exit >> 16;     // table id

  if(id < 1 || id > tabl)
    return 0;

  return tab[id - 1]->name;
}

API xapi_errtab_id xapi_exit_errtab_id(const xapi exit)
{
  xapi_errtab_id id = exit >> 16;     // table id

  if(id < 1 || id > tabl)
    return 0;

  return tab[id - 1]->id;
}

API xapi xapi_exit_synth(const errtab * const restrict etab, const xapi_code errcode)
{
  return ((etab->id) << 16) | errcode;
}
