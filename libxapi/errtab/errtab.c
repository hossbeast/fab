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

#include "types.h"

#include "xapi.h"
#include "errtab.internal.h"
#include "errtab/XAPI.errtab.h"
#include "mm/mm.internal.h"
#include "hashtable.internal.h"

#define TABMAP_TABLE_SIZE 512

hashtable tabmap;
static uint16_t keytab[TABMAP_TABLE_SIZE];
static void * valtab[TABMAP_TABLE_SIZE];
static bool tabmap_setup;

//
// api
//

void API xapi_errtab_stage(errtab * const restrict etab)
{
  if(!tabmap_setup)
  {
    hashtable_init(&tabmap, keytab, valtab, TABMAP_TABLE_SIZE);
    tabmap_setup = true;
  }

  if(hashtable_set(&tabmap, etab->tag, etab) == ENOMEM)
  {
    dprintf(2, "too many staged error tables ; use xapi_errtab_register\n");
  }
}

void API xapi_errtab_unstage(errtab * const restrict etab)
{
}

xapi API xapi_errtab_register(errtab * const etab)
{
  enter;

  finally : coda;
}

const errtab * API xapi_errtab_bytag(xapi_errtab_tag tag)
{
  return hashtable_get(&tabmap, tag);
}

//
// exit value api
//

const char * API xapi_errtab_errname(const errtab * const restrict etab, const xapi exit)
{
  if(etab == 0)
    return 0;

  xapi_code code = exit & 0xFFFF;

  if(code < etab->min || code > etab->max)
    return 0;

  return etab->v[code + (etab->min * -1)].name;
}

const char * API xapi_errtab_errdesc(const errtab * const restrict etab, const xapi exit)
{
  if(etab == 0)
    return 0;

  xapi_code code = exit & 0xFFFF;   // error code

  if(code < etab->min || code > etab->max)
    return 0;

  return etab->v[code + (etab->min * -1)].desc;
}

const char * API xapi_errtab_errstr(const errtab * const restrict etab, const xapi exit)
{
  if(etab == 0)
    return 0;

  xapi_code code = exit & 0xFFFF;   // error code

  if(code < etab->min || code > etab->max)
    return 0;

  return etab->v[code + (etab->min * -1)].str;
}

xapi_code API xapi_errtab_errcode(const errtab * const restrict etab, const xapi exit)
{
  if(etab == 0)
    return 0;

  xapi_code code = exit & 0xFFFF;   // error code

  if(code < etab->min || code > etab->max)
    return 0;

  return code;
}
