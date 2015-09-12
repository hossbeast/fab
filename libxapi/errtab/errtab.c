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
#include "errtab.h"
#include "error/XAPI.errtab.h"

etable **         tab;
size_t            tabl;
static size_t     taba;

static etable *   tab_stat[2];

#define restrict __restrict

//
// static
//

static void __attribute__((constructor)) init()
{
  tab_stat[0] = perrtab_SYS;
  tab_stat[0]->id = 1;

  tab_stat[1] = perrtab_XAPI;
  tab_stat[1]->id = 2;

  tab = tab_stat;
  tabl = 2;
}


//
// public
//

void errtab_teardown()
{
  if(tab != tab_stat)
    free(tab);
}

//
// api
//

API xapi xapi_errtab_register(etable * const etab)
{
  enter;

  if(tab == tab_stat)
  {
    tab = 0;
    fatal(xapi_errtab_register, perrtab_XAPI); 
    fatal(xapi_errtab_register, perrtab_SYS); 
  }

  if(tabl == taba)
  {
    size_t ns = taba ?: 3;
    ns = ns * 2 + ns / 2;
    if((tab = realloc(tab, ns * sizeof(*tab))) == 0)
    {
      fail(SYS_ENOMEM);
    }
    taba = ns;
  }

  tab[tabl++] = etab;
  etab->id = tabl;

  finally : coda;
}

API char * xapi_errtab_tag(const etable * const restrict etab)
{
  if(etab)
    return etab->tag;

  return 0;
}
