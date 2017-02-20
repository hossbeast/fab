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

#include <stdarg.h>
#include <inttypes.h>

#include "narrator.h"

#include "internal.h"
#include "operator.internal.h"

#define restrict __restrict

///
/// public
///

xapi listwise_lwop_say(uint64_t optype, int effectual, narrator * const restrict N)
{
  enter;

  uint64_t t = 0x01;
  int said = 0;

  while(t)
  {
    if(optype & t)
    {
      if(t <= LWOP_EFFECTUAL || !effectual)
      {
        if(said)
          says("|");
        says(LWOPT_STR(t));
        said = 1;
      }
    }

    t <<= 1;
  }

  finally : coda;
}
