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
#include "xlinux.h"
#include "valyria/pstring.h"
#include "narrator.h"
#include "narrator/growing.h"
#include "narrator/nullity.h"

#include "internal.h"
#include "log.internal.h"
#include "stream.internal.h"

#include "macros.h"
#include "strutil.h"
#include "color.h"

#define restrict __restrict

//
// [[ static ]]
//

static __thread int         storage_log;      // whether a log is being constructed
static __thread uint64_t    storage_ids;      // when a log is being constructed, effective bits
static __thread uint32_t    storage_attrs;
static __thread narrator *  storage_narrator; // the log message
static __thread long        storage_time_msec;

/// start
//
// SUMMARY
//  store the log prefix if streams_would
//
// PARAMETERS
//  ids   - log bits
//  attrs - 
//  w     - (returns) whether streams_would
//
static xapi start(const uint64_t ids, uint32_t attrs, int * const restrict w)
{
  enter;

  if(storage_narrator == 0)
    fatal(narrator_growing_create, &storage_narrator);

  storage_log = 1;
  storage_ids = ids;
  if(streams_would(storage_ids))
  {
    (*w) = 1;

    storage_attrs = attrs;
    fatal(narrator_reset, storage_narrator);

    // wall-clock milliseconds
    struct timespec times;
    fatal(xclock_gettime, CLOCK_REALTIME, &times);
    storage_time_msec = (times.tv_sec * 1000) + (times.tv_nsec / 1000);
  }

  finally : coda;
}

static xapi finish()
{
  enter;

  // write to active streams
  fatal(streams_write
    , storage_ids
    , storage_attrs
    , narrator_growing_buffer(storage_narrator)
    , narrator_growing_size(storage_narrator)
    , storage_time_msec
  );

  finally : coda;
}

//
// public
//

void log_teardown()
{
  narrator_free(storage_narrator);
}

//
// api
//

API xapi logger_vlogf(const uint64_t ids, uint32_t attrs, const char * const fmt, va_list va)
{
  enter;

  if(storage_log)
  {
    if(streams_would(storage_ids))
      fatal(narrator_vsayf, storage_narrator, fmt, va);
  }
  else
  {
    int w = 0;
    fatal(start, ids, attrs, &w);

    if(w)
    {
      fatal(narrator_vsayf, storage_narrator, fmt, va);
      fatal(finish);
    }

    storage_log = 0;
  }

  finally : coda;
}

API xapi logger_logf(const uint64_t ids, uint32_t attrs, const char * const fmt, ...)
{
  enter;

  va_list va;
  va_start(va, fmt);
  fatal(logger_vlogf, ids, attrs, fmt, va);

finally:
  va_end(va);
coda;
}

API xapi logger_logs(const uint64_t ids, uint32_t attrs, const char * const s)
{
  enter;

  fatal(logger_logw, ids, attrs, s, strlen(s));

  finally : coda;
}

API xapi logger_logw(const uint64_t ids, uint32_t attrs, const char * const src, size_t len)
{
  enter;

  if(storage_log)
  {
    if(streams_would(storage_ids))
      fatal(narrator_sayw, storage_narrator, src, len);
  }
  else
  {
    int w = 0;
    fatal(start, ids, attrs, &w);

    if(w)
    {
      fatal(narrator_sayw, storage_narrator, src, len);
      fatal(finish);
    }

    storage_log = 0;
  }

  finally : coda;
}

API xapi log_xstart(const uint64_t ids, uint32_t attrs, int * const restrict token)
{
  enter;

  int w = 0;
  fatal(start, ids, attrs, &w);
  *token = 1;

  finally : coda;
}

API xapi log_start(const uint64_t ids, int * const restrict token)
{
  enter;

  int w = 0;
  fatal(start, ids, 0, &w);
  *token = 1;

  finally : coda;
}

API narrator * log_narrator(int * const restrict token)
{
  if(*token && streams_would(storage_ids))
    return storage_narrator;

  return g_narrator_nullity;
}

API xapi log_finish(int * const restrict token)
{
  enter;

  if(*token)
  {
    fatal(finish);
    storage_log = 0;
    *token = 0;
  }

  finally : coda;
}

API int log_would(const uint64_t ids)
{
  return streams_would(ids);
}

API int logger_log_bytes()
{
  if(storage_narrator)
    return narrator_growing_size(storage_narrator);

  return 0;
}

API int logger_log_chars()
{
  if(storage_narrator)
    return narrator_growing_size(storage_narrator);

  return 0;
}
