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

void filter_ifree(filter ** filter)
{
  filter_free(*filter);
  *filter = 0;
}

int filter_parsew(const char * const restrict expr, size_t exprl, filter * const restrict filterp)
{
  size_t off = 0;
  char m = ' ';
  uint64_t v = 0;

  if(expr[off] != '+' && expr[off] != '-')
    return 0;

  off++;
  while(off != exprl)
  {
    // skip leading category delimiters
    while(off < exprl && (expr[off] == ',' || expr[off] == '|'))
      off++;

    // find the end of the category name
    int x;
    for(x = off; x < exprl; x++)
    {
      if((x == off || expr[x] < '0' || expr[x] > '9') && (expr[x] < 'A' || expr[x] > 'Z') && expr[x] != '_')
        break;
    }

    if(x != exprl && expr[x] != ',' && expr[x] != '|' && expr[x] != '%' && expr[x] != '^')
      return 0;

    if(estrcmp(expr + off, x - off, "ALL", 3, 0) == 0)
    {
      v |= L_ALL;
    }
    else
    {
      logger_category * category = category_byname(expr + off, x - off);
      if(category)
        v |= category->id & L_ALL;
    }

    // skip trailing category delimiters
    while(x < exprl && (expr[x] == ',' || expr[x] == '|'))
      x++;

    // trailing flag indicating the mode
    if(x < exprl && (expr[x] == '%' || expr[x] == '^'))
      m = expr[x++];

    off += (x - off);
  }

  if(off != exprl)
    return 0;

  // create a filter if any named category was found
  if(filterp)
    (*filterp) = (typeof(*filterp)) { v : v, m : m, o : expr[0] };

  return 1;
}

int filter_parses(const char * const restrict expr, filter * const restrict filterp)
{
  return filter_parsew(expr, strlen(expr), filterp);
}

xapi filter_expr_process(
    const char * const restrict expr
  , size_t exprl
  , list * const restrict listp
  , xapi (*listop)(list * const restrict, void *)
)
{
  enter;

  filter * filterp = 0;
  filter filters[8];
  int r;

  if((r = filter_expr_parse(expr, exprl, filters, sizeof(filters) / sizeof(*filters))) < 0)
  {
    xapi_fail_intent();
    xapi_info_addf("expr", "%.*s", (int)exprl, expr);
    fail(LOGGER_BADFILTER);
  }

  int x;
  for(x = 0; x < r; x++)
  {
    if(filters[x].v)
    {
      if(filterp == 0)
        fatal(xmalloc, &filterp, sizeof(*filterp));

      memcpy(filterp, &filters[x], sizeof(*filterp));
      fatal(listop, listp, filterp);
      filterp = 0;
    }
  }

finally:
  filter_free(filterp);
coda;
}

int filter_expr_parse(const char * const restrict expr, size_t exprl, filter * filters, size_t filtersz)
{
  int x;
  int y = 0;
  size_t z = 0;

  for(x = y; x < exprl; x = y)
  {
    while(x < exprl && (expr[x] == ' ' || expr[x] == '\t'))
      x++;

    y = x;
    while(y < exprl && expr[y] != ' ' && expr[y] != '\t')
      y++;
    
    if(y > x)
    {
      filter * filterp = 0;
      if(z < filtersz)
        filterp = &filters[z++];

      if(!filter_parsew(expr + x, y - x, filterp))
        return -1;
    }
  }

  return (int) z;
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
      logger_category * category = category_byid(bit);
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

xapi filter_push(int stream_id, filter * restrict filterp)
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

xapi filter_unshift(int stream_id, filter * const restrict filterp)
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

API xapi logger_filter_pushw(int stream_id, const char * const restrict expr, size_t exprl)
{
  enter;

  list * listp = 0;
  stream * streamp = 0;

  if(stream_id)
  {
    fatal(stream_byid, stream_id, &streamp);
    fatal(filter_expr_process, expr, exprl, streamp->filters, list_push);
  }
  else
  {
    fatal(list_create, &listp);
    fatal(filter_expr_process, expr, exprl, listp, list_push);

    int x;
    for(x = g_streams->l - 1; x >= 0; x--)
    {
      streamp = array_get(g_streams, x);
      fatal(list_replicate, streamp->filters, streamp->filters->l, listp, 0, listp->l);
    }
  }
 
finally:
  fatal(list_xfree, listp);
coda;
}

API xapi logger_filter_pushs(int stream_id, const char * const restrict expr)
{
  xproxy(logger_filter_pushw, stream_id, expr, strlen(expr));
}

API xapi logger_filter_unshiftw(int stream_id, const char * const restrict expr, size_t exprl)
{
  enter;

  list * listp = 0;
  stream * streamp = 0;

  if(stream_id)
  {
    fatal(stream_byid, stream_id, &streamp);
    fatal(filter_expr_process, expr, exprl, streamp->filters, list_push);
  }
  else
  {
    fatal(list_create, &listp);
    fatal(filter_expr_process, expr, exprl, listp, list_push);

    int x;
    for(x = g_streams->l - 1; x >= 0; x--)
    {
      streamp = array_get(g_streams, x);
      fatal(list_replicate, streamp->filters, 0, listp, 0, listp->l);
    }
  }
 
finally:
  fatal(list_xfree, listp);
coda;
}

API xapi logger_filter_unshifts(int stream_id, const char * const restrict expr)
{
  xproxy(logger_filter_unshiftw, stream_id, expr, strlen(expr));
}

API xapi logger_filter_pop(int stream_id)
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

API xapi logger_filter_shift(int stream_id)
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

API xapi logger_filter_clear(int stream_id)
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

API int logger_filter_validatew(const char * const restrict expr, size_t exprl)
{
  filter filters[8];
  int r;

  if((r = filter_expr_parse(expr, exprl, filters, sizeof(filters) / sizeof(*filters))) < 0)
    return 0;

  int x;
  for(x = 0; x < r; x++)
  {
    if(filters[x].v == 0)
      return 0;
  }

  return 1;
}

API int logger_filter_validates(const char * const restrict expr)
{
  return logger_filter_validatew(expr, strlen(expr));
}
