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
#include "errtab.internal.h"
#include "errtab/XAPI.errtab.h"
#include "mm/mm.internal.h"

errtab **         tab;
size_t            tabl;
static size_t     taba;

static errtab *   tab_stat[1];

#define restrict __restrict

//
// static
//

static void errtab_prepare(errtab * const restrict etab, xapi_errtab_id id)
{
  etab->id = id;

  int x;
  for(x = 0; x <= (etab->max - etab->min); x++)
  {
    etab->v[x].exit &= 0xFFFF;
    etab->v[x].exit |= ((etab->id) << 16);
  }
}

static void __attribute__((constructor)) init()
{
  tab_stat[0] = perrtab_XAPI;
  tab = tab_stat;
  tabl = 1;

  errtab_prepare(tab[0], 1);
}


//
// public
//

void errtab_teardown()
{
  if(tab != tab_stat)
  {
    free(tab);
    tab = 0;
  }
}

//
// api
//

API xapi xapi_errtab_register(errtab * const etab)
{
  enter;

  errtab * reg[2];
  size_t regl = 0;

  if(tab == tab_stat)
  {
    tab = 0;
    tabl = 0;
    reg[regl++] = perrtab_XAPI;
  }
  reg[regl++] = etab;

  int x;
  for(x = 0; x < regl; x++)
  {
    if(tabl == taba)
    {
      size_t ns = taba ?: 3;
      ns = ns * 2 + ns / 2;
      if((tab = realloc(tab, sizeof(*tab) * ns)) == 0)
        fail(XAPI_NOMEM);
      taba = ns;
    }

    tab[tabl++] = reg[x];
    errtab_prepare(reg[x], tabl);
  }

  finally : coda;
}

API const errtab * xapi_errtab_byid(const xapi_errtab_id id)
{
  if(id < 1 || id > tabl)
    return 0;

  return tab[id - 1];
}

//
// exit value api
//

API const char * xapi_errtab_errname(const errtab * const restrict etab, const xapi exit)
{
  xapi_code code = exit & 0xFFFF;   // error code

  if(code < etab->min || code > etab->max)
    return 0;

  return etab->v[code + (etab->min * -1)].name;
}

API const char * xapi_errtab_errdesc(const errtab * const restrict etab, const xapi exit)
{
  xapi_code code = exit & 0xFFFF;   // error code

  if(code < etab->min || code > etab->max)
    return 0;

  return etab->v[code + (etab->min * -1)].desc;
}

API const char * xapi_errtab_errstr(const errtab * const restrict etab, const xapi exit)
{
  xapi_code code = exit & 0xFFFF;   // error code

  if(code < etab->min || code > etab->max)
    return 0;

  return etab->v[code + (etab->min * -1)].str;
}

API xapi_code xapi_errtab_errcode(const errtab * const restrict etab, const xapi exit)
{
  xapi_code code = exit & 0xFFFF;   // error code

  if(code < etab->min || code > etab->max)
    return 0;

  return code;
}
