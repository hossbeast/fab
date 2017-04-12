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
#include "valyria/array.h"
#include "valyria/map.h"
#include "valyria/list.h"

#include "internal.h"
#include "stream.internal.h"
#include "attr.internal.h"
#include "category.internal.h"
#include "filter.internal.h"
#include "log.internal.h"
#include "logging.internal.h"
#include "config.internal.h"
#include "expr.internal.h"
#include "LOGGER.errtab.h"

#include "macros.h"
#include "strutil.h"
#include "color.h"
#include "spinlock.h"
#include "zbuffer.h"

#define restrict __restrict

// active streams
//array * g_streams;
//map * streams_byid;
stream g_streams[LOGGER_MAX_STREAMS];
uint8_t g_streams_l;

//
// static
//

// registered, not yet activated streams
static list * registered;

static xapi __attribute__((nonnull)) stream_initialize(stream *  restrict streamp, const logger_stream * restrict def)
{
  enter;

  char * expr = 0;

  memset(streamp, 0, sizeof(*streamp));

  streamp->id = def->id;
  if(def->name)
  {
    fatal(ixstrdup, &streamp->name, def->name);
    streamp->namel = strlen(streamp->name);
  }
  streamp->attrs = def->attr;
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

  // save expressions for late binding
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

static xapi __attribute__((nonnull)) stream_finalize(stream *  restrict streamp)
{
  enter;

  fatal(list_recycle, streamp->filters);

  int x;
  for(x = 0; x < streamp->exprs->l; x++)
    fatal(expr_parse, list_get(streamp->exprs, x), streamp->filters, &streamp->attrs);

  finally : coda;
}

static xapi __attribute__((nonnull)) stream_xdestroy(stream *  restrict streamp)
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

static int stream_would(const stream * restrict streamp, uint64_t ids, uint32_t base_attrs)
{
  uint32_t attrs = attr_combine2(base_attrs, streamp->attrs);

  if((attrs & FILTER_OPT) == L_NOFILTER)
    return 1;

  if(list_size(streamp->filters) == 0)
    return 0;

  return filters_would(streamp->filters, ids);
}

//
// public
//

xapi stream_setup()
{
  enter;

//  fatal(array_createx, &g_streams, sizeof(stream), 0, stream_xdestroy, 0);
//  fatal(map_create, &streams_byid);
  fatal(list_create, &registered);

  finally : coda;
}

xapi stream_cleanup()
{
  enter;

  int x;
  for(x = 0; x < g_streams_l; x++)
    fatal(stream_xdestroy, &g_streams[x]);
  g_streams_l = 0;

//  fatal(array_ixfree, &g_streams);
//  fatal(map_ixfree, &streams_byid);
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

      stream * streamp = &g_streams[g_streams_l];
      def->id = g_streams_l + 1;
      g_streams_l++;
      g_streams_l %= LOGGER_MAX_STREAMS;
      fatal(stream_initialize, streamp, def);

//      fatal(array_push, g_streams, &streamp);
//      fatal(map_set, streams_byid, MM(streamp->id), streamp);
    }
  }

  fatal(list_ixfree, &registered);

  finally : coda;
}

xapi streams_finalize()
{
  enter;

  int x;
  for(x = 0; x < g_streams_l; x++)
    fatal(stream_finalize, &g_streams[x]);

  finally : coda;
}

xapi stream_write(stream *  restrict streamp, const uint64_t ids, uint32_t attrs, const char * const restrict b, size_t l, const long time_msec)
{
  enter;

  // enable say
  narrator * N = streamp->narrator;

  spinlock_engage(&streamp->lock);

  int prev = 0;
  if((attrs & COLOR_OPT) && (attrs & COLOR_OPT) != L_NOCOLOR)
  {
    if((attrs & COLOR_OPT) == L_RED)
      sayw(RED);
    else if((attrs & COLOR_OPT) == L_GREEN)
      sayw(GREEN);
    else if((attrs & COLOR_OPT) == L_YELLOW)
      sayw(YELLOW);
    else if((attrs & COLOR_OPT) == L_BLUE)
      sayw(BLUE);
    else if((attrs & COLOR_OPT) == L_MAGENTA)
      sayw(MAGENTA);
    else if((attrs & COLOR_OPT) == L_CYAN)
      sayw(CYAN);
    else if((attrs & COLOR_OPT) == L_GRAY)
      sayw(GRAY);

    else if((attrs & COLOR_OPT) == L_BOLD_RED)
      sayw(BOLD_RED);
    else if((attrs & COLOR_OPT) == L_BOLD_GREEN)
      sayw(BOLD_GREEN);
    else if((attrs & COLOR_OPT) == L_BOLD_YELLOW)
      sayw(BOLD_YELLOW);
    else if((attrs & COLOR_OPT) == L_BOLD_BLUE)
      sayw(BOLD_BLUE);
    else if((attrs & COLOR_OPT) == L_BOLD_MAGENTA)
      sayw(BOLD_MAGENTA);
    else if((attrs & COLOR_OPT) == L_BOLD_CYAN)
      sayw(BOLD_CYAN);
    else if((attrs & COLOR_OPT) == L_BOLD_GRAY)
      sayw(BOLD_GRAY);
  }

  if((attrs & DATESTAMP_OPT) == L_DATESTAMP)
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

  if((attrs & CATEGORY_OPT) == L_CATEGORY)
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
      sayf("%*.*s", category_name_max_length, (int)category->namel, category->name);
    else
      sayf("%*s", category_name_max_length, "");
    prev = 1;
  }

  if((attrs & NAMES_OPT) == L_NAMES)
  {
    if(prev)
      says(" ");
    if(*logger_thread_name || *logger_process_name)
    {
      char name[14];
      size_t namel = 0;

      namel += znloads(name + namel, sizeof(name) - namel, logger_process_name);
      if(namel && *logger_thread_name)
        namel += znloadc(name + namel, sizeof(name) - namel, '/');
      namel += znloads(name + namel, sizeof(name) - namel, logger_thread_name);

      sayf("%14s", name);
      prev = 1;
    }
  }

  if(prev)
    says(" ");

  // the message
  sayw(b, l);
  prev = 1;

  if((attrs & DISCOVERY_OPT) == L_DISCOVERY)
  {
    // emit the names of all tagged categories
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

        sayf("%.*s", (int)category->namel, category->name);
      }

      bit <<= 1;
    }

    says(" }");
  }

  if((attrs & COLOR_OPT) && (attrs & COLOR_OPT) != L_NOCOLOR)
  {
    sayw(NOCOLOR);
  }

  // message terminator
  if(streamp->type == LOGGER_STREAM_FD || streamp->type == LOGGER_STREAM_ROLLING)
  {
    sayc('\n');
  }

  // flush
  fatal(narrator_record_write, N);

