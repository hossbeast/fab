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

#include "xapi.h"
#include "exit.internal.h"
#include "errtab.internal.h"
#include "hashtable.internal.h"

//
// api
//

const char * API xapi_exit_errname(const xapi exit)
{
  xapi_errtab_tag tag = exit >> 16;

  const errtab * tab;
  if((tab = lx_hashtable_get(&tabmap, tag)) == 0)
    return 0;

  return xapi_errtab_errname(tab, exit);
}

const char * API xapi_exit_errdesc(const xapi exit)
{
  xapi_errtab_tag tag = exit >> 16;

  const errtab * tab;
  if((tab = lx_hashtable_get(&tabmap, tag)) == 0)
    return 0;

  return xapi_errtab_errdesc(tab, exit);
}

const char * API xapi_exit_errstr(const xapi exit)
{
  xapi_errtab_tag tag = exit >> 16;

  const errtab * tab;
  if((tab = lx_hashtable_get(&tabmap, tag)) == 0)
    return 0;

  return xapi_errtab_errstr(tab, exit);
}

const errtab * API xapi_exit_errtab(const xapi exit)
{
  xapi_errtab_tag tag = exit >> 16;

  const errtab * tab;
  if((tab = lx_hashtable_get(&tabmap, tag)) == 0)
    return 0;

  return tab;
}

xapi_code API xapi_exit_errcode(const xapi exit)
{
  xapi_errtab_tag tag = exit >> 16;

  const errtab * tab;
  if((tab = lx_hashtable_get(&tabmap, tag)) == 0)
    return 0;

  return xapi_errtab_errcode(tab, exit);
}

const char * API xapi_exit_errtab_name(const xapi exit)
{
  xapi_errtab_tag tag = exit >> 16;

  const errtab * tab;
  if((tab = lx_hashtable_get(&tabmap, tag)) == 0)
    return 0;

  return tab->name;
}

xapi_errtab_tag API xapi_exit_errtab_tag(const xapi exit)
{
  xapi_errtab_tag tag = exit >> 16;

  const errtab * tab;
  if((tab = lx_hashtable_get(&tabmap, tag)) == 0)
    return 0;

  return tab->tag;
}

xapi API xapi_exit_synth(const errtab * const restrict etab, const xapi_code errcode)
{
  return ((etab->tag) << 16) | errcode;
}
