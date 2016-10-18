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

#include "errtab/LOGGER.errtab.h"
#include "category.internal.h"
#include "stream.internal.h"
#include "valyria/array.h"

#include "valyria/list.h"

#include "macros.h"
#include "strutil.h"

#define restrict __restrict

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
  wfree(filter);
}

void __attribute__((unused)) filter_ifree(filter ** filter)
{
  filter_free(*filter);
  *filter = 0;
}

xapi filter_parsew(const char * const restrict expr, size_t exprl, filter ** const restrict filterp, size_t * const restrict offp)
{
  enter;

  // track characters used
  size_t off = 0;

  // skip leading whitespace
  while(off < exprl && expr[off] == ' ')
    off++;

  if(exprl && (expr[off] == '+' || expr[off] == '-'))
  {
    char m = ' ';
    char o = expr[off++];

    // start of category names
    size_t cat_off = off;

    uint64_t category_ids = 0;
    while(off != exprl && expr[off] != ' ')
    {
      // skip leading category delimiters
      for(; off < exprl; off++)
      {
        if(expr[off] != ',' && expr[off] != '|')
          break;
      }

      // find the end of the category name
      int x;
      for(x = off; x < exprl; x++)
      {
        // valid characters in a category name
        if((expr[x] < 'A' || expr[x] > 'Z') && expr[x] != '_')
          break;
      }

      if(x == off)
      {
        break;
      }
      else if(x == exprl || expr[x] == ',' || expr[x] == '|' || expr[x] == '%' || expr[x] == '^' || expr[x] == ' ')
      {
        if(estrcmp(expr + off, x - off, "ALL", 3, 0) == 0)
        {
          category_ids |= L_ALL;
        }
        else
        {
          logger_category * category = 0;
          fatal(category_byname, expr + off, x - off, &category);
          if(category)
            category_ids |= category->id;
        }
      }

      // skip trailing category delimiters
      while(expr[x] == ',' || expr[x] == '|')
        x++;

      // trailing flag indicating the mode
      if(x < exprl && (expr[x] == '%' || expr[x] == '^'))
        m = expr[x++];

      off += (x - off);
    }

    // create a filter if any named category was found
    if(category_ids && filterp)
    {
      fatal(xmalloc, filterp, sizeof(**filterp));

      (**filterp) = (typeof(**filterp)){
          v : category_ids & L_ALL
        , m : m
        , o : o
      };
    }

    if(off > cat_off)
      if(offp)
        *offp = off;
  }

  finally : coda;
}

xapi filter_parses(const char * const restrict expr, filter ** const restrict filterp, size_t * const restrict offp)
{
  xproxy(filter_parsew, expr, strlen(expr), filterp, offp);
}

xapi filter_say(filter * filterp, struct narrator * N)
{
  enter;

  sayc(filterp->o);

  uint64_t bit = UINT64_C(1);
  while(bit)
  {
    if(bit & filterp->v)
    {
      logger_category * category = 0;
      fatal(category_byid, bit, &category);

      if(category)
      {
        if((bit - 1) & filterp->v)
          says(",");

        sayf("%.*s", category->namel, category->name);
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
  for(x = 0; x < filters->l; x++)
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

xapi filter_push(const int stream_id, filter * restrict filterp)
{
  enter;

  filter * clone = 0;

  if(stream_id)
  {
    stream * streamp = 0;
    fatal(stream_byid, stream_id, &streamp);

    fatal(stream_filter_push, streamp, filterp);
    filterp = 0;
  }
  else
  {
    int x;
    for(x = g_streams->l - 1; x >= 0; x--)
    {
      if(x)
      {
        fatal(filter_clone, filterp, &clone);
      }
      else
      {
        clone = filterp;
        filterp = 0;
      }

      fatal(stream_filter_push, (stream*)array_get(g_streams, x), clone);
      clone = 0;
    }
  }
 
finally:
  filter_free(filterp);
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

API xapi logger_filter_pushw(const int stream_id, const char * const restrict expr, size_t exprl)
{
  enter;

  filter * filterp = 0;

  size_t x;
  for(x = 0; x < exprl; x++)
  {
    size_t off = 0;
    fatal(filter_parsew, expr + x, exprl - x, &filterp, &off);

    if(!filterp)
      fail(LOGGER_BADFILTER);

    fatal(filter_push, stream_id, filterp);
    filterp = 0;

    x += off;
  }

finally:
  filter_free(filterp);
  xapi_infof("expr", "%.*s", (int)exprl, expr);
coda;
}

API xapi logger_filter_pushs(const int stream_id, const char * const restrict expr)
{
  xproxy(logger_filter_pushw, stream_id, expr, strlen(expr));
}

API xapi logger_filter_unshiftw(const int stream_id, const char * const restrict expr, size_t exprl)
{
  enter;

  filter * filterp = 0;

  size_t x;
  for(x = 0; x < exprl; x++)
  {
    size_t off = 0;
    fatal(filter_parsew, expr + x, exprl - x, &filterp, &off);

    if(!filterp)
      fail(LOGGER_BADFILTER);

    fatal(filter_unshift, stream_id, filterp);
    filterp = 0;

    x += (off - x) + 1;
  }

finally:
  filter_free(filterp);
  xapi_infof("expr", "%.*s", (int)exprl, expr);
coda;
}

API xapi logger_filter_unshifts(const int stream_id, const char * const restrict expr)
{
  xproxy(logger_filter_unshiftw, stream_id, expr, strlen(expr));
}

API xapi logger_filter_pop(const int stream_id)
{
  enter;

  if(stream_id)
  {
    stream * streamp = 0;
    fatal(stream_byid, stream_id, &streamp);
    fatal(list_pop, streamp->filters, 0);
  }
  else
  {
    int x;
    for(x = 0; x < g_streams->l; x++)
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
    fatal(list_shift, streamp->filters, 0);
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
    fatal(list_recycle, streamp->filters);
  }
  else
  {
    int x;
    for(x = 0; x < array_size(g_streams); x++)
    {
      fatal(list_recycle, ((stream *)array_get(g_streams, x))->filters);
    }
  }
  
  finally : coda;
}
