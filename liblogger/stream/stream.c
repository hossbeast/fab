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
#include <time.h>

#include "xapi.h"
#include "xlinux/xstring.h"
#include "xlinux/xstdlib.h"
#include "narrator.h"
#include "narrator/fd.h"
#include "narrator/rolling.h"
#include "narrator/fixed.h"
#include "narrator/record.h"
#include "valyria/pstring.h"

#include "internal.h"
#include "stream/stream.internal.h"
#include "attr/attr.internal.h"
#include "filter/filter.internal.h"
#include "category/category.internal.h"
#include "log/log.internal.h"
#include "logging/logging.internal.h"
#include "valyria/array.h"
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

// registered, not yet activated streams
static list * registered;

/// stream_write
//
// SUMMARY
//  write a log message to a stream
//
// PARAMETERS
//  streamp    - stream to write to
//  ids        - bitmask of ids for the message
//  base_attr  - category attributes + log site attributes
//  b          -
//  l          -
//  time_msec  -
//
static xapi __attribute__((nonnull)) stream_write(stream * const restrict streamp, const uint64_t ids, const uint32_t base_attr, const char * const restrict b, size_t l, const long time_msec)
{
  enter;

  // enable say
  narrator * N = streamp->narrator;

  // effective attributes : category + log site + stream
  uint32_t attr = attr_combine(base_attr, streamp->attr);

  int prev = 0;
  if((attr & COLOR_OPT) && (attr & COLOR_OPT) != L_NOCOLOR)
  {
    if((attr & COLOR_OPT) == L_RED)
      sayw(COLOR(RED));
    else if((attr & COLOR_OPT) == L_GREEN)
      sayw(COLOR(GREEN));
    else if((attr & COLOR_OPT) == L_YELLOW)
      sayw(COLOR(YELLOW));
    else if((attr & COLOR_OPT) == L_BLUE)
      sayw(COLOR(BLUE));
    else if((attr & COLOR_OPT) == L_MAGENTA)
      sayw(COLOR(MAGENTA));
    else if((attr & COLOR_OPT) == L_CYAN)
      sayw(COLOR(CYAN));
    else if((attr & COLOR_OPT) == L_WHITE)
      sayw(COLOR(WHITE));

    else if((attr & COLOR_OPT) == L_BOLD_RED)
      sayw(COLOR(BOLD_RED));
    else if((attr & COLOR_OPT) == L_BOLD_GREEN)
      sayw(COLOR(BOLD_GREEN));
    else if((attr & COLOR_OPT) == L_BOLD_YELLOW)
      sayw(COLOR(BOLD_YELLOW));
    else if((attr & COLOR_OPT) == L_BOLD_BLUE)
      sayw(COLOR(BOLD_BLUE));
    else if((attr & COLOR_OPT) == L_BOLD_MAGENTA)
      sayw(COLOR(BOLD_MAGENTA));
    else if((attr & COLOR_OPT) == L_BOLD_CYAN)
      sayw(COLOR(BOLD_CYAN));
    else if((attr & COLOR_OPT) == L_BOLD_WHITE)
      sayw(COLOR(BOLD_WHITE));
  }

  if((attr & DATESTAMP_OPT) == L_DATESTAMP)
  {
    struct tm tm;
    time_t time = time_msec / 1000;
    localtime_r(&time, &tm);

    char * months[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun"
      , "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

    sayf("%4d %s %02d %02d:%02d:%02d"
      , tm.tm_year + 1900
      , months[tm.tm_mon]
      , tm.tm_mday
      , tm.tm_hour
      , tm.tm_min
      , tm.tm_sec
    );
    prev = 1;
  }

  if((attr & CATEGORY_OPT) == L_CATEGORY)
  {
    // emit the name of the category with the least id
    uint64_t bit = UINT64_C(1);
    while(bit)
    {
      if(bit & ids)
        break;

      bit <<= 1;
    }

    logger_category * category = category_byid(bit);
    if(prev)
      says(" ");
    if(category)
      sayf("%*.*s", category_name_max_length, category->namel, category->name);
    else
      sayf("%*s", category_name_max_length, "");
    prev = 1;
  }

  if((attr & PROCESSID_OPT) == L_PROCESSID)
  {
    if(prev)
      says(" ");
    sayf("%5ld/%-5ld", getpgid(0), getpid());
    if(g_logger_process_name)
      sayf("/%4s", g_logger_process_name);
    prev = 1;
  }

  if(prev)
    says(" ");

  // the mssage
  sayw(b, l);
  prev = 1;

  if((attr & DISCOVERY_OPT) == L_DISCOVERY)
  {
    // emit the names of all tagged categories
    if(ids & L_ALL)
    {
      if(prev)
        says(" ");
      prev = 1;
      says("{ ");

      uint64_t bit = UINT64_C(1);
      while(bit)
      {
        if(bit & ids)
        {
          logger_category * category = category_byid(bit);

          if((bit - 1) & ids)
            says(" | ");

          sayf("%.*s", category->namel, category->name);
        }

        bit <<= 1;
      }

      says(" }");
    }
  }

  if((attr & COLOR_OPT) && (attr & COLOR_OPT) != L_NOCOLOR)
  {
    sayw(COLOR(NONE));
  }

  // message terminator
  if(streamp->type == LOGGER_STREAM_FD || streamp->type == LOGGER_STREAM_ROLLING)
  {
    sayc('\n');
  }

  // flush
  fatal(narrator_record_write, N);

  finally : coda;
}

static xapi __attribute__((nonnull)) stream_initialize(stream * const restrict streamp, const logger_stream * restrict def)
{
  enter;

  char * expr = 0;

  streamp->id = def->id;
  if(def->name)
  {
    fatal(ixstrdup, &streamp->name, def->name);
    streamp->namel = strlen(streamp->name);
  }
  streamp->attr = def->attr;
  fatal(list_createx, &streamp->filters, filter_free, 0, 0);

  streamp->type = def->type;
  if(def->type == LOGGER_STREAM_FD)
  {
    fatal(narrator_fd_create, &streamp->narrator_owned, def->fd);
    streamp->narrator_base = streamp->narrator_owned;
  }
  else if(def->type == LOGGER_STREAM_ROLLING)
  {
    fatal(narrator_rolling_create, &streamp->narrator_owned, def->path_base, def->file_mode, def->threshold, def->max_files);
    streamp->narrator_base = streamp->narrator_owned;
  }
  else if(def->type == LOGGER_STREAM_NARRATOR)
  {
    streamp->narrator_base = def->narrator;
  }
  else
  {
    failf(LOGGER_NXSTREAM, "type", "%d", def->type);
  }
  fatal(narrator_record_create, &streamp->narrator, streamp->narrator_base);

  // save filter expressions for late binding
  fatal(list_createx, &streamp->exprs, wfree, 0, 0);

  if(def->expr)
  {
    fatal(ixstrdup, &expr, def->expr);
    fatal(list_push, streamp->exprs, expr);
    expr = 0;
  }

  if(def->exprs)
  {
    char ** exprs = def->exprs;
    while(*exprs)
    {
      fatal(ixstrdup, &expr, *exprs);
      fatal(list_push, streamp->exprs, expr);
      expr = 0;
      exprs++;
    }
  }

finally:
  wfree(expr);
coda;
}

static xapi __attribute__((nonnull)) stream_finalize(stream * const restrict streamp)
{
  enter;

  int x;
  for(x = 0; x < streamp->exprs->l; x++)
  {
    char * expr = list_get(streamp->exprs, x);
    fatal(filter_expr_process, expr, strlen(expr), streamp->filters, list_push);
  }

  finally : coda;
}

static xapi __attribute__((nonnull)) stream_xdestroy(stream * const restrict streamp)
{
  enter;

  if(streamp)
  {
    wfree(streamp->name);

    fatal(list_xfree, streamp->filters);
    fatal(narrator_xfree, streamp->narrator);
    fatal(narrator_xfree, streamp->narrator_owned);
    fatal(list_xfree, streamp->exprs);
  }

  finally : coda;
}

//
// public
//

xapi stream_setup()
{
  enter;

  fatal(array_createx, &g_streams, sizeof(stream), 0, stream_xdestroy, 0);
  fatal(list_create, &registered);
  fatal(map_create, &streams_byid);

  finally : coda;
}

xapi stream_cleanup()
{
  enter;

  fatal(array_ixfree, &g_streams);
  fatal(map_ixfree, &streams_byid);
  fatal(list_ixfree, &registered);

  finally : coda;
}

xapi streams_activate()
{
  enter;

  int x;

  if(registered)
  {
    for(x = 0; x < registered->l; x++)
    {
      logger_stream * def = list_get(registered, x);
      def->id = x + 1;

      stream * streamp = 0;
      fatal(array_push, g_streams, &streamp);
      fatal(stream_initialize, streamp, def);
      fatal(map_set, streams_byid, MM(streamp->id), streamp);
    }
  }

  fatal(list_ixfree, &registered);

  finally : coda;
}

xapi streams_finalize()
{
  enter;

  int x;
  for(x = 0; x < g_streams->l; x++)
    fatal(stream_finalize, array_get(g_streams, x));

  finally : coda;
}


xapi streams_write(const uint64_t ids, const uint32_t site_attr, const char * const restrict b, size_t l,  const long time_msec)
{
  enter;

  // base attributes : category attributes + log site attributes
  uint32_t base_attr = 0;

  // log categories
  uint64_t copy = ids;
  uint64_t bit = UINT64_C(1) << 63;
  while(copy)
  {
    if(copy & bit)
    {
      logger_category * category = category_byid(bit);
      if(category)
        base_attr = attr_combine(base_attr, category->attr);

      copy &= ~bit;
    }
    bit >>= 1;
  }

  // log site
  base_attr = attr_combine(base_attr, site_attr);

  // not properly configured - write to stderr
  if(ids == 0 || array_size(g_streams) == 0)
  {
    int __attribute__((unused)) _r = write(2, b, l);
    _r = write(2, "\n", 1);
  }
  else
  {
    int x;
    for(x = 0; x < array_size(g_streams); x++)
    {
      stream * streamp = array_get(g_streams, x);
      if(stream_would(streamp, ids))
        fatal(stream_write, streamp, ids, base_attr, b, l, time_msec);
    }
  }

  finally : coda;
}

int stream_would(const stream * const restrict streamp, const uint64_t ids)
{
  if(list_size(streamp->filters) == 0)
    return 0;

  return filters_would(streamp->filters, ids);
}

int streams_would(const uint64_t ids)
{
  if(ids == 0 || array_size(g_streams) == 0)
    return !!g_logger_default_stderr;

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

xapi stream_filter_push(stream * const restrict streamp, filter * restrict filterp)
{
  enter;

  fatal(list_push, streamp->filters, filterp);
  filterp = 0;

finally:
  filter_free(filterp);
coda;
}

xapi stream_say(stream * const restrict streamp, narrator * restrict N)
{
  enter;

  sayf("id : %"PRIu32, streamp->id);
  sayf(", type : %s", LOGGER_STREAM_STR(streamp->type));
  sayf(", name : %s", streamp->name);
  says(", attr : ");
  fatal(attr_say, streamp->attr, N);
  says(", filters : [");
  int x;
  for(x = 0; x < streamp->filters->l; x++)
  {
    says(" ");
    fatal(filter_say, list_get(streamp->filters, x), N);
  }
  says(" ]");

  if(streamp->type == LOGGER_STREAM_FD)
    sayf(", fd : %d", narrator_fd_fd(streamp->narrator_base));
  else if(streamp->type == LOGGER_STREAM_NARRATOR)
    sayf(", narrator : %p", streamp->narrator_base);

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
    fatal(list_push, registered, (void*)streams);
    streams++;
  }

  finally : coda;
}

API xapi logger_streams_report()
{
  enter;

  int token = 0;

  logs(L_LOGGER, "logger streams");

  int x;
  for(x = 0; x < g_streams->l; x++)
  {
    fatal(log_start, L_LOGGER, &token);
    narrator * N = log_narrator(&token);
    says(" ");
    fatal(stream_say, array_get(g_streams, x), N);
    fatal(log_finish, &token);
  }

finally:
  fatal(log_finish, &token);
coda;
}
