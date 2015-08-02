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
#include "info.h"

#include "macros.h"
#include "memblk.def.h"

#define restrict __restrict

//
// public
//

void info_freeze(memblk * const restrict mb, info * restrict i)
{
  memblk_freeze(mb, &i->ks);
  memblk_freeze(mb, &i->vs);
}

void info_unfreeze(memblk * const restrict mb, info * restrict i)
{
  memblk_unfreeze(mb, &i->ks);
  memblk_unfreeze(mb, &i->vs);
}

void info_thaw(char * const restrict mb, info * restrict i)
{
  memblk_thaw(mb, &i->ks);
  memblk_thaw(mb, &i->vs);
}
