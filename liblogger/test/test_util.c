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
#include "xapi/errtab.h"

#include "test_util.h"

#define restrict __restrict

const char * errtab_name(const etable * const restrict etab)
{
  if(etab == 0)
    return 0;

  return xapi_errtab_name(etab);
}

const char * errtab_errname(const etable * const restrict etab, const xapi exit)
{
  if(etab == 0)
    return 0;

  return xapi_errtab_errname(etab, exit);
}
