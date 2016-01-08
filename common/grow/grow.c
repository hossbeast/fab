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
#include "xlinux.h"

#include "grow.h"
#include "ensure.h"

#define SEED 10
#define restrict __restrict

xapi grow(void * target, size_t es, size_t len, size_t ec, size_t * const restrict ac)
{
  xproxy(ensure, target, es, ec + len, ac);
}

xapi grow2(void * target, size_t es, size_t len, size_t ec, size_t * const restrict ac, size_t seed)
{
  xproxy(ensure2, target, es, ec + len, ac, seed);
}