finally:
  spinlock_release(&streamp->lock);
coda;
}

xapi streams_write(const uint64_t ids, const uint32_t site_attrs, const char *  restrict b, size_t l,  const long time_msec, uint64_t vector)
{
  enter;

  // misconfigured
  if(ids == 0 || g_streams_l == 0)
  {
    if(g_logger_default_stderr)
    {
      int __attribute__((unused)) r = write(2, b, l);
      r = write(2, "\n", 1);
    }

    goto XAPI_FINALIZE;
  }

  uint32_t base_attrs = attr_combine4(logger_process_attrs, logger_thread_attrs, categories_attrs(ids), site_attrs);

  int x;
  for(x = 0; x < g_streams_l; x++)
  {
    stream * streamp = &g_streams[x];
    if(vector & (UINT64_C(1) << x))
    {
      uint32_t attrs = attr_combine2(base_attrs, streamp->attrs);
      fatal(stream_write, streamp, ids, attrs, b, l, time_msec);
    }
  }

  finally : coda;
}

int streams_would(const uint64_t ids, uint32_t site_attrs, uint64_t * vector)
{
  // misconfigured
  if(ids == 0 || g_streams_l == 0)
    return g_logger_default_stderr;

  uint32_t base_attrs = attr_combine4(logger_process_attrs, logger_thread_attrs, categories_attrs(ids), site_attrs);

  int x;
  for(x = 0; x < g_streams_l; x++)
  {
    stream * streamp = &g_streams[x];
    if(stream_would(streamp, ids, base_attrs))
    {
      if(vector)
        (*vector) |= UINT64_C(1) << x;
      else
        return 1;
    }
    else if(vector)
    {
      (*vector) &= ~(UINT64_C(1) << x);
    }
  }

  if(vector)
    return !!*vector;

  return 0;
}

xapi stream_say(stream *  restrict streamp, narrator * restrict N)
{
  enter;

  sayf("id : %"PRIu32, streamp->id);
  sayf(", type : %s", LOGGER_STREAM_STR(streamp->type));
  sayf(", name : %s", streamp->name);
  says(", attrs : ");
  fatal(attr_say, streamp->attrs, N);
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

xapi stream_expr_push(stream *  restrict streamp, const char * restrict expr)
{
  enter;

  char * e = 0;
  fatal(ixstrdup, &e, expr);
  fatal(list_push, streamp->exprs, e);
  e = 0;
  fatal(stream_finalize, streamp);

finally:
  wfree(e);
coda;
}

xapi stream_expr_pop(stream *  restrict streamp)
{
  enter;

  fatal(list_pop, streamp->exprs, 0);
  fatal(stream_finalize, streamp);

  finally : coda;
}

xapi stream_expr_unshift(stream *  restrict streamp, const char * restrict expr)
{
  enter;

  char * e = 0;
  fatal(ixstrdup, &e, expr);
  fatal(list_unshift, streamp->exprs, e);
  e = 0;
  fatal(stream_finalize, streamp);

finally:
  wfree(e);
coda;
}

xapi stream_expr_shift(stream *  restrict streamp)
{
  enter;

  fatal(list_shift, streamp->exprs, 0);
  fatal(stream_finalize, streamp);

  finally : coda;
}

xapi stream_expr_clear(stream *  restrict streamp)
{
  enter;

  fatal(list_recycle, streamp->filters);
  fatal(stream_finalize, streamp);

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

  logs(L_LOGGER, "logger streams");

  int x;
  for(x = 0; x < g_streams_l; x++)
  {
    narrator * N;
    fatal(log_start, L_LOGGER, &N);
    says(" ");
    fatal(stream_say, &g_streams[x], N);
    fatal(log_finish);
  }

  finally : coda;
}
