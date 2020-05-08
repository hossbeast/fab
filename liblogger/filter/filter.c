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
#include "xlinux/xstdlib.h"
#include "narrator.h"

#include "internal.h"
#include "filter.internal.h"

#include "LOGGER.errtab.h"
#include "category.internal.h"
#include "stream.internal.h"
#include "valyria/array.h"

#include "valyria/list.h"

#include "macros.h"

//
// public
//

void filter_free(filter * filter)
{
  wfree(filter);
}

void filter_ifree(filter ** filter)
{
  filter_free(*filter);
  *filter = 0;
}

xapi filter_say(filter * filterp, struct narrator * N)
{
  enter;

  uint64_t bit;
  logger_category *category;

  xsayc(filterp->o);

  bit = UINT64_C(1);
  while(bit)
  {
    if(bit & filterp->v)
    {
      if((category = category_byid(bit)))
      {
        if((bit - 1) & filterp->v)
          xsays(",");

        xsayf("%.*s", (int)category->namel, category->name);
      }
    }

    bit <<= 1;
  }

  finally : coda;
}

int filters_would(const list * const restrict filters, const uint64_t ids)
{
  int r = 0;

  int x;
  for(x = 0; x < filters->size; x++)
  {
    filter * filterp = list_get(filters, x);
    int rr = 0;

    // a filter with unrecognized categories cannot match
    if(filterp->u)
      continue;

    // all categories in the filter are in the log
    // the log has all of the categories and labels in the filter
    //  (which cannot be true if the filter has unrecognized categories)
    rr = ((ids & filterp->v) == filterp->v);

    // and, the filter has all of the non-supplemental categories in the log
    //  (a log cannot have unrecognized categories)
    rr &= (ids & ~category_optional_mask) == (filterp->v & ids & ~category_optional_mask);

    if(rr)
    {
      if(filterp->o == '+')
        r = 1;
      else if(filterp->o == '-')
        r = 0;
    }
  }

  return r;
}
