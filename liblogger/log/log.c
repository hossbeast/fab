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
#include "xlinux/xtime.h"
#include "valyria/pstring.h"
#include "narrator.h"
#include "narrator/fixed.h"
#include "narrator/nullity.h"

#include "internal.h"
#include "log.internal.h"
#include "stream.internal.h"
#include "config.internal.h"

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

static __thread char storage_fixed_buffer[4096];
static __thread narrator_fixed_storage storage_fixed_storage;

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
static xapi start(uint64_t ids, uint32_t attrs, int * const restrict w)
{
  enter;

  if(storage_narrator == 0)
  {
    storage_fixed_storage.s = storage_fixed_buffer;
    storage_fixed_storage.a = sizeof(storage_fixed_buffer);
    storage_narrator = narrator_fixed_init(&storage_fixed_storage);
  }

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
    , narrator_fixed_buffer(storage_narrator)
    , narrator_fixed_size(storage_narrator)
    , storage_time_msec
  );

  finally : coda;
}

//
// public
//

xapi log_cleanup()
{
  enter;

  fatal(narrator_ixfree, &storage_narrator);

  finally : coda;
}

//
// api
//

API xapi logger_vlogf(uint64_t ids, uint32_t attrs, const char * const fmt, va_list va)
{
  enter;

  ids |= logger_process_categories;
  ids |= logger_thread_categories;

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

API xapi logger_logf(uint64_t ids, uint32_t attrs, const char * const fmt, ...)
{
  enter;

  ids |= logger_process_categories;
  ids |= logger_thread_categories;

  va_list va;
  va_start(va, fmt);
  fatal(logger_vlogf, ids, attrs, fmt, va);

finally:
  va_end(va);
coda;
}

API xapi logger_logs(uint64_t ids, uint32_t attrs, const char * const s)
{
  enter;

  ids |= logger_process_categories;
  ids |= logger_thread_categories;

  fatal(logger_logw, ids, attrs, s, strlen(s));

  finally : coda;
}

#include "category.internal.h"

API xapi logger_logw(uint64_t ids, uint32_t attrs, const char * const src, size_t len)
{
  enter;

  ids |= logger_process_categories;
  ids |= logger_thread_categories;

  if(storage_log)
  {
    if(streams_would(storage_ids))
      fatal(narrator_sayw, storage_narrator, src, len);
  }
  else
  {
    int w = 0;
    fatal(start, ids, attrs, &w);

#if 0
printf(" >> ");
uint64_t bit = UINT64_C(1);
while(bit)
{
  if(bit & ids)
  {
    logger_category * category = category_byid(bit);
    if(category)
    {
      if((bit - 1) & ids)
        printf(",");

      printf("%.*s", (int)category->namel, category->name);
    }
  }

  bit <<= 1;
}

printf(" : %d <<\n", w);
#endif

    if(w)
    {
      fatal(narrator_sayw, storage_narrator, src, len);
      fatal(finish);
    }

    storage_log = 0;
  }

  finally : coda;
}

API xapi log_xstart(uint64_t ids, uint32_t attrs, int * const restrict token)
{
  enter;

  ids |= logger_process_categories;
  ids |= logger_thread_categories;

  int w = 0;
  fatal(start, ids, attrs, &w);
  *token = 1;

  finally : coda;
}

API xapi log_start(uint64_t ids, int * const restrict token)
{
  enter;

  ids |= logger_process_categories;
  ids |= logger_thread_categories;

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

API int log_would(uint64_t ids)
{
  if(ids == 0)
    return !!g_logger_default_stderr;

  return streams_would(ids);
}

API int log_bytes()
{
  if(storage_narrator)
    return narrator_fixed_size(storage_narrator);

  return 0;
}

API int log_chars()
{
  if(storage_narrator)
    return narrator_fixed_size(storage_narrator);

  return 0;
}
