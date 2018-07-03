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

#include "internal.h"
#include "error.internal.h"
#include "errtab.internal.h"

#include "memblk.def.h"

#define restrict __restrict

//
// public
//

void error_freeze(memblk * const restrict mb, error * restrict e)
{
  /*
  ** etab is allocated outside of the memblk and must be handled specially
  */
  e->etab = (void*)(intptr_t)e->etab->tag;
}

void error_unfreeze(memblk * const restrict mb, error * restrict e)
{
  e->etab = xapi_errtab_bytag((intptr_t)e->etab);
}

void error_thaw(char * const restrict mb, error * restrict e)
{
  e->etab = xapi_errtab_bytag((intptr_t)e->etab);
}

xapi error_errval(const error * const restrict e)
{
  return (e->etab->tag << 16) | e->code;
}
