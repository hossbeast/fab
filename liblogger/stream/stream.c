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
#include "narrator.h"
#include "narrator/file.h"
#include "valyria/pstring.h"

#include "internal.h"
#include "stream/stream.internal.h"
#include "attr/attr.internal.h"
#include "filter/filter.internal.h"
#include "category/category.internal.h"

#define ARRAY_ELEMENT_TYPE stream
#include "valyria/array.h"

#define MAP_VALUE_TYPE stream
#include "valyria/map.h"

#include "valyria/list.h"

#include "macros.h"
#include "strutil.h"
#include "color.h"

#define restrict __restrict

// active streams
array * g_streams;

//
// static
//

static map * streams_byid;

/// stream_write
//
// SUMMARY
//  write a log message to a stream
//
// PARAMETERS
//  streamp    - stream to write to
//  ids        - bitmask of ids for the message
//  base_attr  - category attributes + log site attributes
//  message    - 
//  time_msec  - 
//
static xapi __attribute__((nonnull)) stream_write(stream * const restrict streamp, const uint64_t ids, const uint32_t base_attr, const pstring * const restrict message, const long time_msec)
{
  enter;

  // effective attributes : category + log site + stream
  uint32_t attr = attr_combine(base_attr, streamp->attr);

  fatal(psclear, streamp->buffer);

  int prev = 0;
  if(attr & COLOR_OPT)
  {
    if((attr & COLOR_OPT) == L_RED)
      fatal(pscatw, streamp->buffer, COLOR(RED));
    else if((attr & COLOR_OPT) == L_GREEN)
      fatal(pscatw, streamp->buffer, COLOR(GREEN));
    else if((attr & COLOR_OPT) == L_YELLOW)
      fatal(pscatw, streamp->buffer, COLOR(YELLOW));
    else if((attr & COLOR_OPT) == L_BLUE)
      fatal(pscatw, streamp->buffer, COLOR(BLUE));
    else if((attr & COLOR_OPT) == L_MAGENTA)
      fatal(pscatw, streamp->buffer, COLOR(MAGENTA));
    else if((attr & COLOR_OPT) == L_CYAN)
      fatal(pscatw, streamp->buffer, COLOR(CYAN));
    else if((attr & COLOR_OPT) == L_WHITE)
      fatal(pscatw, streamp->buffer, COLOR(WHITE));

    else if((attr & COLOR_OPT) == L_BOLD_RED)
      fatal(pscatw, streamp->buffer, COLOR(BOLD_RED));
    else if((attr & COLOR_OPT) == L_BOLD_GREEN)
      fatal(pscatw, streamp->buffer, COLOR(BOLD_GREEN));
    else if((attr & COLOR_OPT) == L_BOLD_YELLOW)
      fatal(pscatw, streamp->buffer, COLOR(BOLD_YELLOW));
    else if((attr & COLOR_OPT) == L_BOLD_BLUE)
      fatal(pscatw, streamp->buffer, COLOR(BOLD_BLUE));
    else if((attr & COLOR_OPT) == L_BOLD_MAGENTA)
      fatal(pscatw, streamp->buffer, COLOR(BOLD_MAGENTA));
    else if((attr & COLOR_OPT) == L_BOLD_CYAN)
      fatal(pscatw, streamp->buffer, COLOR(BOLD_CYAN));
    else if((attr & COLOR_OPT) == L_BOLD_WHITE)
      fatal(pscatw, streamp->buffer, COLOR(BOLD_WHITE));
  }

  if(attr & DATESTAMP_OPT)
  {
    struct tm tm;
    time_t time = time_msec / 1000;
    localtime_r(&time, &tm);

    char * months[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun"
      , "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    fatal(pscatf, streamp->buffer, "%4d %s %02d %02d:%02d:%02d"
      , tm.tm_year + 1900
      , months[tm.tm_mon]
      , tm.tm_mday
      , tm.tm_hour
      , tm.tm_min
      , tm.tm_sec
    );
    prev = 1;
  }

  if(attr & CATEGORY_OPT)
  {
    // emit the name of the category with the least id
    uint64_t bit = UINT64_C(1);
    while(bit)
    {
      if(bit & ids)
        break;

      bit <<= 1;
    }

    logger_category * category = 0;
    fatal(category_byid, bit, &category);
    if(prev)
      fatal(pscats, streamp->buffer, " ");
    fatal(pscatf, streamp->buffer, "%*.*s", category_name_max_length, category->namel, category->name);
    prev = 1;
  }

  if(prev)
    fatal(pscats, streamp->buffer, " ");

  // the mssage
  fatal(pscatw, streamp->buffer, message->s, message->l);

  if(attr & DISCOVERY_OPT)
  {

  }

  // message terminator
  if(streamp->type == LOGGER_STREAM_FD)
  {
    fatal(pscatc, streamp->buffer, '\n');
  }

  if(attr & COLOR_OPT)
  {
    fatal(pscatw, streamp->buffer, COLOR(NONE));
  }

  fatal(narrator_sayw, streamp->narrator, streamp->buffer->s, streamp->buffer->l);

  finally : coda;
}

static xapi __attribute__((nonnull)) stream_initialize(stream * const restrict streamp, const logger_stream * restrict def)
{
  enter;

  if(def->name)
  {
    fatal(ixstrdup, &streamp->name, def->name);
    streamp->namel = strlen(streamp->name);
  }
  streamp->attr = def->attr;
  fatal(pscreate, &streamp->buffer);

  streamp->type = def->type;
  if(def->type == LOGGER_STREAM_FD)
  {
    fatal(narrator_file_create, &streamp->narrator, def->fd);
  }

  if(def->filter_expr)
  {
    // parse and attach to just this stream
  }

  finally : coda;
}

static void __attribute__((nonnull)) stream_destroy(stream * const restrict streamp)
{
  if(streamp)
  {
    free(streamp->name);
    list_free(streamp->filters);
    psfree(streamp->buffer);
    narrator_free(streamp->narrator);
  }
}

//
// public
//
xapi stream_setup()
{
  enter;

  fatal(array_createx, &g_streams, sizeof(stream), stream_destroy, 0);
  fatal(map_create, &streams_byid);

  finally : coda;
}

void stream_teardown()
{
  array_xfree(&g_streams);
  map_xfree(&streams_byid);
}

xapi streams_write(const uint64_t ids, const uint32_t site_attr, const pstring * restrict message, const long time_msec)
{
  enter;

  if(streams_would(ids))
  {
    // base attributes : category attributes + log site attributes
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
    for(x = 0; x < array_size(g_streams); x++)
    {
      stream * streamp = array_get(g_streams, x);
      if(stream_would(streamp, ids))
      {
        fatal(stream_write, streamp, ids, base_attr, message, time_msec);
      }
    }

    if(array_size(g_streams) == 0)
    {
      // the default is to write everything to stderr
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
  // the default is to log everything
  if(array_size(g_streams) == 0)
    return 1;

  int x;
  for(x = 0; x < array_size(g_streams); x++)
  {
    stream * streamp = array_get(g_streams, x);
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
    fatal(array_push, g_streams, &streamp);

    fatal(stream_initialize, streamp, streams);

    fatal(map_set, streams_byid, MM(streamp->id), streamp);

    streams++;
  }

  finally : coda;
}
