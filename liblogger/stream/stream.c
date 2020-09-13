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

#include "logger.h"
#include "stream.internal.h"
#include "opts.internal.h"
#include "category.internal.h"
#include "filter.internal.h"
#include "log.internal.h"
#include "logging.internal.h"
#include "config.internal.h"
#include "expr.internal.h"
#include "LOGGER.errtab.h"

#include "macros.h"
#include "common/color.h"
#include "zbuffer.h"
#include "locks.h"

// active streams
stream g_streams[LOGGER_MAX_STREAMS];
uint8_t g_streams_l;

//
// static
//

static const char * months[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun"
  , "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

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
  fatal(list_createx, &streamp->filters, 0, 0, wfree, 0);

  streamp->type = def->type;
  if(def->type == LOGGER_STREAM_FD)
  {
    fatal(narrator_fd_create, &streamp->narrator_owned_fd, def->fd);
    streamp->narrator_base = streamp->narrator_owned;
  }
  else if(def->type == LOGGER_STREAM_ROLLING)
  {
    fatal(narrator_rolling_create, &streamp->narrator_owned_rolling, def->path_base, def->file_mode, def->threshold, def->max_files);
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
  fatal(narrator_record_create, &streamp->narrator_record, streamp->narrator_base);

  // save expressions for late binding
  fatal(list_createx, &streamp->exprs, 0, 0, wfree, 0);

  if(def->expr)
  {
    fatal(ixstrdup, &expr, def->expr);
    fatal(list_push, streamp->exprs, expr, 0);
    expr = 0;
  }

  if(def->exprs)
  {
    char ** exprs = def->exprs;
    while(*exprs)
    {
      fatal(ixstrdup, &expr, *exprs);
      fatal(list_push, streamp->exprs, expr, 0);
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

  streamp->attrs = 0;
  fatal(list_recycle, streamp->filters);

  int x;
  for(x = 0; x < streamp->exprs->size; x++) {
    fatal(expr_parse, list_get(streamp->exprs, x), streamp->filters, &streamp->attrs);
  }

  finally : coda;
}

static xapi stream_xdestroy(stream *  restrict streamp)
{
  enter;

  if(streamp)
  {
    wfree(streamp->name);

    fatal(list_xfree, streamp->filters);
    fatal(narrator_record_free, streamp->narrator_record);
    if(streamp->narrator_owned) {
      if(streamp->narrator_owned->vtab == &narrator_fd_vtable) {
        fatal(narrator_fd_free, streamp->narrator_owned_fd);
      } else if(streamp->narrator_owned->vtab == &narrator_rolling_vtable) {
        fatal(narrator_rolling_free, streamp->narrator_owned_rolling);
      }
    }
    fatal(list_xfree, streamp->exprs);
  }

  finally : coda;
}

static int stream_would(const stream * restrict streamp, uint64_t ids, uint32_t base_attrs)
{
  uint32_t attrs = attrs_combine2(base_attrs, streamp->attrs);

  if((attrs & LOGGER_FILTER_OPT) == L_NOFILTER)
    return 1;

  if(streamp->filters->size == 0)
    return 0;

  return filters_would(streamp->filters, ids);
}

//
// public
//

xapi stream_setup()
{
  enter;

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

  fatal(list_ixfree, &registered);

  finally : coda;
}

xapi streams_activate()
{
  enter;

  int x;
  stream * streamp;

  if(registered)
  {
    for(x = 0; x < registered->size; x++)
    {
      logger_stream * def = list_get(registered, x);
      def->id = x + 1;

      streamp = &g_streams[g_streams_l];
      def->id = g_streams_l + 1;
      g_streams_l++;
      g_streams_l %= LOGGER_MAX_STREAMS;
      fatal(stream_initialize, streamp, def);
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
  pid_t tid;
  int prev;

  tid = gettid();
  futex_acquire(&streamp->lock, tid);

  prev = 0;
  if((attrs & LOGGER_COLOR_OPT) && (attrs & LOGGER_COLOR_OPT) != L_NOCOLOR)
  {
    if((attrs & LOGGER_COLOR_OPT) == L_RED)
      xsayw(RED);
    else if((attrs & LOGGER_COLOR_OPT) == L_GREEN)
      xsayw(GREEN);
    else if((attrs & LOGGER_COLOR_OPT) == L_YELLOW)
      xsayw(YELLOW);
    else if((attrs & LOGGER_COLOR_OPT) == L_BLUE)
      xsayw(BLUE);
    else if((attrs & LOGGER_COLOR_OPT) == L_MAGENTA)
      xsayw(MAGENTA);
    else if((attrs & LOGGER_COLOR_OPT) == L_CYAN)
      xsayw(CYAN);
    else if((attrs & LOGGER_COLOR_OPT) == L_GRAY)
      xsayw(GRAY);

    else if((attrs & LOGGER_COLOR_OPT) == L_BOLD_RED)
      xsayw(BOLD_RED);
    else if((attrs & LOGGER_COLOR_OPT) == L_BOLD_GREEN)
      xsayw(BOLD_GREEN);
    else if((attrs & LOGGER_COLOR_OPT) == L_BOLD_YELLOW)
      xsayw(BOLD_YELLOW);
    else if((attrs & LOGGER_COLOR_OPT) == L_BOLD_BLUE)
      xsayw(BOLD_BLUE);
    else if((attrs & LOGGER_COLOR_OPT) == L_BOLD_MAGENTA)
      xsayw(BOLD_MAGENTA);
    else if((attrs & LOGGER_COLOR_OPT) == L_BOLD_CYAN)
      xsayw(BOLD_CYAN);
    else if((attrs & LOGGER_COLOR_OPT) == L_BOLD_GRAY)
      xsayw(BOLD_GRAY);
  }

  if((attrs & LOGGER_DATESTAMP_OPT) == L_DATESTAMP)
  {
    struct tm tm;
    time_t time = time_msec / 1000;
    localtime_r(&time, &tm);

    if(prev)
      xsays(" ");
    xsayf("%4d %s %02d %02d:%02d:%02d"
      , tm.tm_year + 1900
      , months[tm.tm_mon]
      , tm.tm_mday
      , tm.tm_hour
      , tm.tm_min
      , tm.tm_sec
    );
    prev = 1;
  }

  if((attrs & LOGGER_CATEGORY_OPT) == L_CATEGORY)
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
      xsays(" ");
    if(category)
      xsayf("%*.*s", category_name_max_length, (int)category->namel, category->name);
    else
      xsayf("%*s", category_name_max_length, "");
    prev = 1;
  }

  if((attrs & LOGGER_NAMES_OPT) == L_NAMES)
  {
    if(prev)
      xsays(" ");
    if(*logger_process_name || *logger_thread_name)
    {
      char name[32];
      size_t namel = 0;

      if(*logger_process_name)
        namel += znloads(name + namel, sizeof(name) - namel, logger_process_name);
      if((attrs & LOGGER_PID_OPT) == L_PID)
        namel += znloadf(name + namel, sizeof(name) - namel, "[%5d]", getpid());

      if(namel && (*logger_thread_name || (attrs & LOGGER_TID_OPT) == L_TID))
        namel += znloadc(name + namel, sizeof(name) - namel, '/');

      if(*logger_thread_name)
        namel += znloads(name + namel, sizeof(name) - namel, logger_thread_name);
      if((attrs & LOGGER_TID_OPT) == L_TID)
        namel += znloadf(name + namel, sizeof(name) - namel, "[%5d]", gettid());

      xsayf("%28s", name);
      prev = 1;
    }
  }
  else if((attrs & LOGGER_PID_OPT) == L_PID)
  {
    if(prev)
      xsays(" ");
    xsayf("[%5d]", getpid());
    prev = 1;
  }

  if(prev)
    xsays(" ");

  // the message
  xsayw(b, l);
  prev = 1;

  if((attrs & LOGGER_DISCOVERY_OPT) == L_DISCOVERY)
  {
    // emit the names of all tagged, required categories
    if(prev)
      xsays(" ");
    prev = 1;
    xsays("{ ");

    bool any = false;
    uint64_t bit = UINT64_C(1);
    while(bit)
    {
      if(bit & ids & ~category_optional_mask)
      {
        logger_category * category = category_byid(bit);

        if(any)
          xsays(" | ");
        any = true;

        xsayf("%.*s", (int)category->namel, category->name);
        if(category->id & category_optional_mask)
          xsays("*");
      }

      bit <<= 1;
    }

    // tagged, optional categories
    bit = UINT64_C(1);
    while(bit)
    {
      if(bit & ids & category_optional_mask)
      {
        logger_category * category = category_byid(bit);

        if(any)
          xsays(" | ");
        any = true;

        xsayf("%.*s", (int)category->namel, category->name);
        if(category->id & category_optional_mask)
          xsays("*");
      }

      bit <<= 1;
    }

    xsays(" }");
  }

  if((attrs & LOGGER_COLOR_OPT) && (attrs & LOGGER_COLOR_OPT) != L_NOCOLOR)
  {
    xsayw(NOCOLOR);
  }

  // message terminator
  if(streamp->type == LOGGER_STREAM_FD || streamp->type == LOGGER_STREAM_ROLLING)
  {
    xsayc('\n');
  }

  fatal(narrator_record_flush, streamp->narrator_record);

finally:
  futex_release(&streamp->lock, tid);
coda;
}

xapi streams_write(const uint64_t ids, const uint32_t site_attrs, const char *  restrict b, size_t l,  const long time_msec, uint64_t vector)
{
  enter;

  uint32_t base_attrs;
  uint32_t attrs;
  int x;
  int __attribute__((unused)) r;

  // misconfigured
  if(g_streams_l == 0)
  {
    if(g_logger_default_stderr)
    {
      int __attribute__((unused)) r = write(2, b, l);
      r = write(2, "\n", 1);
    }

    goto XAPI_FINALIZE;
  }

  RUNTIME_ASSERT(ids);

  base_attrs = attrs_combine4(logger_process_attrs, logger_thread_attrs, categories_attrs(ids), site_attrs);

  for(x = 0; x < g_streams_l; x++)
  {
    stream * streamp = &g_streams[x];
    if(vector & (UINT64_C(1) << x))
    {
      attrs = attrs_combine2(base_attrs, streamp->attrs);
      fatal(stream_write, streamp, ids, attrs, b, l, time_msec);
    }
  }

  finally : coda;
}

int streams_would(const uint64_t ids, uint32_t site_attrs, uint64_t * vector)
{
  // misconfigured
  if(g_streams_l == 0)
    return g_logger_default_stderr;

  RUNTIME_ASSERT(ids);

  uint32_t base_attrs = attrs_combine4(logger_process_attrs, logger_thread_attrs, categories_attrs(ids), site_attrs);

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

  filter *f;

  xsayf("id : %"PRIu32, streamp->id);
  xsayf(", type : %s", LOGGER_STREAM_STR(streamp->type));
  xsayf(", name : %s", streamp->name);
  xsays(", attrs : ");
  fatal(attrs_say, streamp->attrs, N);
  xsays(", filters : [");
  int x;
  for(x = 0; x < streamp->filters->size; x++)
  {
    f = list_get(streamp->filters, x);
    if(f->u) {
      continue; // cannot match
    }

    xsays(" ");
    fatal(filter_say, f, N);
  }
  xsays(" ]");

  if(streamp->type == LOGGER_STREAM_FD)
  {
    xsayf(", fd : %d", streamp->narrator_base_fd->fd);
  }
  else if(streamp->type == LOGGER_STREAM_NARRATOR)
  {
    xsayf(", narrator : %p", streamp->narrator_base);
  }

  finally : coda;
}

xapi stream_expr_push(stream *  restrict streamp, const char * restrict expr)
{
  enter;

  char * e = 0;
  fatal(ixstrdup, &e, expr);
  fatal(list_push, streamp->exprs, e, 0);
  e = 0;
  fatal(stream_finalize, streamp);

finally:
  wfree(e);
coda;
}

xapi stream_expr_pop(stream *  restrict streamp)
{
  enter;

  fatal(list_pop, streamp->exprs);
  fatal(stream_finalize, streamp);

  finally : coda;
}

xapi stream_expr_unshift(stream *  restrict streamp, const char * restrict expr)
{
  enter;

  char * e = 0;
  fatal(ixstrdup, &e, expr);
  fatal(list_unshift, streamp->exprs, e, 0);
  e = 0;
  fatal(stream_finalize, streamp);

finally:
  wfree(e);
coda;
}

xapi stream_expr_shift(stream *  restrict streamp)
{
  enter;

  fatal(list_shift, streamp->exprs);
  fatal(stream_finalize, streamp);

  finally : coda;
}

xapi stream_expr_reset(stream *  restrict streamp)
{
  enter;

  fatal(list_recycle, streamp->exprs);
  fatal(stream_finalize, streamp);

  finally : coda;
}

//
// api
//

xapi API logger_stream_register(const logger_stream * restrict streams)
{
  enter;

  while(streams->type)
  {
    fatal(list_push, registered, (logger_stream*)streams, 0);
    streams++;
  }

  finally : coda;
}

xapi API logger_streams_report()
{
  enter;

  logs(L_LOGGER, "logger streams");

  int x;
  for(x = 0; x < g_streams_l; x++)
  {
    narrator * N;
    fatal(log_start, L_LOGGER, &N);
    xsays(" ");
    fatal(stream_say, &g_streams[x], N);
    fatal(log_finish);
  }

  finally : coda;
}
