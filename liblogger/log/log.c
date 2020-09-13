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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#include "xapi.h"
#include "xapi/trace.h"
#include "xlinux/xtime.h"
#include "valyria/pstring.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "narrator/nullity.h"

#include "logger.h"
#include "log.internal.h"
#include "stream.internal.h"
#include "config.internal.h"
#include "opts.internal.h"
#include "category.internal.h"

#include "macros.h"
#include "common/color.h"

//
// [[ static ]]
//

static __thread int         storage_would;    // whether a log is being constructed
static __thread uint64_t    storage_would_vector;
static __thread uint64_t    storage_ids;      // when a log is being constructed, effective bits
static __thread uint32_t    storage_attrs;
static __thread narrator *  storage_narrator; // the log message
static __thread long        storage_time_msec;

static __thread char storage_fixed_buffer[1024*32];
static __thread narrator_fixed storage_narrator_fixed;

//
// public
//

xapi log_cleanup()
{
  enter;

  fatal(narrator_fixed_destroy, &storage_narrator_fixed);

  finally : coda;
}

//
// api
//

xapi API logger_vlogf(uint64_t ids, uint32_t attrs, const char * fmt, va_list va)
{
  enter;

  ids |= logger_process_categories;
  ids |= logger_thread_categories;

  if(storage_would)
    fatal(log_finish);

  fatal(log_xstart, ids, attrs, 0);
  if(storage_would)
    fatal(narrator_xsayvf, storage_narrator, fmt, va);
  fatal(log_finish);

  finally : coda;
}

xapi API logger_logf(uint64_t ids, uint32_t attrs, const char * fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);
  fatal(logger_vlogf, ids, attrs, fmt, va);

finally:
  va_end(va);
coda;
}

xapi API logger_logs(uint64_t ids, uint32_t attrs, const char * s)
{
  xproxy(logger_logw, ids, attrs, s, strlen(s));
}

xapi API logger_logw(uint64_t ids, uint32_t attrs, const char * src, size_t len)
{
  enter;

  ids |= logger_process_categories;
  ids |= logger_thread_categories;

  if(storage_would)
    fatal(log_finish);

  fatal(log_xstart, ids, attrs, 0);
  if(storage_would)
    fatal(narrator_xsayw, storage_narrator, src, len);

  fatal(log_finish);

  finally : coda;
}

xapi API log_xstart(uint64_t ids, uint32_t attrs, narrator ** restrict N)
{
  enter;

  struct timespec times;

  ids |= logger_process_categories;
  ids |= logger_thread_categories;

  if(storage_narrator == 0)
  {
    storage_narrator = narrator_fixed_init(&storage_narrator_fixed, storage_fixed_buffer, sizeof(storage_fixed_buffer));
  }

  storage_ids = ids;
  storage_attrs = attrs;
  if((storage_would = streams_would(storage_ids, storage_attrs, &storage_would_vector)))
  {
    fatal(narrator_xreset, storage_narrator);

    // wall-clock milliseconds
    fatal(xclock_gettime, CLOCK_REALTIME_COARSE, &times);
    storage_time_msec = (times.tv_sec * 1000) + (times.tv_nsec / 1000 / 1000);

    if(N)
      *N = storage_narrator;
  }
  else if(N)
  {
    *N = g_narrator_nullity;
  }

  finally : coda;
}

xapi API log_start(uint64_t ids, narrator ** restrict N)
{
  xproxy(log_xstart, ids, 0, N);
}

xapi API log_finish()
{
  enter;

  if(storage_would)
  {
    fatal(streams_write
      , storage_ids
      , storage_attrs
      , storage_narrator_fixed.s
      , storage_narrator_fixed.l
      , storage_time_msec
      , storage_would_vector
    );
  }
  storage_would = 0;

  finally : coda;
}

int API log_would(uint64_t ids)
{
  // misconfigured
  if(ids == 0 || g_streams_l == 0)
    return g_logger_default_stderr;

  return streams_would(ids, 0, 0);
}

int API log_xwould(uint64_t ids, uint32_t attrs)
{
  // misconfigured
  if(ids == 0 || g_streams_l == 0)
    return g_logger_default_stderr;

  return streams_would(ids, attrs, 0);
}

int API log_bytes()
{
  if(storage_narrator)
    return storage_narrator_fixed.l;

  return 0;
}

int API log_chars()
{
  if(storage_narrator)
    return storage_narrator_fixed.l;

  return 0;
}

#if XAPI_STACKTRACE
static __attribute__((nonnull)) xapi logger_trace(uint64_t ids, uint32_t site_attrs, uint16_t trace_attrs, size_t (*tracer)(char * restrict, size_t, uint16_t))
{
  enter;

  uint64_t vector = 0;
  char trace[4096];
  size_t tracesz;

  ids |= logger_process_categories;
  ids |= logger_thread_categories;

  // misconfigured
  if(ids == 0 || g_streams_l == 0)
  {
    if(g_logger_default_stderr)
    {
      tracesz = tracer(trace, sizeof(trace), trace_attrs);
      int __attribute__((unused)) r = write(2, trace, tracesz);
    }
    goto XAPI_FINALIZE;
  }

  if(streams_would(ids, site_attrs, &vector))
  {
    // wall-clock milliseconds
    struct timespec times;
    int __attribute__((unused)) r = clock_gettime(CLOCK_REALTIME, &times);
    long time_msec = (times.tv_sec * 1000) + (times.tv_nsec / 1000);

    uint32_t base_attrs = attrs_combine2(categories_attrs(ids), site_attrs);

    int x;
    for(x = 0; x < g_streams_l; x++)
    {
      stream * streamp = &g_streams[x];

      uint32_t attrs = attrs_combine2(base_attrs, streamp->attrs);
      if(vector & (UINT64_C(1) << x))
      {
        uint16_t effective_trace_attrs = trace_attrs;
        if((attrs & LOGGER_COLOR_OPT) == L_NOCOLOR)
          effective_trace_attrs &= ~XAPI_TRACE_COLORIZE;

        // stream narrator handles the newline as appropriate
        tracesz = tracer(trace, sizeof(trace), effective_trace_attrs | XAPI_TRACE_NONEWLINE);
        fatal(stream_write, streamp, ids, attrs, trace, tracesz, time_msec);
      }
    }
  }

  finally : coda;
}

xapi API logger_xtrace_full(uint64_t ids, uint32_t site_attrs, uint16_t trace_attrs)
{
  xproxy(logger_trace, ids, site_attrs, trace_attrs, xapi_trace_full);
}

xapi API logger_trace_full(uint64_t ids, uint16_t trace_attrs)
{
  xproxy(logger_trace, ids, 0, trace_attrs, xapi_trace_full);
}

xapi API logger_xtrace_pithy(uint64_t ids, uint32_t site_attrs, uint16_t trace_attrs)
{
  xproxy(logger_trace, ids, site_attrs, trace_attrs, xapi_trace_pithy);
}

xapi API logger_trace_pithy(uint64_t ids, uint16_t trace_attrs)
{
  xproxy(logger_trace, ids, 0, trace_attrs, xapi_trace_pithy);
}
#endif
