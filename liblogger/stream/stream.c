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

#include <inttypes.h>
#include <stdio.h>

#include "xapi.h"
#include "xlinux.h"
#include "narrate.h"
#include "narrate/file.h"
#include "pstring.h"

#include "internal.h"
#include "stream/stream.internal.h"
#include "attr/attr.internal.h"
#include "filter/filter.internal.h"
#include "category/category.internal.h"

#define LIST_ELEMENT_TYPE stream*
#include "list.h"
#include "macros.h"
#include "strutil.h"
#include "map.h"

#define restrict __restrict

// active streams
list * g_streams;

//
// static
//

static map * streams_byid;

static xapi __attribute__((nonnull)) stream_write(stream * const restrict streamp, const uint64_t ids, const uint32_t base_attr, const pstring * const restrict message, const long time_msec)
{
  enter;

  uint32_t attr = attr_combine(base_attr, streamp->attr);

narrationd(1);
sayf("base 0x%08lx ", base_attr);
fatal(attr_say, base_attr, _narrator);
says("\n");

sayf("effe 0x%08lx ", attr);
fatal(attr_say, attr, _narrator);
says("\n");

  fatal(psclear, &streamp->buffer);

  if(CATEGORY_OPT & attr)
  {
    uint64_t bit = UINT64_C(1);
    while(bit)
    {
      if(bit & ids)
        break;

      bit <<= 1;
    }

    logger_category * category = 0;
    fatal(category_byid, bit, &category);
    fatal(pscatw, &streamp->buffer, category->name, category->namel);
  }

  fatal(pscatw, &streamp->buffer, message->s, message->l);
  fatal(pscatc, &streamp->buffer, '\n');

  fatal(narrate_sayw, streamp->narrator, streamp->buffer->s, streamp->buffer->l);

  finally : coda;
}

//
// public
//
xapi stream_setup()
{
  enter;

  fatal(list_create, &g_streams, sizeof(stream), 0, LIST_PRIMARY);
  fatal(map_create, &streams_byid, 0);

  finally : coda;
}

void stream_teardown()
{
  list_xfree(&g_streams);
  map_xfree(&streams_byid);
}

xapi streams_write(const uint64_t ids, const uint32_t site_attr, const pstring * restrict message, const long time_msec)
{
  enter;

  if(streams_would(ids))
  {
    /* combine attributes from various places in increasing order of precedence to get the effective attributes */
    uint32_t base_attr = 0;

    // log categories
    uint64_t copy = ids;
    uint64_t bit = UINT64_C(1) << 63;
    while(copy)
    {
      if(copy & bit)
      {
        logger_category * category = 0;
        fatal(category_byid, bit, &category);
        if(category)
          base_attr = attr_combine(base_attr, category->attr);

        copy &= ~bit;
      }
      bit >>= 1;
    }

    // log site
    base_attr = attr_combine(base_attr, site_attr);

    int x;
    for(x = 0; x < list_size(g_streams); x++)
    {
      stream * streamp = list_get(g_streams, x);
      if(stream_would(streamp, ids))
      {
        fatal(stream_write, streamp, ids, base_attr, message, time_msec);
      }
    }

    if(list_size(g_streams) == 0)
    {
      int __attribute__((unused)) _r = write(2, message->s, message->l);
      _r = write(2, "\n", 1);
    }
  }

  finally : coda;
}

int stream_would(const stream * const restrict streamp, const uint64_t ids)
{
  if(!streamp->filters)
    return 1;

  return filters_would(streamp->filters, ids);
}

int streams_would(const uint64_t ids)
{
  if(list_size(g_streams) == 0)
    return 1;

  int x;
  for(x = 0; x < list_size(g_streams); x++)
  {
    stream * streamp = list_get(g_streams, x);
    if(stream_would(streamp, ids))
      return 1;
  }

  return 0;
}

xapi stream_byid(int id, stream ** const restrict streamp)
{
  enter;

  (*streamp) = map_get(streams_byid, MM(id));

  finally : coda;
}

//
// api
//

API xapi logger_stream_register(const logger_stream * restrict streams)
{
  enter;

  while(streams->type)
  {
    stream * streamp = 0;
    fatal(list_push, g_streams, &streamp);

    fatal(ixstrdup, &streamp->name, streams->name);
    streamp->namel = strlen(streamp->name);
    streamp->attr = streams->attr;

    if(streams->type == LOGGER_STREAM_FD)
    {
      fatal(narrator_file_create, streams->fd, &streamp->narrator);
    }

    if(streams->filter_expr)
    {
      // parse and attach to just this stream
    }

    fatal(map_set, streams_byid, MM(streamp->id), MM(streamp), 0);

    streams++;
  }

  finally : coda;
}
