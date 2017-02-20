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
#include <string.h>

#include "internal.h"
#include "errtab.internal.h"
#include "errtab/XAPI.errtab.h"
#include "mm/mm.internal.h"

errtab **         tab;
size_t            tabl;
static size_t     taba;

static errtab *   tab_stage[64];
static size_t     tab_stagel;

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

//
// public
//

void errtab_teardown()
{
  if(tab != tab_stage)
    free(tab);
  tab = 0;
}

//
// api
//

API void xapi_errtab_stage(errtab * const etab)
{
  if(tab_stagel == (sizeof(tab_stage) / sizeof(tab_stage[0])) - 1)
  {
    dprintf(2, "too many staged error tables ; use xapi_errtab_register\n");
    return;
  }

  tab_stage[tab_stagel] = etab;
  errtab_prepare(tab_stage[tab_stagel], tab_stagel + 1);
  tab_stagel++;

  tab = tab_stage;
  tabl = tab_stagel;
}

API xapi xapi_errtab_register(errtab * const etab)
{
  enter;

  if(tab == tab_stage)
  {
    size_t ns = tab_stagel + 1;
    ns = ns * 2 + ns / 2;
    if((tab = calloc(sizeof(*tab), ns)) == 0)
      fail(XAPI_NOMEM);
    taba = ns;

    memcpy(tab, tab_stage, sizeof(*tab) * tab_stagel);
    tabl = tab_stagel;
  }

  if(tabl == taba)
  {
    size_t ns = taba + 1;
    ns = ns * 2 + ns / 2;
    if((tab = realloc(tab, sizeof(*tab) * ns)) == 0)
      fail(XAPI_NOMEM);
    taba = ns;
  }

  tab[tabl] = etab;
  errtab_prepare(tab[tabl], tabl + 1);
  tabl++;

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
