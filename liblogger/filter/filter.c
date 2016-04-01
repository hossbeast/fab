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
#include "narrator.h"

#include "internal.h"
#include "filter.internal.h"

#include "errtab/LOGGER.errtab.h"
#include "category.internal.h"
#include "stream.internal.h"

#define LIST_ELEMENT_TYPE filter
#include "valyria/list.h"

#define ARRAY_ELEMENT_TYPE stream
#include "valyria/array.h"

#include "macros.h"
#include "strutil.h"

#define restrict __restrict

typedef struct filter
{
	uint64_t	v;		// tag
  char      m;    // mode, ( or % or ^
	char			o;		// operation, + or -
} filter;

//
// [[ static ]]
//

static xapi filter_clone(filter * const restrict A, filter ** const restrict B)
{
  enter;

  filter * filterp = 0;
  fatal(xmalloc, &filterp, sizeof(*filterp));
  memcpy(filterp, A, sizeof(*filterp));
  (*B) = filterp;
  filterp = 0;

finally:
  filter_free(filterp);
coda;
}

//
// [[ public ]]
//

void filter_free(filter * filter)
{
  free(filter);
}

void __attribute__((unused)) filter_xfree(filter ** filter)
{
  filter_free(*filter);
  *filter = 0;
}

xapi filter_parse(const char * const restrict expr, size_t exprl, filter ** const restrict res)
{
  enter;

  exprl = exprl ?: strlen(expr);

  // skip leading whitespace
  int off = 0;
  while(exprl > off && (expr[off] == ' ' || expr[off] == '\t'))
    off++;

  if(exprl > off && (expr[off] == '+' || expr[off] == '-'))
  {
    char o = expr[off];
    off++;

    if(off < exprl)
    {
      int y;
      for(y = off; y < exprl; y++)
      {
        if(expr[y] == ' ' || expr[y] == '\t')
          break;
      }

      uint64_t tag = 0;
      int k = 0;
      while(k != y)
      {
        if(expr[off] == ',' || expr[off] == '|')
          off++;

        for(k = off; k < y; k++)
        {
          if(expr[k] == ',' || expr[k] == '|')
            break;
        }

        if(estrcmp(&expr[off], k-off, "ALL", 3, 0) == 0)
        {
          tag |= L_ALL;
          off += 3;
        }
        else
        {
          logger_category * category = 0;
          fatal(category_byname, &expr[off], k - off, &category);

          if(category == 0)
            break; // nothing

          tag |= category->id;
          off += category->namel;
        }
      }

      if(tag && (off == y))
      {
        // allocate
        fatal(xmalloc, res, sizeof(**res));

        (**res) = (typeof(**res)){
            v : tag & L_ALL
          , o : o
        };
      }
    }
  }

  finally : coda;
}

xapi filter_say(filter * filterp, struct narrator * N)
{
  enter;

  sayc(filterp->o);

  uint64_t mask = filterp->v;
  uint64_t id = UINT64_C(1);
  while(mask)
  {
    if(mask & id)
    {
      logger_category * category = 0;
      fatal(category_byid, mask & id, &category);
      if(category)
      {
        sayf("%.*s", category->namel, category->name);
      }

      mask &= ~id;
    }
    id <<= 1;
  }

  finally : coda;
}

int filters_would(const list * const restrict filters, const uint64_t ids)
{
  int r = 1;

  int x;
  for(x = 0; x < list_size(filters); x++)
  {
    filter * filterp = list_get(filters, x);
    uint64_t rr = 0;
    if(filterp->m == ' ')
      rr = ids & filterp->v & L_ALL;
    else if(filterp->m == '^')
      rr = ((ids & filterp->v & L_ALL) == (filterp->v & L_ALL));
    else if(filterp->m == '%')
      rr = ((ids & filterp->v & L_ALL) == (ids & L_ALL));

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


xapi filter_push(const int stream_id, filter * const restrict filterp)
{
  enter;

  filter * clone = 0;

  if(stream_id)
  {
    stream * streamp = 0;
    fatal(stream_byid, stream_id, &streamp);

    fatal(list_push, streamp->filters, filterp);
  }
  else
  {
    int x;
    for(x = array_size(g_streams) - 1; x >= 0; x--)
    {
      if(x)
      {
        fatal(filter_clone, filterp, &clone);
      }
      else
      {
        clone = filterp;
      }

      fatal(list_push, ((stream *)array_get(g_streams, x))->filters, clone);
      clone = 0;
    }
  }
 
finally:
  filter_free(clone);
coda;
}

xapi filter_unshift(const int stream_id, filter * const restrict filterp)
{
  enter;

  filter * clone = 0;

  if(stream_id)
  {
    stream * streamp = 0;
    fatal(stream_byid, stream_id, &streamp);

    fatal(list_unshift, streamp->filters, filterp);
  }
  else
  {
    int x;
    for(x = array_size(g_streams) - 1; x >= 0; x--)
    {
      if(x)
      {
        fatal(filter_clone, filterp, &clone);
      }
      else
      {
        clone = filterp;
      }

      fatal(list_unshift, ((stream *)array_get(g_streams, x))->filters, clone);
      clone = 0;
    }
  }
 
finally:
  filter_free(clone);
coda;
}

//
// api
//

API xapi logger_filter_push(const int stream_id, const char * const restrict expr, size_t exprl)
{
  enter;

  filter * filterp = 0;
  fatal(filter_parse, expr, exprl, &filterp);

  if(!filterp)
    fail(LOGGER_BADFILTER);

  fatal(filter_push, stream_id, filterp);
  filterp = 0;

finally:
  filter_free(filterp);
coda;
}

API xapi logger_filter_unshift(const int stream_id, const char * const restrict expr, size_t exprl)
{
  enter;

  filter * filterp = 0;
  fatal(filter_parse, expr, exprl, &filterp);

  if(!filterp)
    fail(LOGGER_BADFILTER);

finally:
  filter_free(filterp);
coda;
}

API xapi logger_filter_pop(const int stream_id)
{
  enter;

  if(stream_id)
  {
    stream * streamp = 0;
    fatal(stream_byid, stream_id, &streamp);
  }
  else
  {
    int x;
    for(x = 0; x < array_size(g_streams); x++)
    {
      fatal(list_pop, ((stream *)array_get(g_streams, x))->filters, 0);
    }
  }
  
  finally : coda;
}

API xapi logger_filter_shift(const int stream_id)
{
  enter;

  if(stream_id)
  {
    stream * streamp = 0;
    fatal(stream_byid, stream_id, &streamp);
  }
  else
  {
    int x;
    for(x = 0; x < array_size(g_streams); x++)
    {
      fatal(list_shift, ((stream *)array_get(g_streams, x))->filters, 0);
    }
  }
  
  finally : coda;
}

API xapi logger_filter_clear(const int stream_id)
{
  enter;

  if(stream_id)
  {
    stream * streamp = 0;
    fatal(stream_byid, stream_id, &streamp);
  }
  else
  {
    int x;
    for(x = 0; x < array_size(g_streams); x++)
    {
      list_clear(((stream *)array_get(g_streams, x))->filters);
    }
  }
  
  finally : coda;
}
